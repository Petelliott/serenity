/*
 * Copyright (c) 2021, Peter Elliott <pelliott@ualberta.ca>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Forward.h>
#include <LibWeb/Bindings/Wrappable.h>
#include <LibWeb/DOM/EventTarget.h>

namespace Web::Notifications {

class Notification final
    : public RefCounted<Notification>
             , public DOM::EventTarget
             , public Bindings::Wrappable {
public:
    static NonnullRefPtr<Notification> create(DOM::Window& window, const String& title, const JS::Object& options)
    {
        return adopt_ref(*new Notification(window, title, options));
    }

    static DOM::ExceptionOr<NonnullRefPtr<Notification>> create_with_global_object(Bindings::WindowObject& window, const String& title, const JS::Object& options)
    {
        return create(window.impl(), title, options);
    }

    virtual ~Notification() override;

private:
    explicit Notification(DOM::Window&, const String& title, const JS::Object& options);



};

}
