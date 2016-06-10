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

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/grpc.h>
#include <grpc_cb/server.h>
#include <grpc_cb/security/server_credentials.h>
#include "helper.h"
#include "route_guide.grpc_cb.pb.h"

using grpc_cb::Server;
using grpc_cb::ServerReader;
using grpc_cb::ServerReaderWriter;
using grpc_cb::ServerWriter;
using grpc_cb::Status;
using routeguide::Point;
using routeguide::Feature;
using routeguide::Rectangle;
using routeguide::RouteSummary;
using routeguide::RouteNote;
using std::chrono::system_clock;

float ConvertToRadians(float num) {
  return num * 3.1415926 /180;
}

float GetDistance(const Point& start, const Point& end) {
  const float kCoordFactor = 10000000.0;
  float lat_1 = start.latitude() / kCoordFactor;
  float lat_2 = end.latitude() / kCoordFactor;
  float lon_1 = start.longitude() / kCoordFactor;
  float lon_2 = end.longitude() / kCoordFactor;
  float lat_rad_1 = ConvertToRadians(lat_1);
  float lat_rad_2 = ConvertToRadians(lat_2);
  float delta_lat_rad = ConvertToRadians(lat_2-lat_1);
  float delta_lon_rad = ConvertToRadians(lon_2-lon_1);

  float a = pow(sin(delta_lat_rad/2), 2) + cos(lat_rad_1) * cos(lat_rad_2) *
            pow(sin(delta_lon_rad/2), 2);
  float c = 2 * atan2(sqrt(a), sqrt(1-a));
  int R = 6371000; // metres

  return R * c;
}

std::string GetFeatureName(const Point& point,
                           const std::vector<Feature>& feature_list) {
  for (const Feature& f : feature_list) {
    if (f.location().latitude() == point.latitude() &&
        f.location().longitude() == point.longitude()) {
      return f.name();
    }
  }
  return "";
}

class RouteGuideImpl final : public routeguide::RouteGuide::Service {
 public:
  explicit RouteGuideImpl(const std::string& db) {
    routeguide::ParseDb(db, &feature_list_);
  }

  void GetFeature(
      const Point& point,
      ::grpc_cb::ServerReplier<Feature> replier_copy) override {
    Feature feature;
    feature.set_name(GetFeatureName(point, feature_list_));
    feature.mutable_location()->CopyFrom(point);
    replier_copy.Reply(feature);
  }

  void ListFeatures(
      const routeguide::Rectangle& rectangle,
      const ::grpc_cb::ServerWriter< ::routeguide::Feature>& writer) override {
    const std::vector<Feature>& feature_list = feature_list_;
    std::thread t([&rectangle, writer, &feature_list]() {
      auto lo = rectangle.lo();
      auto hi = rectangle.hi();
      long left = (std::min)(lo.longitude(), hi.longitude());
      long right = (std::max)(lo.longitude(), hi.longitude());
      long top = (std::max)(lo.latitude(), hi.latitude());
      long bottom = (std::min)(lo.latitude(), hi.latitude());
      for (const Feature& f : feature_list) {
        if (f.location().longitude() >= left &&
            f.location().longitude() <= right &&
            f.location().latitude() >= bottom &&
            f.location().latitude() <= top) {
          writer.Write(f);  // XXX check return?
          std::this_thread::sleep_for(std::chrono::seconds(1));
        }
      }
      // Todo: auto writer.Finish(Status::OK);
    });  // thread t
    t.detach();
  }

  void RecordRoute_OnStart(
      const ::grpc_cb::ServerReader<Point, RouteSummary>& reader) override {
      record_route_result_.reset(new RecordRouteResult);
  }

  void RecordRoute_OnMsg(const Point& point,
      const ::grpc_cb::ServerReader<Point, RouteSummary>& reader) override {
    assert(record_route_result_);
    RecordRouteResult& r = *record_route_result_;
    r.point_count++;
    if (!GetFeatureName(point, feature_list_).empty()) {
      r.feature_count++;
    }
    if (r.point_count != 1) {
      r.distance += GetDistance(r.previous, point);
    }
    r.previous = point;
  }

  void RecordRoute_OnEnd(
      const ::grpc_cb::ServerReader<Point, RouteSummary>& reader) override {
    assert(record_route_result_);
    RecordRouteResult& r = *record_route_result_;
    system_clock::time_point end_time = system_clock::now();
    RouteSummary summary;
    summary.set_point_count(r.point_count);
    summary.set_feature_count(r.feature_count);
    summary.set_distance(static_cast<long>(r.distance));
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(
        end_time - r.start_time);
    summary.set_elapsed_time(secs.count());

    reader.Reply(summary);
    record_route_result_.reset();
  }

  void RouteChat(const ::grpc_cb::ServerReaderWriter<RouteNote, RouteNote>& stream) override {
    std::thread t([stream]() {
      std::vector<RouteNote> received_notes;
      RouteNote note;
      while (stream.BlockingReadOne(&note)) {
        for (const RouteNote& n : received_notes) {
          if (n.location().latitude() == note.location().latitude() &&
              n.location().longitude() == note.location().longitude()) {
            stream.Write(n);
          }
        }
        received_notes.push_back(note);
      }  // while

      // Todo: auto stream.Finish(Status::OK);
    });  // thread t
    t.detach();
  }

 private:
  std::vector<Feature> feature_list_;
  struct RecordRouteResult {
      int point_count = 0;
      int feature_count = 0;
      float distance = 0.0;
      Point previous;
      system_clock::time_point start_time = system_clock::now();
  };
  std::unique_ptr<RecordRouteResult> record_route_result_;
};

void RunServer(const std::string& db_path) {
  std::string server_address("0.0.0.0:50051");
  RouteGuideImpl service(db_path);

  Server svr;
  svr.AddListeningPort(server_address);
  svr.RegisterService(service);
  std::cout << "Server listening on " << server_address << std::endl;
  svr.BlockingRun();
}

int main(int argc, char** argv) {
  // Expect only arg: --db_path=path/to/route_guide_db.json.
  std::string db = routeguide::GetDbFileContent(argc, argv);
  RunServer(db);

  return 0;
}
