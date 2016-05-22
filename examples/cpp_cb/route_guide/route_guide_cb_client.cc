/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>

#include <grpc/grpc.h>
#include <grpc_cb/channel.h>
#include "helper.h"
#include "route_guide.grpc_cb.pb.h"

using grpc_cb::Channel;
using grpc_cb::ClientReader;
using grpc_cb::ClientReaderWriter;
using grpc_cb::ClientWriter;
using grpc_cb::Status;
using routeguide::Point;
using routeguide::Feature;
using routeguide::Rectangle;
using routeguide::RouteSummary;
using routeguide::RouteNote;

Point MakePoint(long latitude, long longitude) {
  Point p;
  p.set_latitude(latitude);
  p.set_longitude(longitude);
  return p;
}

Feature MakeFeature(const std::string& name,
                    long latitude, long longitude) {
  Feature f;
  f.set_name(name);
  f.mutable_location()->CopyFrom(MakePoint(latitude, longitude));
  return f;
}

RouteNote MakeRouteNote(const std::string& message,
                        long latitude, long longitude) {
  RouteNote n;
  n.set_message(message);
  n.mutable_location()->CopyFrom(MakePoint(latitude, longitude));
  return n;
}

class RouteGuideClient {
 public:
  RouteGuideClient(std::shared_ptr<Channel> channel, const std::string& db)
      : stub_(routeguide::RouteGuide::NewStub(channel)) {
    routeguide::ParseDb(db, &feature_list_);
  }

  void GetFeature() {
    Point point;
    Feature feature;
    point = MakePoint(409146138, -746188906);
    GetOneFeature(point, &feature);
    point = MakePoint(0, 0);
    GetOneFeature(point, &feature);
  }

  void ListFeatures() {
    routeguide::Rectangle rect;
    Feature feature;

    rect.mutable_lo()->set_latitude(400000000);
    rect.mutable_lo()->set_longitude(-750000000);
    rect.mutable_hi()->set_latitude(420000000);
    rect.mutable_hi()->set_longitude(-730000000);
    std::cout << "Looking for features between 40, -75 and 42, -73"
              << std::endl;

    ClientReader<Feature> reader(
        stub_->ListFeatures(rect));
    while (reader.BlockingRead(&feature)) {
      std::cout << "Found feature called "
                << feature.name() << " at "
                << feature.location().latitude()/kCoordFactor_ << ", "
                << feature.location().longitude()/kCoordFactor_ << std::endl;
    }
    Status status = reader.BlockingRecvStatus();
    if (status.ok()) {
      std::cout << "ListFeatures rpc succeeded." << std::endl;
    } else {
      std::cout << "ListFeatures rpc failed." << std::endl;
    }
  }

  void RecordRoute() {
    Point point;
    RouteSummary stats;
    const int kPoints = 10;
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> feature_distribution(
        0, feature_list_.size() - 1);
    std::uniform_int_distribution<int> delay_distribution(
        500, 1500);

    ClientWriter<Point> writer(
        stub_->RecordRoute(&stats));
    for (int i = 0; i < kPoints; i++) {
      const Feature& f = feature_list_[feature_distribution(generator)];
      std::cout << "Visiting point "
                << f.location().latitude()/kCoordFactor_ << ", "
                << f.location().longitude()/kCoordFactor_ << std::endl;
      if (!writer.Write(f.location())) {
        // Broken stream.
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(
          delay_distribution(generator)));
    }
    Status status = writer.Finish();
    if (status.ok()) {
      std::cout << "Finished trip with " << stats.point_count() << " points\n"
                << "Passed " << stats.feature_count() << " features\n"
                << "Travelled " << stats.distance() << " meters\n"
                << "It took " << stats.elapsed_time() << " seconds"
                << std::endl;
    } else {
      std::cout << "RecordRoute rpc failed." << std::endl;
    }
  }

  void RouteChat() {
    ClientReaderWriter<RouteNote, RouteNote> stream(
        stub_->RouteChat());

    std::thread writer([stream]() {
      ClientReaderWriter<RouteNote, RouteNote> stream_copy(stream);
      std::vector<RouteNote> notes{
        MakeRouteNote("First message", 0, 0),
        MakeRouteNote("Second message", 0, 1),
        MakeRouteNote("Third message", 1, 0),
        MakeRouteNote("Fourth message", 0, 0)};
      for (const RouteNote& note : notes) {
        std::cout << "Sending message " << note.message()
                  << " at " << note.location().latitude() << ", "
                  << note.location().longitude() << std::endl;
        stream_copy.Write(note);
      }
      stream_copy.WritesDone();
    });

    RouteNote server_note;
    while (stream.BlockingRead(&server_note)) {
      std::cout << "Got message " << server_note.message()
                << " at " << server_note.location().latitude() << ", "
                << server_note.location().longitude() << std::endl;
    }
    writer.join();
    Status status = stream.Finish();
    if (!status.ok()) {
      std::cout << "RouteChat rpc failed." << std::endl;
    }
  }

 private:

  bool GetOneFeature(const Point& point, Feature* feature) {
    Status status = stub_->BlockingGetFeature(point, feature);
    if (!status.ok()) {
      std::cout << "GetFeature rpc failed." << std::endl;
      return false;
    }
    if (!feature->has_location()) {
      std::cout << "Server returns incomplete feature." << std::endl;
      return false;
    }
    if (feature->name().empty()) {
      std::cout << "Found no feature at "
                << feature->location().latitude()/kCoordFactor_ << ", "
                << feature->location().longitude()/kCoordFactor_ << std::endl;
    } else {
      std::cout << "Found feature called " << feature->name()  << " at "
                << feature->location().latitude()/kCoordFactor_ << ", "
                << feature->location().longitude()/kCoordFactor_ << std::endl;
    }
    return true;
  }

  const float kCoordFactor_ = 10000000.0;
  std::unique_ptr<routeguide::RouteGuide::Stub> stub_;
  std::vector<Feature> feature_list_;
};

int main(int argc, char** argv) {
  // Expect only arg: --db_path=path/to/route_guide_db.json.
  std::string db = routeguide::GetDbFileContent(argc, argv);
  grpc_cb::ChannelSptr channel(new Channel("localhost:50051"));
  RouteGuideClient guide(channel, db);

  std::cout << "-------------- GetFeature --------------" << std::endl;
  guide.GetFeature();
  std::cout << "-------------- ListFeatures --------------" << std::endl;
  guide.ListFeatures();
  std::cout << "-------------- RecordRoute --------------" << std::endl;
  guide.RecordRoute();
  std::cout << "-------------- RouteChat --------------" << std::endl;
  guide.RouteChat();

  routeguide::RouteGuide::Stub stub(channel);
  routeguide::Rectangle rect;
  ClientReader<Feature> reader(stub.ListFeatures(rect));
  reader.SetReadCallback([](const Feature& feature){
    std::cout << "Got feature." << std::endl;
  });
  stub.BlockingRun();
  return 0;
}
