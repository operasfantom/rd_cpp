//
// Created by jetbrains on 08.08.2018.
//

#ifndef RD_CPP_FRAMEWORKMARSHALLERS_H
#define RD_CPP_FRAMEWORKMARSHALLERS_H

#include "serialization/SerializationCtx.h"

namespace FrameworkMarshallers {
    class Int32 {
    private:
        int32_t predefinedId = 3;
    public:
        static int32_t read(SerializationCtx const &ctx, Buffer const &buffer);

        static void write(SerializationCtx const &ctx, Buffer const &buffer, const int32_t &value);
    };

    class Bool {
    private:
        int32_t predefinedId = 8;
    public:
        static bool read(SerializationCtx const &ctx, Buffer const &buffer);

        static void write(SerializationCtx const &ctx, Buffer const &buffer, bool const &value);
    };
}


#endif //RD_CPP_FRAMEWORKMARSHALLERS_H
