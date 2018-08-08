//
// Created by jetbrains on 08.08.2018.
//

#ifndef RD_CPP_FRAMEWORKMARSHALLERS_H
#define RD_CPP_FRAMEWORKMARSHALLERS_H

#include "serialization/SerializationCtx.h"

namespace FrameworkMarshallers {
    class Bool {
    public:
        static bool read(SerializationCtx const &ctx, Buffer const &buffer);

        static void write(SerializationCtx const &ctx, Buffer const &buffer, bool const &value);
    };
}


#endif //RD_CPP_FRAMEWORKMARSHALLERS_H
