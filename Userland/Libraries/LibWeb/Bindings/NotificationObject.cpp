/*
 * Copyright (c) 2021, Peter Elliott <pelliott@ualberta.ca>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/Bindings/NotificationObject.h>
#include <LibJS/Runtime/GlobalObject.h>

namespace Web {
namespace Bindings {

NotificationObject* NotificationObject::create(JS::GlobalObject& global_object, const JS::Value& title, const JS::Value& options)
{
    return global_object.heap().allocate<NotificationObject>(global_object, global_object, title, options);
}

NotificationObject::NotificationObject(JS::GlobalObject& global_object, const JS::Value& title, const JS::Value&)
    : Object(*global_object.object_prototype())
{
    m_notification->set_title(title.as_string().string());
    m_notification->show();
}

void NotificationObject::initialize(JS::GlobalObject& global_object)
{
    Object::initialize(global_object);
}

NotificationObject::~NotificationObject()
{
}

}
}
