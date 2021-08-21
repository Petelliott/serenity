/*
 * Copyright (c) 2021, Peter Elliott <pelliott@ualberta.ca>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Client.h"

namespace X {

static constexpr Window root_window = 0x01000000;
static constexpr ColorMap true_colormap = 0x02000000;

static size_t s_clients = 0;

Client::Client(NonnullRefPtr<Core::Socket> socket, Core::Object* parent)
    : IPC::ServerConnection<WindowClientEndpoint, WindowServerEndpoint>(*this, "/tmp/portal/window", parent)
    , m_socket(socket)

{
    ++s_clients;
}

void Client::die()
{
    deferred_invoke([this](auto& object) {
        NonnullRefPtr protector { object };
        remove_from_parent();
        --s_clients;
        if (s_clients == 0)
            exit(0);
    });
}

void Client::start()
{
    m_socket->on_ready_to_read = [this] {
        if (!m_handshook) {
            do_handshake();
        } else {
            handle_request();
        }
    };
}

void Client::do_handshake()
{
    auto endianness_byte = m_socket->read(sizeof(ByteOrderByte));
    if (endianness_byte.size() != sizeof(ByteOrderByte)) {
        die();
        return;
    }
    auto endianness = deserialize<ByteOrderByte>(endianness_byte, 0);

    // FIXME: Support variable endianness.
    if (endianness != ByteOrderByte::Little) {
        dbgln("Invalid Byte Order: '{}'", to_underlying(endianness));
        die();
        return;
    }

    auto setup = read_connection_setup();
    if (!setup.has_value()) {
        die();
        return;
    }

    if (setup.value().protocol_major_version != 11) {
        dbgln("Invalid X protocol version: {}.{} (Seriously? what year is it?)",
            setup.value().protocol_major_version,
            setup.value().protocol_minor_version);
        die();
        return;
    }

    if (!write_connection_success()) {
        dbgln("failed to write connection setup: {}", strerror(errno));
        die();
        return;
    }

    m_handshook = true;
}

Optional<ConnectionSetup> Client::read_connection_setup()
{
    // For some reason X11 doesn't send the connection setup as a regular
    // message with a length, so we need to handle this one specially.
    auto buffer = m_socket->read(11);
    if (buffer.size() != 11)
        return {};

    ConnectionSetup setup;

    setup.protocol_major_version = deserialize<Card16>(buffer, 1);
    setup.protocol_minor_version = deserialize<Card16>(buffer, 3);

    auto auth_name_len = deserialize<Card16>(buffer, 5);
    auto auth_data_len = deserialize<Card16>(buffer, 7);

    auto auth_name_buffer = m_socket->read(aligned(4, auth_name_len));
    if (auth_name_buffer.size() < auth_name_len)
        return {};

    setup.authorization_protocol_name = String8::deserialize_n(auth_name_buffer, 0, auth_name_len);

    auto auth_data_buffer = m_socket->read(aligned(4, auth_data_len));
    if (auth_data_buffer.size() < auth_data_len)
        return {};

    setup.authorization_protocol_data = String8::deserialize_n(auth_data_buffer, 0, auth_data_len);

    return setup;
}

bool Client::write_connection_success()
{
    ConnectionSetupSuccess setup;

    setup.protocol_major_version = 11;
    setup.protocol_minor_version = 0;
    setup.vendor = "SerenityOS XServer" Xs8;
    setup.release_number = 0;
    setup.resource_id_base = 0;
    setup.resource_id_mask = 0x00ffffff;
    setup.image_byte_order = ConnectionSetupSuccess::ByteOrder::LSBFirst;
    setup.bitmap_scanline_unit = 32;
    setup.bitmap_scanline_pad = 32;
    setup.bitmap_bit_order = ConnectionSetupSuccess::BitmapBitOrder::LeastSignificant;
    setup.pixmap_formats = { { 32, 32, 0 } };

    auto screen_layout = get_screen_layout();
    Vector<ConnectionSetupSuccess::Screen> screens;
    for (auto& screen : screen_layout.screens) {
        screens.append(ConnectionSetupSuccess::Screen {
            .root = root_window,
            .width_in_pixels = static_cast<Card16>(screen.resolution.width()),
            .height_in_pixels = static_cast<Card16>(screen.resolution.height()),
            // FIXME: Calculate this somehow.
            .width_in_millimeters = 500,
            .height_in_millimeters = 500,
            .allowed_depths = {
                ConnectionSetupSuccess::Depth {
                    32,
                    { ConnectionSetupSuccess::VisualType {
                        .visual_id = 0,
                        .clas = ConnectionSetupSuccess::VisualType::Class::TrueColor,
                        .red_mask = 0x000000ff,
                        .green_mask = 0x0000ff00,
                        .blue_mask = 0x00ff0000,
                        .bits_per_rgb_value = 32,
                        .colormap_entries = 256,
                    } } } },
            .root_depth = 0,
            .root_visual = 0,
            .default_colormap = true_colormap,
            .white_pixel = 0x00ffffff,
            .black_pixel = 0x00000000,
            .min_installed_maps = 1,
            .max_installed_maps = 1,
            .backing_stores = ConnectionSetupSuccess::Screen::BackingStores::Always,
            .save_unders = true,
            .current_input_masks = SetOf<Event>(),
        });
    }
    setup.roots = ListOf<ConnectionSetupSuccess::Screen>(screens);
    setup.motion_buffer_size = 0;
    setup.maximum_request_length = NumericLimits<Card16>::max();
    setup.min_keycode = 8;
    setup.max_keycode = 255;

    // Serialize and write the message.
    auto buffer = serialize(setup);
    return m_socket->write(buffer);
}

Optional<Request> Client::read_request()
{
    Request request;

    request.data = m_socket->read(4);
    if (request.data.size() != 4)
        return {};

    request.opcode = deserialize<Request::Opcode>(request.data, 0);
    size_t remaining_length = 4 * (deserialize<Card16>(request.data, 2) - 1);

    ByteBuffer rest = m_socket->read(remaining_length);
    if (rest.size() != remaining_length)
        return {};

    request.data.append(rest);
    request.sequence_number = ++m_sequence_number;
    return request;
}

template<typename T>
static T decode_request(Request const& request)
{
    auto full_request = deserialize<T>(request.data, 0);
    full_request.add_metadata(request);
    return full_request;
}

void Client::handle_request()
{
    auto request = read_request();
    if (!request.has_value()) {
        dbgln("failed to read request from client socket");
        die();
        return;
    }

    switch (request.value().opcode) {
    case Request::Opcode::InternAtom:
        intern_atom(decode_request<InternAtomRequest>(request.value()));
        return;
    case Request::Opcode::GetProperty:
        get_property(decode_request<GetPropertyRequest>(request.value()));
        return;
    case Request::Opcode::QueryExtension:
        query_extension(decode_request<QueryExtensionRequest>(request.value()));
        return;
    default:
        dbgln("UNKNOWN REQUEST: opcode = {}, length = {}", to_underlying(request.value().opcode), request.value().data.size());
    }
}

void Client::get_property(GetPropertyRequest const& request)
{

    GetPropertyReply reply;
    reply.format = 8;

    dbgln("GetProperty: unsupported property: {}", request.property.text());
    reply.bytes_after = 0;
    reply.type = Atom::null();
    send_reply(request, reply);
}

void Client::query_extension(QueryExtensionRequest const& request)
{
    QueryExtensionReply reply;

    dbgln("unsuported extension, name = {}", request.name.utf8().as_string());
    reply.present = false;
    send_reply(request, reply);
}

void Client::intern_atom(InternAtomRequest const& request)
{
    InternAtomReply reply;
    if (request.only_if_exists) {
        reply.atom = Atom::only_if_exists(request.name);
    } else {
        reply.atom = Atom(request.name);
    }
    send_reply(request, reply);
}

void Client::fast_greet(Vector<Gfx::IntRect> const&, u32, u32, u32, Core::AnonymousBuffer const&, String const&, String const&, i32) { }
void Client::paint(i32, Gfx::IntSize const&, Vector<Gfx::IntRect> const&) { }
void Client::mouse_move(i32, Gfx::IntPoint const&, u32, u32, u32, i32, bool, Vector<String> const&) { }
void Client::mouse_down(i32, Gfx::IntPoint const&, u32, u32, u32, i32) { }
void Client::mouse_double_click(i32, Gfx::IntPoint const&, u32, u32, u32, i32) { }
void Client::mouse_up(i32, Gfx::IntPoint const&, u32, u32, u32, i32) { }
void Client::mouse_wheel(i32, Gfx::IntPoint const&, u32, u32, u32, i32) { }
void Client::window_entered(i32) { }
void Client::window_left(i32) { }
void Client::key_down(i32, u32, u32, u32, u32) { }
void Client::key_up(i32, u32, u32, u32, u32) { }
void Client::window_activated(i32) { }
void Client::window_deactivated(i32) { }
void Client::window_input_entered(i32) { }
void Client::window_input_left(i32) { }
void Client::window_close_request(i32) { }
void Client::window_resized(i32, Gfx::IntRect const&) { }
void Client::menu_item_activated(i32, u32) { }
void Client::menu_item_entered(i32, u32) { }
void Client::menu_item_left(i32, u32) { }
void Client::menu_visibility_did_change(i32, bool) { }
void Client::screen_rects_changed(Vector<Gfx::IntRect> const&, u32, u32, u32) { }
void Client::set_wallpaper_finished(bool) { }
void Client::drag_dropped(i32, Gfx::IntPoint const&, String const&, HashMap<String, ByteBuffer> const&) { }
void Client::drag_accepted() { }
void Client::drag_cancelled() { }
void Client::update_system_theme(Core::AnonymousBuffer const&) { }
void Client::update_system_fonts(String const&, String const&) { }
void Client::window_state_changed(i32, bool, bool) { }
void Client::display_link_notification() { }
void Client::ping() { }

}
