/*
 * Copyright (c) 2021, Peter Elliott <pelliott@ualberta.ca>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/Bindings/NotificationConstructor.h>
#include <LibWeb/Bindings/NotificationObject.h>

namespace Web {
namespace Bindings {


NotificationConstructor::NotificationConstructor(JS::GlobalObject& global_object)
    : JS::NativeFunction(*global_object.function_prototype())
{
}


void NotificationConstructor::initialize(JS::GlobalObject& global_object)
{
    NativeFunction::initialize(global_object);
}

NotificationConstructor::~NotificationConstructor()
{
}

JS::Value NotificationConstructor::call()
{
    this->vm().throw_exception<JS::TypeError>(global_object(), JS::ErrorType::ConstructorWithoutNew, "Notification");
    return {};
}

JS::Value NotificationConstructor::construct(FunctionObject&)
{
    auto &vm = this->vm();
    auto title = vm.argument(0);
    auto options = vm.argument(1);

    return NotificationObject::create(global_object(), title, options);
}


}
}
