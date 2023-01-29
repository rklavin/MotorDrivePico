if (NOT TARGET interface)
    add_library(interface INTERFACE)

    target_sources(interface INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/interface.c
    )

    #target_link_libraries(fatfs INTERFACE pico_stdlib hardware_clocks hardware_spi)
    #target_include_directories(fatfs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
endif()