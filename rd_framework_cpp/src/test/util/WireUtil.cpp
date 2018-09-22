#include <utility>

#include <utility>

//
// Created by jetbrains on 19.09.2018.
//

#include "WireUtil.h"

uint16 find_free_port() {
    CPassiveSocket fake_server;
    fake_server.Initialize();
    fake_server.Listen("127.0.0.1", 0);
    uint16 port = fake_server.GetServerPort();
    MY_ASSERT_MSG(port != 0, "no free port");
    return port;
}

void sleep_this_thread(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

Protocol server(Lifetime lifetime, uint16 port) {
    SocketWire::Server *server = new SocketWire::Server(std::move(lifetime), &testScheduler, port, "TestServer");
    std::shared_ptr<IWire> wire(server);
    return Protocol(Identities(IdKind::Server), &testScheduler, std::move(wire));
}

Protocol client(Lifetime lifetime, Protocol const &serverProtocol) {
    auto const *server = dynamic_cast<SocketWire::Server const *>(serverProtocol.wire.get());
    SocketWire::Client *client = new SocketWire::Client(std::move(lifetime), &testScheduler, server->port, "TestClient");
    std::shared_ptr<IWire> wire(client);
    return Protocol(Identities(), &testScheduler, std::move(wire));
}

Protocol client(Lifetime lifetime, uint16 port) {
    SocketWire::Client *client = new SocketWire::Client(std::move(lifetime), &testScheduler, port, "TestClient");
    std::shared_ptr<IWire> wire(client);
    return Protocol(Identities(), &testScheduler, std::move(wire));
}
