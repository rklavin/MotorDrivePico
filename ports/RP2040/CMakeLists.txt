if (NOT TARGET port)
    add_library(port INTERFACE)

    target_sources(port INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/port.c
            ${CMAKE_CURRENT_LIST_DIR}/FatFs/diskio.c
    )

    #target_link_libraries(fatfs INTERFACE pico_stdlib hardware_clocks hardware_spi)
    #target_include_directories(fatfs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
endif()