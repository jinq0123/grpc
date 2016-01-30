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

#include <map>

#include "src/compiler/cpp_cb_generator.h"
#include "src/compiler/cpp_generator_helpers.h"

#include "src/compiler/config.h"

#include <sstream>

namespace grpc_cpp_cb_generator {
namespace {

template <class T>
grpc::string as_string(T x) {
  std::ostringstream out;
  out << x;
  return out.str();
}

bool NoStreaming(const grpc::protobuf::MethodDescriptor *method) {
  return !method->client_streaming() && !method->server_streaming();
}

bool ClientOnlyStreaming(const grpc::protobuf::MethodDescriptor *method) {
  return method->client_streaming() && !method->server_streaming();
}

bool ServerOnlyStreaming(const grpc::protobuf::MethodDescriptor *method) {
  return !method->client_streaming() && method->server_streaming();
}

bool BidiStreaming(const grpc::protobuf::MethodDescriptor *method) {
  return method->client_streaming() && method->server_streaming();
}

grpc::string FilenameIdentifier(const grpc::string &filename) {
  grpc::string result;
  for (unsigned i = 0; i < filename.size(); i++) {
    char c = filename[i];
    if (isalnum(c)) {
      result.push_back(c);
    } else {
      static char hex[] = "0123456789abcdef";
      result.push_back('_');
      result.push_back(hex[(c >> 4) & 0xf]);
      result.push_back(hex[c & 0xf]);
    }
  }
  return result;
}
}  // namespace

grpc::string GetHeaderPrologue(const grpc::protobuf::FileDescriptor *file,
                               const Parameters &params) {
  grpc::string output;
  {
    // Scope the output stream so it closes and finalizes output to the string.
    grpc::protobuf::io::StringOutputStream output_stream(&output);
    grpc::protobuf::io::Printer printer(&output_stream, '$');
    std::map<grpc::string, grpc::string> vars;

    vars["filename"] = file->name();
    vars["filename_identifier"] = FilenameIdentifier(file->name());
    vars["filename_base"] = grpc_generator::StripProto(file->name());

    printer.Print(vars, "// Generated by the gRPC protobuf plugin.\n");
    printer.Print(vars,
                  "// If you make any local change, they will be lost.\n");
    printer.Print(vars, "// source: $filename$\n");
    printer.Print(vars, "#ifndef GRPC_CB_$filename_identifier$__INCLUDED\n");
    printer.Print(vars, "#define GRPC_CB_$filename_identifier$__INCLUDED\n");
    printer.Print(vars, "\n");
    printer.Print(vars, "#include \"$filename_base$.pb.h\"\n");
    printer.Print(vars, "\n");
  }
  return output;
}

grpc::string GetHeaderIncludes(const grpc::protobuf::FileDescriptor *file,
                               const Parameters &params) {
  grpc::string temp =
      "#include <grpc_cb/channel_ptr.h>\n"
      "#include <grpc_cb/service_stub.h>\n"
      "\n"
      "namespace grpc_cb {\n"
      "class RpcService;\n"
      "}  // namespace grpc_cb\n\n";

  if (!file->package().empty()) {
    std::vector<grpc::string> parts =
        grpc_generator::tokenize(file->package(), ".");

    for (auto part = parts.begin(); part != parts.end(); part++) {
      temp.append("namespace ");
      temp.append(*part);
      temp.append(" {\n");
    }
    temp.append("\n");
  }

  return temp;
}

void PrintHeaderClientMethod(grpc::protobuf::io::Printer *printer,
                             const grpc::protobuf::MethodDescriptor *method,
                             std::map<grpc::string, grpc::string> *vars,
                             bool is_public) {
  (*vars)["Method"] = method->name();
  (*vars)["Request"] =
      grpc_cpp_generator::ClassName(method->input_type(), true);
  (*vars)["Response"] =
      grpc_cpp_generator::ClassName(method->output_type(), true);
  if (is_public) {
    if (NoStreaming(method)) {
      printer->Print(
          *vars,
          "::grpc_cb::Status $Method$(::grpc_cb::ClientContext* context, "
          "const $Request$& request, $Response$* response) GRPC_OVERRIDE;\n");
      printer->Print(
          *vars,
          "std::unique_ptr< ::grpc_cb::ClientAsyncResponseReader< $Response$>> "
          "Async$Method$(::grpc_cb::ClientContext* context, "
          "const $Request$& request, "
          "::grpc_cb::CompletionQueue* cq) {\n");
      printer->Indent();
      printer->Print(*vars,
                     "return std::unique_ptr< "
                     "::grpc_cb::ClientAsyncResponseReader< $Response$>>("
                     "Async$Method$Raw(context, request, cq));\n");
      printer->Outdent();
      printer->Print("}\n");
    } else if (ClientOnlyStreaming(method)) {
      printer->Print(
          *vars,
          "std::unique_ptr< ::grpc_cb::ClientWriter< $Request$>>"
          " $Method$("
          "::grpc_cb::ClientContext* context, $Response$* response) {\n");
      printer->Indent();
      printer->Print(*vars,
                     "return std::unique_ptr< ::grpc_cb::ClientWriter< $Request$>>"
                     "($Method$Raw(context, response));\n");
      printer->Outdent();
      printer->Print("}\n");
      printer->Print(*vars,
                     "std::unique_ptr< ::grpc_cb::ClientAsyncWriter< $Request$>>"
                     " Async$Method$(::grpc_cb::ClientContext* context, "
                     "$Response$* response, "
                     "::grpc_cb::CompletionQueue* cq, void* tag) {\n");
      printer->Indent();
      printer->Print(
          *vars,
          "return std::unique_ptr< ::grpc_cb::ClientAsyncWriter< $Request$>>("
          "Async$Method$Raw(context, response, cq, tag));\n");
      printer->Outdent();
      printer->Print("}\n");
    } else if (ServerOnlyStreaming(method)) {
      printer->Print(
          *vars,
          "std::unique_ptr< ::grpc_cb::ClientReader< $Response$>>"
          " $Method$(::grpc_cb::ClientContext* context, const $Request$& request)"
          " {\n");
      printer->Indent();
      printer->Print(
          *vars,
          "return std::unique_ptr< ::grpc_cb::ClientReader< $Response$>>"
          "($Method$Raw(context, request));\n");
      printer->Outdent();
      printer->Print("}\n");
      printer->Print(
          *vars,
          "std::unique_ptr< ::grpc_cb::ClientAsyncReader< $Response$>> "
          "Async$Method$("
          "::grpc_cb::ClientContext* context, const $Request$& request, "
          "::grpc_cb::CompletionQueue* cq, void* tag) {\n");
      printer->Indent();
      printer->Print(
          *vars,
          "return std::unique_ptr< ::grpc_cb::ClientAsyncReader< $Response$>>("
          "Async$Method$Raw(context, request, cq, tag));\n");
      printer->Outdent();
      printer->Print("}\n");
    } else if (BidiStreaming(method)) {
      printer->Print(
          *vars,
          "std::unique_ptr< ::grpc_cb::ClientReaderWriter< $Request$, $Response$>>"
          " $Method$(::grpc_cb::ClientContext* context) {\n");
      printer->Indent();
      printer->Print(*vars,
                     "return std::unique_ptr< "
                     "::grpc_cb::ClientReaderWriter< $Request$, $Response$>>("
                     "$Method$Raw(context));\n");
      printer->Outdent();
      printer->Print("}\n");
      printer->Print(*vars,
                     "std::unique_ptr<  ::grpc_cb::ClientAsyncReaderWriter< "
                     "$Request$, $Response$>> "
                     "Async$Method$(::grpc_cb::ClientContext* context, "
                     "::grpc_cb::CompletionQueue* cq, void* tag) {\n");
      printer->Indent();
      printer->Print(*vars,
                     "return std::unique_ptr< "
                     "::grpc_cb::ClientAsyncReaderWriter< $Request$, $Response$>>("
                     "Async$Method$Raw(context, cq, tag));\n");
      printer->Outdent();
      printer->Print("}\n");
    }
  } else {
    if (NoStreaming(method)) {
      printer->Print(*vars,
                     "::grpc_cb::ClientAsyncResponseReader< $Response$>* "
                     "Async$Method$Raw(::grpc_cb::ClientContext* context, "
                     "const $Request$& request, "
                     "::grpc_cb::CompletionQueue* cq) GRPC_OVERRIDE;\n");
    } else if (ClientOnlyStreaming(method)) {
      printer->Print(*vars,
                     "::grpc_cb::ClientWriter< $Request$>* $Method$Raw("
                     "::grpc_cb::ClientContext* context, $Response$* response) "
                     "GRPC_OVERRIDE;\n");
      printer->Print(
          *vars,
          "::grpc_cb::ClientAsyncWriter< $Request$>* Async$Method$Raw("
          "::grpc_cb::ClientContext* context, $Response$* response, "
          "::grpc_cb::CompletionQueue* cq, void* tag) GRPC_OVERRIDE;\n");
    } else if (ServerOnlyStreaming(method)) {
      printer->Print(*vars,
                     "::grpc_cb::ClientReader< $Response$>* $Method$Raw("
                     "::grpc_cb::ClientContext* context, const $Request$& request)"
                     " GRPC_OVERRIDE;\n");
      printer->Print(
          *vars,
          "::grpc_cb::ClientAsyncReader< $Response$>* Async$Method$Raw("
          "::grpc_cb::ClientContext* context, const $Request$& request, "
          "::grpc_cb::CompletionQueue* cq, void* tag) GRPC_OVERRIDE;\n");
    } else if (BidiStreaming(method)) {
      printer->Print(
          *vars,
          "::grpc_cb::ClientReaderWriter< $Request$, $Response$>* "
          "$Method$Raw(::grpc_cb::ClientContext* context) GRPC_OVERRIDE;\n");
      printer->Print(
          *vars,
          "::grpc_cb::ClientAsyncReaderWriter< $Request$, $Response$>* "
          "Async$Method$Raw(::grpc_cb::ClientContext* context, "
          "::grpc_cb::CompletionQueue* cq, void* tag) GRPC_OVERRIDE;\n");
    }
  }
}

void PrintHeaderClientMethodData(grpc::protobuf::io::Printer *printer,
                                 const grpc::protobuf::MethodDescriptor *method,
                                 std::map<grpc::string, grpc::string> *vars) {
  (*vars)["Method"] = method->name();
  printer->Print(*vars, "const ::grpc_cb::RpcMethod rpcmethod_$Method$_;\n");
}

void PrintHeaderServerMethodSync(grpc::protobuf::io::Printer *printer,
                                 const grpc::protobuf::MethodDescriptor *method,
                                 std::map<grpc::string, grpc::string> *vars) {
  (*vars)["Method"] = method->name();
  (*vars)["Request"] =
      grpc_cpp_generator::ClassName(method->input_type(), true);
  (*vars)["Response"] =
      grpc_cpp_generator::ClassName(method->output_type(), true);
  if (NoStreaming(method)) {
    printer->Print(*vars,
                   "virtual ::grpc_cb::Status $Method$("
                   "::grpc_cb::ServerContext* context, const $Request$* request, "
                   "$Response$* response);\n");
  } else if (ClientOnlyStreaming(method)) {
    printer->Print(*vars,
                   "virtual ::grpc_cb::Status $Method$("
                   "::grpc_cb::ServerContext* context, "
                   "::grpc_cb::ServerReader< $Request$>* reader, "
                   "$Response$* response);\n");
  } else if (ServerOnlyStreaming(method)) {
    printer->Print(*vars,
                   "virtual ::grpc_cb::Status $Method$("
                   "::grpc_cb::ServerContext* context, const $Request$* request, "
                   "::grpc_cb::ServerWriter< $Response$>* writer);\n");
  } else if (BidiStreaming(method)) {
    printer->Print(
        *vars,
        "virtual ::grpc_cb::Status $Method$("
        "::grpc_cb::ServerContext* context, "
        "::grpc_cb::ServerReaderWriter< $Response$, $Request$>* stream);"
        "\n");
  }
}

void PrintHeaderServerMethodAsync(
    grpc::protobuf::io::Printer *printer,
    const grpc::protobuf::MethodDescriptor *method,
    std::map<grpc::string, grpc::string> *vars) {
  (*vars)["Method"] = method->name();
  (*vars)["Request"] =
      grpc_cpp_generator::ClassName(method->input_type(), true);
  (*vars)["Response"] =
      grpc_cpp_generator::ClassName(method->output_type(), true);
  if (NoStreaming(method)) {
    printer->Print(
        *vars,
        "void Request$Method$("
        "::grpc_cb::ServerContext* context, $Request$* request, "
        "::grpc_cb::ServerAsyncResponseWriter< $Response$>* response, "
        "::grpc_cb::CompletionQueue* new_call_cq, "
        "::grpc_cb::ServerCompletionQueue* notification_cq, void *tag);\n");
  } else if (ClientOnlyStreaming(method)) {
    printer->Print(
        *vars,
        "void Request$Method$("
        "::grpc_cb::ServerContext* context, "
        "::grpc_cb::ServerAsyncReader< $Response$, $Request$>* reader, "
        "::grpc_cb::CompletionQueue* new_call_cq, "
        "::grpc_cb::ServerCompletionQueue* notification_cq, void *tag);\n");
  } else if (ServerOnlyStreaming(method)) {
    printer->Print(
        *vars,
        "void Request$Method$("
        "::grpc_cb::ServerContext* context, $Request$* request, "
        "::grpc_cb::ServerAsyncWriter< $Response$>* writer, "
        "::grpc_cb::CompletionQueue* new_call_cq, "
        "::grpc_cb::ServerCompletionQueue* notification_cq, void *tag);\n");
  } else if (BidiStreaming(method)) {
    printer->Print(
        *vars,
        "void Request$Method$("
        "::grpc_cb::ServerContext* context, "
        "::grpc_cb::ServerAsyncReaderWriter< $Response$, $Request$>* stream, "
        "::grpc_cb::CompletionQueue* new_call_cq, "
        "::grpc_cb::ServerCompletionQueue* notification_cq, void *tag);\n");
  }
}

void PrintHeaderService(grpc::protobuf::io::Printer *printer,
                        const grpc::protobuf::ServiceDescriptor *service,
                        std::map<grpc::string, grpc::string> *vars) {
  (*vars)["Service"] = service->name();

  printer->Print(*vars,
      "namespace $Service$ {\n");

  // Client side
  printer->Print(
      "class Stub : public ::grpc_cb::ServiceStub {\n"
      " public:\n");
  printer->Indent();
  printer->Print("Stub(const ::grpc_cb::ChannelPtr& channel);\n");
  for (int i = 0; i < service->method_count(); ++i) {
    PrintHeaderClientMethod(printer, service->method(i), vars, true);
  }
  printer->Outdent();
  printer->Print("\n private:\n");
  printer->Indent();
  for (int i = 0; i < service->method_count(); ++i) {
    PrintHeaderClientMethod(printer, service->method(i), vars, false);
  }
  for (int i = 0; i < service->method_count(); ++i) {
    PrintHeaderClientMethodData(printer, service->method(i), vars);
  }
  printer->Outdent();
  printer->Print("};\n");

  printer->Print("\n");
  printer->Print(
      "std::unique_ptr<Stub> NewStub(const ::grpc_cb::ChannelPtr& channel);\n");
  printer->Print("\n");

  // Server side - Synchronous
  printer->Print(
      "class Service : public ::grpc_cb::SynchronousService {\n"
      " public:\n");
  printer->Indent();
  printer->Print("Service();\n");
  printer->Print("virtual ~Service();\n");
  for (int i = 0; i < service->method_count(); ++i) {
    PrintHeaderServerMethodSync(printer, service->method(i), vars);
  }
  printer->Print("::grpc_cb::RpcService* service() GRPC_OVERRIDE GRPC_FINAL;\n");
  printer->Outdent();
  printer->Print(
      " private:\n"
      "  std::unique_ptr< ::grpc_cb::RpcService> service_;\n");
  printer->Print("};\n");

  // Server side - Asynchronous
  printer->Print(
      "class AsyncService GRPC_FINAL : public ::grpc_cb::AsynchronousService {\n"
      " public:\n");
  printer->Indent();
  (*vars)["MethodCount"] = as_string(service->method_count());
  printer->Print("explicit AsyncService();\n");
  printer->Print("~AsyncService() {};\n");
  for (int i = 0; i < service->method_count(); ++i) {
    PrintHeaderServerMethodAsync(printer, service->method(i), vars);
  }
  printer->Outdent();
  printer->Print("};\n");
  printer->Print(*vars,
      "}  // namespace $Service$\n");
}

grpc::string GetHeaderServices(const grpc::protobuf::FileDescriptor *file,
                               const Parameters &params) {
  grpc::string output;
  {
    // Scope the output stream so it closes and finalizes output to the string.
    grpc::protobuf::io::StringOutputStream output_stream(&output);
    grpc::protobuf::io::Printer printer(&output_stream, '$');
    std::map<grpc::string, grpc::string> vars;

    if (!params.services_namespace.empty()) {
      vars["services_namespace"] = params.services_namespace;
      printer.Print(vars, "\nnamespace $services_namespace$ {\n\n");
    }

    for (int i = 0; i < file->service_count(); ++i) {
      PrintHeaderService(&printer, file->service(i), &vars);
      printer.Print("\n");
    }

    if (!params.services_namespace.empty()) {
      printer.Print(vars, "}  // namespace $services_namespace$\n\n");
    }
  }
  return output;
}

grpc::string GetHeaderEpilogue(const grpc::protobuf::FileDescriptor *file,
                               const Parameters &params) {
  grpc::string output;
  {
    // Scope the output stream so it closes and finalizes output to the string.
    grpc::protobuf::io::StringOutputStream output_stream(&output);
    grpc::protobuf::io::Printer printer(&output_stream, '$');
    std::map<grpc::string, grpc::string> vars;

    vars["filename"] = file->name();
    vars["filename_identifier"] = FilenameIdentifier(file->name());

    if (!file->package().empty()) {
      std::vector<grpc::string> parts =
          grpc_generator::tokenize(file->package(), ".");

      for (auto part = parts.rbegin(); part != parts.rend(); part++) {
        vars["part"] = *part;
        printer.Print(vars, "}  // namespace $part$\n");
      }
      printer.Print(vars, "\n");
    }

    printer.Print(vars, "#endif  // GRPC_CB_$filename_identifier$__INCLUDED\n");
  }
  return output;
}

grpc::string GetSourcePrologue(const grpc::protobuf::FileDescriptor *file,
                               const Parameters &params) {
  grpc::string output;
  {
    // Scope the output stream so it closes and finalizes output to the string.
    grpc::protobuf::io::StringOutputStream output_stream(&output);
    grpc::protobuf::io::Printer printer(&output_stream, '$');
    std::map<grpc::string, grpc::string> vars;

    vars["filename"] = file->name();
    vars["filename_base"] = grpc_generator::StripProto(file->name());

    printer.Print(vars, "// Generated by the gRPC protobuf plugin.\n");
    printer.Print(vars,
                  "// If you make any local change, they will be lost.\n");
    printer.Print(vars, "// source: $filename$\n\n");
    printer.Print(vars, "#include \"$filename_base$.pb.h\"\n");
    printer.Print(vars, "#include \"$filename_base$.grpc_cb.pb.h\"\n");
    printer.Print(vars, "\n");
  }
  return output;
}

grpc::string GetSourceIncludes(const grpc::protobuf::FileDescriptor *file,
                               const Parameters &param) {
  grpc::string output;
  {
    // Scope the output stream so it closes and finalizes output to the string.
    grpc::protobuf::io::StringOutputStream output_stream(&output);
    grpc::protobuf::io::Printer printer(&output_stream, '$');
    std::map<grpc::string, grpc::string> vars;

    printer.Print(vars, "#include <grpc_cb/channel.h>\n");
    printer.Print("\n");

    if (!file->package().empty()) {
      std::vector<grpc::string> parts =
          grpc_generator::tokenize(file->package(), ".");

      for (auto part = parts.begin(); part != parts.end(); part++) {
        vars["part"] = *part;
        printer.Print(vars, "namespace $part$ {\n");
      }
    }

    printer.Print(vars, "\n");
  }
  return output;
}

void PrintSourceClientMethod(grpc::protobuf::io::Printer *printer,
                             const grpc::protobuf::MethodDescriptor *method,
                             std::map<grpc::string, grpc::string> *vars) {
  (*vars)["Method"] = method->name();
  (*vars)["Request"] =
      grpc_cpp_generator::ClassName(method->input_type(), true);
  (*vars)["Response"] =
      grpc_cpp_generator::ClassName(method->output_type(), true);
  if (NoStreaming(method)) {
    printer->Print(*vars,
                   "::grpc_cb::Status $ns$$Service$::Stub::$Method$("
                   "::grpc_cb::ClientContext* context, "
                   "const $Request$& request, $Response$* response) {\n");
    printer->Print(*vars,
                   "  return ::grpc_cb::BlockingUnaryCall(channel_.get(), "
                   "rpcmethod_$Method$_, "
                   "context, request, response);\n"
                   "}\n\n");
    printer->Print(
        *vars,
        "::grpc_cb::ClientAsyncResponseReader< $Response$>* "
        "$ns$$Service$::Stub::Async$Method$Raw(::grpc_cb::ClientContext* context, "
        "const $Request$& request, "
        "::grpc_cb::CompletionQueue* cq) {\n");
    printer->Print(*vars,
                   "  return new "
                   "::grpc_cb::ClientAsyncResponseReader< $Response$>("
                   "channel_.get(), cq, "
                   "rpcmethod_$Method$_, "
                   "context, request);\n"
                   "}\n\n");
  } else if (ClientOnlyStreaming(method)) {
    printer->Print(*vars,
                   "::grpc_cb::ClientWriter< $Request$>* "
                   "$ns$$Service$::Stub::$Method$Raw("
                   "::grpc_cb::ClientContext* context, $Response$* response) {\n");
    printer->Print(*vars,
                   "  return new ::grpc_cb::ClientWriter< $Request$>("
                   "channel_.get(), "
                   "rpcmethod_$Method$_, "
                   "context, response);\n"
                   "}\n\n");
    printer->Print(*vars,
                   "::grpc_cb::ClientAsyncWriter< $Request$>* "
                   "$ns$$Service$::Stub::Async$Method$Raw("
                   "::grpc_cb::ClientContext* context, $Response$* response, "
                   "::grpc_cb::CompletionQueue* cq, void* tag) {\n");
    printer->Print(*vars,
                   "  return new ::grpc_cb::ClientAsyncWriter< $Request$>("
                   "channel_.get(), cq, "
                   "rpcmethod_$Method$_, "
                   "context, response, tag);\n"
                   "}\n\n");
  } else if (ServerOnlyStreaming(method)) {
    printer->Print(
        *vars,
        "::grpc_cb::ClientReader< $Response$>* "
        "$ns$$Service$::Stub::$Method$Raw("
        "::grpc_cb::ClientContext* context, const $Request$& request) {\n");
    printer->Print(*vars,
                   "  return new ::grpc_cb::ClientReader< $Response$>("
                   "channel_.get(), "
                   "rpcmethod_$Method$_, "
                   "context, request);\n"
                   "}\n\n");
    printer->Print(*vars,
                   "::grpc_cb::ClientAsyncReader< $Response$>* "
                   "$ns$$Service$::Stub::Async$Method$Raw("
                   "::grpc_cb::ClientContext* context, const $Request$& request, "
                   "::grpc_cb::CompletionQueue* cq, void* tag) {\n");
    printer->Print(*vars,
                   "  return new ::grpc_cb::ClientAsyncReader< $Response$>("
                   "channel_.get(), cq, "
                   "rpcmethod_$Method$_, "
                   "context, request, tag);\n"
                   "}\n\n");
  } else if (BidiStreaming(method)) {
    printer->Print(
        *vars,
        "::grpc_cb::ClientReaderWriter< $Request$, $Response$>* "
        "$ns$$Service$::Stub::$Method$Raw(::grpc_cb::ClientContext* context) {\n");
    printer->Print(*vars,
                   "  return new ::grpc_cb::ClientReaderWriter< "
                   "$Request$, $Response$>("
                   "channel_.get(), "
                   "rpcmethod_$Method$_, "
                   "context);\n"
                   "}\n\n");
    printer->Print(
        *vars,
        "::grpc_cb::ClientAsyncReaderWriter< $Request$, $Response$>* "
        "$ns$$Service$::Stub::Async$Method$Raw(::grpc_cb::ClientContext* context, "
        "::grpc_cb::CompletionQueue* cq, void* tag) {\n");
    printer->Print(*vars,
                   "  return new "
                   "::grpc_cb::ClientAsyncReaderWriter< $Request$, $Response$>("
                   "channel_.get(), cq, "
                   "rpcmethod_$Method$_, "
                   "context, tag);\n"
                   "}\n\n");
  }
}

void PrintSourceServerMethod(grpc::protobuf::io::Printer *printer,
                             const grpc::protobuf::MethodDescriptor *method,
                             std::map<grpc::string, grpc::string> *vars) {
  (*vars)["Method"] = method->name();
  (*vars)["Request"] =
      grpc_cpp_generator::ClassName(method->input_type(), true);
  (*vars)["Response"] =
      grpc_cpp_generator::ClassName(method->output_type(), true);
  if (NoStreaming(method)) {
    printer->Print(*vars,
                   "::grpc_cb::Status $ns$$Service$::Service::$Method$("
                   "::grpc_cb::ServerContext* context, "
                   "const $Request$* request, $Response$* response) {\n");
    printer->Print("  (void) context;\n");
    printer->Print("  (void) request;\n");
    printer->Print("  (void) response;\n");
    printer->Print(
        "  return ::grpc_cb::Status("
        "::grpc_cb::StatusCode::UNIMPLEMENTED, \"\");\n");
    printer->Print("}\n\n");
  } else if (ClientOnlyStreaming(method)) {
    printer->Print(*vars,
                   "::grpc_cb::Status $ns$$Service$::Service::$Method$("
                   "::grpc_cb::ServerContext* context, "
                   "::grpc_cb::ServerReader< $Request$>* reader, "
                   "$Response$* response) {\n");
    printer->Print("  (void) context;\n");
    printer->Print("  (void) reader;\n");
    printer->Print("  (void) response;\n");
    printer->Print(
        "  return ::grpc_cb::Status("
        "::grpc_cb::StatusCode::UNIMPLEMENTED, \"\");\n");
    printer->Print("}\n\n");
  } else if (ServerOnlyStreaming(method)) {
    printer->Print(*vars,
                   "::grpc_cb::Status $ns$$Service$::Service::$Method$("
                   "::grpc_cb::ServerContext* context, "
                   "const $Request$* request, "
                   "::grpc_cb::ServerWriter< $Response$>* writer) {\n");
    printer->Print("  (void) context;\n");
    printer->Print("  (void) request;\n");
    printer->Print("  (void) writer;\n");
    printer->Print(
        "  return ::grpc_cb::Status("
        "::grpc_cb::StatusCode::UNIMPLEMENTED, \"\");\n");
    printer->Print("}\n\n");
  } else if (BidiStreaming(method)) {
    printer->Print(*vars,
                   "::grpc_cb::Status $ns$$Service$::Service::$Method$("
                   "::grpc_cb::ServerContext* context, "
                   "::grpc_cb::ServerReaderWriter< $Response$, $Request$>* "
                   "stream) {\n");
    printer->Print("  (void) context;\n");
    printer->Print("  (void) stream;\n");
    printer->Print(
        "  return ::grpc_cb::Status("
        "::grpc_cb::StatusCode::UNIMPLEMENTED, \"\");\n");
    printer->Print("}\n\n");
  }
}

void PrintSourceServerAsyncMethod(
    grpc::protobuf::io::Printer *printer,
    const grpc::protobuf::MethodDescriptor *method,
    std::map<grpc::string, grpc::string> *vars) {
  (*vars)["Method"] = method->name();
  (*vars)["Request"] =
      grpc_cpp_generator::ClassName(method->input_type(), true);
  (*vars)["Response"] =
      grpc_cpp_generator::ClassName(method->output_type(), true);
  if (NoStreaming(method)) {
    printer->Print(
        *vars,
        "void $ns$$Service$::AsyncService::Request$Method$("
        "::grpc_cb::ServerContext* context, "
        "$Request$* request, "
        "::grpc_cb::ServerAsyncResponseWriter< $Response$>* response, "
        "::grpc_cb::CompletionQueue* new_call_cq, "
        "::grpc_cb::ServerCompletionQueue* notification_cq, void *tag) {\n");
    printer->Print(*vars,
                   "  AsynchronousService::RequestAsyncUnary($Idx$, context, "
                   "request, response, new_call_cq, notification_cq, tag);\n");
    printer->Print("}\n\n");
  } else if (ClientOnlyStreaming(method)) {
    printer->Print(
        *vars,
        "void $ns$$Service$::AsyncService::Request$Method$("
        "::grpc_cb::ServerContext* context, "
        "::grpc_cb::ServerAsyncReader< $Response$, $Request$>* reader, "
        "::grpc_cb::CompletionQueue* new_call_cq, "
        "::grpc_cb::ServerCompletionQueue* notification_cq, void *tag) {\n");
    printer->Print(*vars,
                   "  AsynchronousService::RequestClientStreaming($Idx$, "
                   "context, reader, new_call_cq, notification_cq, tag);\n");
    printer->Print("}\n\n");
  } else if (ServerOnlyStreaming(method)) {
    printer->Print(
        *vars,
        "void $ns$$Service$::AsyncService::Request$Method$("
        "::grpc_cb::ServerContext* context, "
        "$Request$* request, "
        "::grpc_cb::ServerAsyncWriter< $Response$>* writer, "
        "::grpc_cb::CompletionQueue* new_call_cq, "
        "::grpc_cb::ServerCompletionQueue* notification_cq, void *tag) {\n");
    printer->Print(
        *vars,
        "  AsynchronousService::RequestServerStreaming($Idx$, "
        "context, request, writer, new_call_cq, notification_cq, tag);\n");
    printer->Print("}\n\n");
  } else if (BidiStreaming(method)) {
    printer->Print(
        *vars,
        "void $ns$$Service$::AsyncService::Request$Method$("
        "::grpc_cb::ServerContext* context, "
        "::grpc_cb::ServerAsyncReaderWriter< $Response$, $Request$>* stream, "
        "::grpc_cb::CompletionQueue* new_call_cq, "
        "::grpc_cb::ServerCompletionQueue* notification_cq, void *tag) {\n");
    printer->Print(*vars,
                   "  AsynchronousService::RequestBidiStreaming($Idx$, "
                   "context, stream, new_call_cq, notification_cq, tag);\n");
    printer->Print("}\n\n");
  }
}

void PrintSourceService(grpc::protobuf::io::Printer *printer,
                        const grpc::protobuf::ServiceDescriptor *service,
                        std::map<grpc::string, grpc::string> *vars) {
  (*vars)["Service"] = service->name();

  printer->Print(*vars,
                 "static const char* $prefix$$Service$_method_names[] = {\n");
  for (int i = 0; i < service->method_count(); ++i) {
    (*vars)["Method"] = service->method(i)->name();
    printer->Print(*vars, "  \"/$Package$$Service$/$Method$\",\n");
  }
  printer->Print(*vars, "};\n\n");

  printer->Print(*vars,
                 "std::unique_ptr< $ns$$Service$::Stub> $ns$$Service$::NewStub("
                 "const ::grpc_cb::ChannelPtr& channel) {\n"
                 "  std::unique_ptr< $ns$$Service$::Stub> stub(new "
                 "$ns$$Service$::Stub(channel));\n"
                 "  return stub;\n"
                 "}\n\n");
  printer->Print(*vars,
                 "$ns$$Service$::Stub::Stub(const ::grpc_cb::ChannelPtr& channel)\n");
  printer->Indent();
  printer->Print(": channel_(channel)");
  for (int i = 0; i < service->method_count(); ++i) {
    const grpc::protobuf::MethodDescriptor *method = service->method(i);
    (*vars)["Method"] = method->name();
    (*vars)["Idx"] = as_string(i);
    if (NoStreaming(method)) {
      (*vars)["StreamingType"] = "NORMAL_RPC";
    } else if (ClientOnlyStreaming(method)) {
      (*vars)["StreamingType"] = "CLIENT_STREAMING";
    } else if (ServerOnlyStreaming(method)) {
      (*vars)["StreamingType"] = "SERVER_STREAMING";
    } else {
      (*vars)["StreamingType"] = "BIDI_STREAMING";
    }
    printer->Print(*vars,
                   ", rpcmethod_$Method$_("
                   "$prefix$$Service$_method_names[$Idx$], "
                   "::grpc_cb::RpcMethod::$StreamingType$, "
                   "channel"
                   ")\n");
  }
  printer->Print("{}\n\n");
  printer->Outdent();

  for (int i = 0; i < service->method_count(); ++i) {
    (*vars)["Idx"] = as_string(i);
    PrintSourceClientMethod(printer, service->method(i), vars);
  }

  (*vars)["MethodCount"] = as_string(service->method_count());
  printer->Print(*vars,
                 "$ns$$Service$::AsyncService::AsyncService() : "
                 "::grpc_cb::AsynchronousService("
                 "$prefix$$Service$_method_names, $MethodCount$) "
                 "{}\n\n");

  printer->Print(*vars,
                 "$ns$$Service$::Service::Service() {\n"
                 "}\n\n");
  printer->Print(*vars,
                 "$ns$$Service$::Service::~Service() {\n"
                 "}\n\n");
  for (int i = 0; i < service->method_count(); ++i) {
    (*vars)["Idx"] = as_string(i);
    PrintSourceServerMethod(printer, service->method(i), vars);
    PrintSourceServerAsyncMethod(printer, service->method(i), vars);
  }
  printer->Print(*vars,
                 "::grpc_cb::RpcService* $ns$$Service$::Service::service() {\n");
  printer->Indent();
  printer->Print(
      "if (service_) {\n"
      "  return service_.get();\n"
      "}\n");
  printer->Print("service_ = std::unique_ptr< ::grpc_cb::RpcService>(new ::grpc_cb::RpcService());\n");
  for (int i = 0; i < service->method_count(); ++i) {
    const grpc::protobuf::MethodDescriptor *method = service->method(i);
    (*vars)["Idx"] = as_string(i);
    (*vars)["Method"] = method->name();
    (*vars)["Request"] =
        grpc_cpp_generator::ClassName(method->input_type(), true);
    (*vars)["Response"] =
        grpc_cpp_generator::ClassName(method->output_type(), true);
    if (NoStreaming(method)) {
      printer->Print(
          *vars,
          "service_->AddMethod(new ::grpc_cb::RpcServiceMethod(\n"
          "    $prefix$$Service$_method_names[$Idx$],\n"
          "    ::grpc_cb::RpcMethod::NORMAL_RPC,\n"
          "    new ::grpc_cb::RpcMethodHandler< $ns$$Service$::Service, "
          "$Request$, "
          "$Response$>(\n"
          "        std::mem_fn(&$ns$$Service$::Service::$Method$), this)));\n");
    } else if (ClientOnlyStreaming(method)) {
      printer->Print(
          *vars,
          "service_->AddMethod(new ::grpc_cb::RpcServiceMethod(\n"
          "    $prefix$$Service$_method_names[$Idx$],\n"
          "    ::grpc_cb::RpcMethod::CLIENT_STREAMING,\n"
          "    new ::grpc_cb::ClientStreamingHandler< "
          "$ns$$Service$::Service, $Request$, $Response$>(\n"
          "        std::mem_fn(&$ns$$Service$::Service::$Method$), this)));\n");
    } else if (ServerOnlyStreaming(method)) {
      printer->Print(
          *vars,
          "service_->AddMethod(new ::grpc_cb::RpcServiceMethod(\n"
          "    $prefix$$Service$_method_names[$Idx$],\n"
          "    ::grpc_cb::RpcMethod::SERVER_STREAMING,\n"
          "    new ::grpc_cb::ServerStreamingHandler< "
          "$ns$$Service$::Service, $Request$, $Response$>(\n"
          "        std::mem_fn(&$ns$$Service$::Service::$Method$), this)));\n");
    } else if (BidiStreaming(method)) {
      printer->Print(
          *vars,
          "service_->AddMethod(new ::grpc_cb::RpcServiceMethod(\n"
          "    $prefix$$Service$_method_names[$Idx$],\n"
          "    ::grpc_cb::RpcMethod::BIDI_STREAMING,\n"
          "    new ::grpc_cb::BidiStreamingHandler< "
          "$ns$$Service$::Service, $Request$, $Response$>(\n"
          "        std::mem_fn(&$ns$$Service$::Service::$Method$), this)));\n");
    }
  }
  printer->Print("return service_.get();\n");
  printer->Outdent();
  printer->Print("}\n\n");
}

grpc::string GetSourceServices(const grpc::protobuf::FileDescriptor *file,
                               const Parameters &params) {
  grpc::string output;
  {
    // Scope the output stream so it closes and finalizes output to the string.
    grpc::protobuf::io::StringOutputStream output_stream(&output);
    grpc::protobuf::io::Printer printer(&output_stream, '$');
    std::map<grpc::string, grpc::string> vars;
    // Package string is empty or ends with a dot. It is used to fully qualify
    // method names.
    vars["Package"] = file->package();
    if (!file->package().empty()) {
      vars["Package"].append(".");
    }
    if (!params.services_namespace.empty()) {
      vars["ns"] = params.services_namespace + "::";
      vars["prefix"] = params.services_namespace;
    } else {
      vars["ns"] = "";
      vars["prefix"] = "";
    }

    for (int i = 0; i < file->service_count(); ++i) {
      PrintSourceService(&printer, file->service(i), &vars);
      printer.Print("\n");
    }
  }
  return output;
}

grpc::string GetSourceEpilogue(const grpc::protobuf::FileDescriptor *file,
                               const Parameters &params) {
  grpc::string temp;

  if (!file->package().empty()) {
    std::vector<grpc::string> parts =
        grpc_generator::tokenize(file->package(), ".");

    for (auto part = parts.begin(); part != parts.end(); part++) {
      temp.append("}  // namespace ");
      temp.append(*part);
      temp.append("\n");
    }
    temp.append("\n");
  }

  return temp;
}

}  // namespace grpc_cpp_cb_generator
