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

#include <LibCore/ArgsParser.h>
#include <LibCore/GetPassword.h>
#include <LibKeyring/Keyring.h>

int main(int argc, char** argv)
{
    bool set = false;
    const char* username = nullptr;
    const char* id = nullptr;

    auto args_parser = Core::ArgsParser();
    args_parser.set_general_help("Query or modify the system keyring.");
    args_parser.add_option(set, "insert a value into the keyring", "set", 's');
    args_parser.add_option(username, "username when inserting", "username", 'u', "username");
    args_parser.add_positional_argument(id, "reference id", "id");

    args_parser.parse(argc, argv);

    // We don't actually recive messages from KeyServer, so we don't need to run the EventLoop.
    Core::EventLoop event_loop;
    auto keyring = Keyring::Keyring::construct();

    if (set) {
        if (username) {
            auto password = Core::get_password("Password: ");
            if (password.is_error()) {
                warnln("{}", password.error());
                return 1;
            }

            if (!keyring->addUsernamePassword(id, username, password.value())) {
                warnln("unable to access keyring");
                return 1;
            }
        } else {
            auto key = Core::get_password("Key: ");
            if (key.is_error()) {
                warnln("{}", key.error());
                return 1;
            }

            if (!keyring->addKey(id, key.value())) {
                warnln("unable to access keyring");
                return 1;
            }
        }
    } else {
        auto username_password = keyring->getUsernamePassword(id);
        if (!username_password.success) {
            warnln("unable to access keyring");
            return 1;
        }

        auto key = keyring->getKey(id);
        if (!key.success) {
            warnln("unable to access keyring");
            return 1;
        }

        if (username_password.exists && key.exists) {
            outln("username/password:");
            outln("    '{}':'{}'", username_password.username, username_password.password);
            outln("key:");
            outln("    {}", key.key);
        } else if (username_password.exists) {
            outln("{}", username_password.username);
            outln("{}", username_password.password);
        } else if (key.exists) {
            outln("{}", key.key);
        }
        return 1;
    }

    return 0;
}
