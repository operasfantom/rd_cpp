//
// Created by jetbrains on 19.09.2018.
//

#ifndef RD_CPP_WIREUTIL_H
#define RD_CPP_WIREUTIL_H

#include <util.h>
#include <thread>
#include <chrono>
#include <Lifetime.h>
#include "../../main/wire/clsocket/src/Host.h"
#include "../../main/wire/clsocket/src/PassiveSocket.h"
#include "../../main/Protocol.h"
#include "../../main/wire/SocketWire.h"
#include "RdFrameworkTestBase.h"
#include "../../main/Identities.h"

uint16 find_free_port();

void sleep_this_thread(int ms);

Protocol server(Lifetime lifetime, int32_t port = 0);

Protocol client(Lifetime lifetime, Protocol const &serverProtocol);

Protocol client(Lifetime lifetime, uint16 port);


#endif //RD_CPP_WIREUTIL_H
