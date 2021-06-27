/*
 * Copyright (c) 2021, Peter Elliott <pelliott@ualberta.ca>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <LibKeyring/Keyring.h>

namespace Keyring {

using ServerConnection = IPC::ServerConnection<KeyClientEndpoint, KeyServerEndpoint>;

Keyring::Keyring()
    : ServerConnection(*this, "/tmp/portal/keyserver")
{
}

bool Keyring::add_username_password(const StringView& id, const StringView& username, const StringView& password)
{
    return ServerConnection::add_username_password(id, username, password);
}

UsernamePassword Keyring::get_username_password(const StringView& id)
{
    auto result = ServerConnection::get_username_password(id);
    return UsernamePassword {
        .success = result.success(),
        .exists = result.exists(),
        .username = result.username(),
        .password = result.password()
    };
}

bool Keyring::add_key(const StringView& id, const StringView& key)
{
    return ServerConnection::add_key(id, key);
}

Key Keyring::get_key(const StringView& id)
{
    auto result = ServerConnection::get_key(id);
    return Key {
        .success = result.success(),
        .exists = result.exists(),
        .key = result.key()
    };
}

}
