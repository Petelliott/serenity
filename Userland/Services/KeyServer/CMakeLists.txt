serenity_component(
    KeyServer
    TARGETS KeyServer
)

compile_ipc(KeyServer.ipc KeyServerEndpoint.h)
compile_ipc(KeyClient.ipc KeyClientEndpoint.h)

set(SOURCES
    ClientConnection.cpp
    KeyClientEndpoint.h
    KeyServerEndpoint.h
    KeyringFile.cpp
    main.cpp
)

serenity_bin(KeyServer)
target_link_libraries(KeyServer LibGUI LibIPC LibCrypto)
