/**
 * Autogenerated by Thrift Compiler (0.8.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef Pathing_H
#define Pathing_H

#include <TProcessor.h>
#include "pathing_types.h"

namespace pathing {

class PathingIf {
 public:
  virtual ~PathingIf() {}
  virtual void GetRoute(Response& _return, const Position& A, const Position& B, const RouteParameters& params) = 0;
};

class PathingIfFactory {
 public:
  typedef PathingIf Handler;

  virtual ~PathingIfFactory() {}

  virtual PathingIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(PathingIf* /* handler */) = 0;
};

class PathingIfSingletonFactory : virtual public PathingIfFactory {
 public:
  PathingIfSingletonFactory(const boost::shared_ptr<PathingIf>& iface) : iface_(iface) {}
  virtual ~PathingIfSingletonFactory() {}

  virtual PathingIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(PathingIf* /* handler */) {}

 protected:
  boost::shared_ptr<PathingIf> iface_;
};

class PathingNull : virtual public PathingIf {
 public:
  virtual ~PathingNull() {}
  void GetRoute(Response& /* _return */, const Position& /* A */, const Position& /* B */, const RouteParameters& /* params */) {
    return;
  }
};

typedef struct _Pathing_GetRoute_args__isset {
  _Pathing_GetRoute_args__isset() : A(false), B(false), params(false) {}
  bool A;
  bool B;
  bool params;
} _Pathing_GetRoute_args__isset;

class Pathing_GetRoute_args {
 public:

  Pathing_GetRoute_args() {
  }

  virtual ~Pathing_GetRoute_args() throw() {}

  Position A;
  Position B;
  RouteParameters params;

  _Pathing_GetRoute_args__isset __isset;

  void __set_A(const Position& val) {
    A = val;
  }

  void __set_B(const Position& val) {
    B = val;
  }

  void __set_params(const RouteParameters& val) {
    params = val;
  }

  bool operator == (const Pathing_GetRoute_args & rhs) const
  {
    if (!(A == rhs.A))
      return false;
    if (!(B == rhs.B))
      return false;
    if (!(params == rhs.params))
      return false;
    return true;
  }
  bool operator != (const Pathing_GetRoute_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Pathing_GetRoute_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Pathing_GetRoute_pargs {
 public:


  virtual ~Pathing_GetRoute_pargs() throw() {}

  const Position* A;
  const Position* B;
  const RouteParameters* params;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Pathing_GetRoute_result__isset {
  _Pathing_GetRoute_result__isset() : success(false) {}
  bool success;
} _Pathing_GetRoute_result__isset;

class Pathing_GetRoute_result {
 public:

  Pathing_GetRoute_result() {
  }

  virtual ~Pathing_GetRoute_result() throw() {}

  Response success;

  _Pathing_GetRoute_result__isset __isset;

  void __set_success(const Response& val) {
    success = val;
  }

  bool operator == (const Pathing_GetRoute_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Pathing_GetRoute_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Pathing_GetRoute_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Pathing_GetRoute_presult__isset {
  _Pathing_GetRoute_presult__isset() : success(false) {}
  bool success;
} _Pathing_GetRoute_presult__isset;

class Pathing_GetRoute_presult {
 public:


  virtual ~Pathing_GetRoute_presult() throw() {}

  Response* success;

  _Pathing_GetRoute_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class PathingClient : virtual public PathingIf {
 public:
  PathingClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  PathingClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
    piprot_(iprot),
    poprot_(oprot) {
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void GetRoute(Response& _return, const Position& A, const Position& B, const RouteParameters& params);
  void send_GetRoute(const Position& A, const Position& B, const RouteParameters& params);
  void recv_GetRoute(Response& _return);
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class PathingProcessor : public ::apache::thrift::TProcessor {
 protected:
  boost::shared_ptr<PathingIf> iface_;
  virtual bool process_fn(apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot, std::string& fname, int32_t seqid, void* callContext);
 private:
  std::map<std::string, void (PathingProcessor::*)(int32_t, apache::thrift::protocol::TProtocol*, apache::thrift::protocol::TProtocol*, void*)> processMap_;
  void process_GetRoute(int32_t seqid, apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  PathingProcessor(boost::shared_ptr<PathingIf> iface) :
    iface_(iface) {
    processMap_["GetRoute"] = &PathingProcessor::process_GetRoute;
  }

  virtual bool process(boost::shared_ptr<apache::thrift::protocol::TProtocol> piprot, boost::shared_ptr<apache::thrift::protocol::TProtocol> poprot, void* callContext);
  virtual ~PathingProcessor() {}
};

class PathingProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  PathingProcessorFactory(const ::boost::shared_ptr< PathingIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< PathingIfFactory > handlerFactory_;
};

class PathingMultiface : virtual public PathingIf {
 public:
  PathingMultiface(std::vector<boost::shared_ptr<PathingIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~PathingMultiface() {}
 protected:
  std::vector<boost::shared_ptr<PathingIf> > ifaces_;
  PathingMultiface() {}
  void add(boost::shared_ptr<PathingIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void GetRoute(Response& _return, const Position& A, const Position& B, const RouteParameters& params) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        ifaces_[i]->GetRoute(_return, A, B, params);
        return;
      } else {
        ifaces_[i]->GetRoute(_return, A, B, params);
      }
    }
  }

};

} // namespace

#endif
