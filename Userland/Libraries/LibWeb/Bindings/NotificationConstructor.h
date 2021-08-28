/*
 * Copyright (c) 2021, Peter Elliott <pelliott@ualberta.ca>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibJS/Runtime/NativeFunction.h>
#include <LibJS/Runtime/GlobalObject.h>

namespace Web {
namespace Bindings {

class NotificationConstructor final : public JS::NativeFunction {
    JS_OBJECT(NotificationObject, NativeFunction);

public:
    explicit NotificationConstructor(JS::GlobalObject&);
    virtual void initialize(JS::GlobalObject&) override;
    virtual ~NotificationConstructor() override;

    virtual JS::Value call() override;
    virtual JS::Value construct(JS::FunctionObject&) override;

private:
    virtual bool has_constructor() const override { return true; }
};

}
}
