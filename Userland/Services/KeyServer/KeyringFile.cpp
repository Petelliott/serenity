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

#include "KeyringFile.h"
#include <LibCore/File.h>
#include <LibCrypto/Cipher/AES.h>

namespace KeyServer {

KeyringFile::KeyringFile(const StringView& path, const StringView& password)
    : m_path(path)
    , m_password(password)
{
}

bool KeyringFile::open()
{
    if (!Core::File::exists(m_path))
        return true;

    auto file_or_error = Core::File::open(m_path, Core::IODevice::OpenMode::ReadOnly);
    if (file_or_error.is_error()) {
        dbgln("KeryingFile: failed to open path {} for reading: {}", m_path, file_or_error.error());
        return false;
    }

    auto json_or_error = JsonValue::from_string(file_or_error.value()->read_all());
    if (!json_or_error.has_value()) {
        dbgln("KeryingFile: malformed json {}: {}", m_path, file_or_error.error());
        return false;
    }

    auto json = json_or_error.value().as_object();
    m_username_object = json.get("usernames/passwords").as_object();
    m_key_object = json.get("keys").as_object();

    return true;
    /*
    auto iv = ByteBuffer::create_zeroed(Crypto::Cipher::AESCipher::block_size());

    Crypto::Cipher::AESCipher::CBCMode cipher(
        m_password.bytes(),
        256,
        Crypto::Cipher::Intent::Decryption);


    auto
    */
}

bool KeyringFile::sync()
{
    auto file_or_error = Core::File::open(m_path, Core::IODevice::OpenMode::WriteOnly);
    if (file_or_error.is_error()) {
        dbgln("KeryingFile: failed to open path {} for writing: {}", m_path, file_or_error.error());
        return false;
    }

    JsonObject writeObject;
    writeObject.set("usernames/passwords", m_username_object);
    writeObject.set("keys", m_key_object);

    file_or_error.value()->write(writeObject.to_string());
    return true;
}

Optional<KeyringFile> KeyringFile::open(const StringView& path, const StringView& password)
{
    KeyringFile keyring(path, password);

    if (keyring.open())
        return keyring;

    return {};
}

}
