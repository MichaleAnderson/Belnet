#pragma once

#include <string_view>
#include <bmq/bmq.h>
#include <bmq/address.h>

namespace llarp
{
  struct AbstractRouter;
}

namespace llarp::rpc
{
  using LMQ_ptr = std::shared_ptr<bmq::BMQ>;

  struct RpcServer
  {
    explicit RpcServer(LMQ_ptr, AbstractRouter*);
    ~RpcServer() = default;
    void
    AsyncServeRPC(bmq::address addr);

   private:
    LMQ_ptr m_LMQ;
    AbstractRouter* const m_Router;
  };
}  // namespace llarp::rpc
