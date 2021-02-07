set(SOURCES
    BootSector.s)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES SerenityOS)
    include_directories(/usr/local/include/c++/10.2.0/)
    include_directories(/usr/local/include/c++/10.2.0/${SERENITY_ARCH}-pc-serenity/)
else()
    include_directories(../Toolchain/Local/${SERENITY_ARCH}/${SERENITY_ARCH}-pc-serenity/include/c++/10.2.0/)
    include_directories(../Toolchain/Local/${SERENITY_ARCH}/${SERENITY_ARCH}-pc-serenity/include/c++/10.2.0/${SERENITY_ARCH}-pc-serenity/)
endif()

SET(ASM_OPTIONS "-m16 -ffreestanding -nostdlib")
add_link_options(-Wl,--oformat=binary -nostdlib -ffreestanding -m16 -static -fno-pic -Ttext 0x7c00)

add_executable(BootSector.img ${SOURCES})
install(TARGETS BootSector.img RUNTIME DESTINATION boot/BootLoader)
