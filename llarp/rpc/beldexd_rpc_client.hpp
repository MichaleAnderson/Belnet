#pragma once

#include <llarp/router_id.hpp>

#include <bmq/bmq.h>
#include <bmq/address.h>
#include <llarp/crypto/types.hpp>
#include <llarp/dht/key.hpp>
#include <llarp/service/name.hpp>

namespace llarp
{
  struct AbstractRouter;

  namespace rpc
  {
    using LMQ_ptr = std::shared_ptr<bmq::BMQ>;

    /// The BeldexdRpcClient uses bmq to talk to make API requests to beldexd.
    struct BeldexdRpcClient : public std::enable_shared_from_this<BeldexdRpcClient>
    {
      explicit BeldexdRpcClient(LMQ_ptr bmq, std::weak_ptr<AbstractRouter> r);

      /// Connect to beldexd async
      void
      ConnectAsync(bmq::address url);

      /// blocking request identity key from beldexd
      /// throws on failure
      SecretKey
      ObtainIdentityKey();

      /// get what the current block height is according to Beldexd
      uint64_t
      BlockHeight() const
      {
        return m_BlockHeight;
      }

      void
      LookupLNSNameHash(
          dht::Key_t namehash,
          std::function<void(std::optional<service::EncryptedName>)> resultHandler);

      /// inform that if connected to a router successfully
      void
      InformConnection(RouterID router, bool success);

     private:
      /// called when we have connected to beldexd via lokimq
      void
      Connected();

      /// do a bmq command on the current connection
      void
      Command(std::string_view cmd);

      void
      UpdateMasterNodeList();

      template <typename HandlerFunc_t, typename Args_t>
      void
      Request(std::string_view cmd, HandlerFunc_t func, const Args_t& args)
      {
        m_lokiMQ->request(*m_Connection, std::move(cmd), std::move(func), args);
      }

      template <typename HandlerFunc_t>
      void
      Request(std::string_view cmd, HandlerFunc_t func)
      {
        m_lokiMQ->request(*m_Connection, std::move(cmd), std::move(func));
      }

      void
      HandleGotMasterNodeList(std::string json);

      // Handles request from beldexd for peer stats on a specific peer
      void
      HandleGetPeerStats(bmq::Message& msg);

      // Handles notification of a new block
      void
      HandleNewBlock(bmq::Message& msg);

      std::optional<bmq::ConnectionID> m_Connection;
      LMQ_ptr m_lokiMQ;

      std::weak_ptr<AbstractRouter> m_Router;
      std::atomic<bool> m_UpdatingList;

      std::unordered_map<RouterID, PubKey> m_KeyMap;

      uint64_t m_BlockHeight;
    };

  }  // namespace rpc
}  // namespace llarp
