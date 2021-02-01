set(SOURCES
    main.cpp)

serenity_bin(BootLoader)
target_link_libraries(BootLoader LibCore)

add_subdirectory(BootSector)
