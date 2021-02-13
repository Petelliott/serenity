set(SOURCES
    Keyring.cpp
)

set(GENERATED_SOURCES
    ../../Services/KeyServer/KeyClientEndpoint.h
    ../../Services/KeyServer/KeyServerEndpoint.h
)

serenity_lib(LibKeyring keyring)
target_link_libraries(LibKeyring LibIPC)
