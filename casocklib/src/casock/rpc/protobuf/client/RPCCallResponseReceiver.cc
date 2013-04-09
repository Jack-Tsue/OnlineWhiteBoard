/*
 * casocklib - An asynchronous communication library for C++
 * ---------------------------------------------------------
 * Copyright (C) 2010 Leandro Costa
 *
 * This file is part of casocklib.
 *
 * casocklib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * casocklib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with casocklib. If not, see <http://www.gnu.org/licenses/>.
 */

/*!
 * \file casock/rpc/protobuf/client/RPCCallResponseReceiver.cc
 * \brief [brief description]
 * \author Leandro Costa
 * \date 2010
 *
 * $LastChangedDate$
 * $LastChangedBy$
 * $Revision$
 */

#include "casock/rpc/protobuf/client/RPCCallResponseReceiver.h"

#include <glog/logging.h>

#include "casock/rpc/protobuf/api/rpc.pb.h"
#include "casock/rpc/protobuf/client/RPCCall.h"
#include "casock/rpc/protobuf/client/RPCCallHash.h"
#include "casock/rpc/protobuf/client/RPCCallQueue.h"

namespace casock {
  namespace rpc {
    namespace protobuf {
      namespace client {
        RPCCallResponseReceiver::RPCCallResponseReceiver (RPCCallHash& rCallHash, RPCCallQueue& rCallQueue)
          : mrCallHash (rCallHash), mrCallQueue (rCallQueue)
        { }

        void RPCCallResponseReceiver::handle (casock::rpc::protobuf::api::RpcResponse* pRpcResponse)
        {
          casock::rpc::protobuf::client::RPCCall* pCall = mrCallHash.pop (pRpcResponse->id ());

          if (pCall)
          {
            DLOG(INFO) << "receiving response for call ID " << pRpcResponse->id ();
            pCall->setRpcResponse (pRpcResponse);
            mrCallQueue.push (pCall);
          }
          else
          {
            LOG(WARNING) << "request not found for call ID " << pRpcResponse->id () << ", discarding response!";
            delete pRpcResponse;
          }
        }
      }
    }
  }
}
