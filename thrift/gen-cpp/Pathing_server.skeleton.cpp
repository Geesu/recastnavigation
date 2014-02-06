// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "Pathing.h"
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::pathing;

class PathingHandler : virtual public PathingIf {
 public:
  PathingHandler() {
    // Your initialization goes here
  }

  void GetRoute(Response& _return, const Position& A, const Position& B, const RouteParameters& params) {
    // Your implementation goes here
    printf("GetRoute\n");
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  shared_ptr<PathingHandler> handler(new PathingHandler());
  shared_ptr<TProcessor> processor(new PathingProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

