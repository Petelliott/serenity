/*
 * Copyright (c) 2021, Peter Elliott <pelliott@ualberta.ca>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibJS/Runtime/Object.h>
#include <LibWeb/Forward.h>
#include <LibGUI/Notification.h>

namespace Web {
namespace Bindings {

class NotificationObject final : public JS::Object {
    JS_OBJECT(NotificationObject, JS::Object);

public:
    explicit NotificationObject(JS::GlobalObject&, const JS::Value& title, const JS::Value& options);
    virtual void initialize(JS::GlobalObject&) override;
    virtual ~NotificationObject() override;

    static NotificationObject *create(JS::GlobalObject&, const JS::Value& title, const JS::Value& options);

private:
    RefPtr<GUI::Notification> m_notification;
};

}
}
