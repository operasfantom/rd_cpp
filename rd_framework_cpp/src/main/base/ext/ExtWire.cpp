//
// Created by jetbrains on 28.09.2018.
//

#include "ExtWire.h"

ExtWire::ExtWire() {
    connected.advise(Lifetime::Eternal(), [](bool b) {
        if (b) {
/*
        Sync.lock(sendQ) {
            while (true) {
                val (id, payload) = sendQ.poll() ?: return@lock
                realWire.send(id) { buffer -> buffer.writeByteArrayRaw(payload) }
            }
        }
*/
        }
    });
}

void ExtWire::advise(Lifetime lifetime, IRdReactive const *entity) const {
    realWire->advise(lifetime, entity);
}

void ExtWire::send(RdId const &id, std::function<void(Buffer const &buffer)> writer) const {
/*
    Sync.lock(sendQ) {
        if (!sendQ.isEmpty() || !connected.value) {
            val buffer = createAbstractBuffer()
            writer(buffer)
            sendQ.offer(id to buffer.getArray())
            return
        }

    }
    realWire.send(id, writer)
*/
}
