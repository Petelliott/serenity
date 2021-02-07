set(SOURCES
    entry.cpp)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES SerenityOS)
    include_directories(/usr/local/include/c++/10.2.0/)
    include_directories(/usr/local/include/c++/10.2.0/${SERENITY_ARCH}-pc-serenity/)
else()
    include_directories(../Toolchain/Local/${SERENITY_ARCH}/${SERENITY_ARCH}-pc-serenity/include/c++/10.2.0/)
    include_directories(../Toolchain/Local/${SERENITY_ARCH}/${SERENITY_ARCH}-pc-serenity/include/c++/10.2.0/${SERENITY_ARCH}-pc-serenity/)
endif()

add_compile_options(-m16 -ffreestanding -fno-pie -nostdlib)
add_link_options(LINKER:-T ${CMAKE_CURRENT_BINARY_DIR}/linker.ld -nostdlib -ffreestanding -static)
file(GENERATE OUTPUT linker.ld INPUT linker.ld)

add_executable(BootLoader.img ${SOURCES})
install(TARGETS BootLoader.img RUNTIME DESTINATION boot/BootLoader)
