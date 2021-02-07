set(SOURCES
    main.cpp)

serenity_bin(BootLoader)
target_link_libraries(BootLoader LibCore)

add_subdirectory(BootSector)
add_subdirectory(stage2)
