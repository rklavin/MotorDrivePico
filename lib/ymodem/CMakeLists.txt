if (NOT TARGET ymodem)
    add_library(ymodem INTERFACE)

    target_sources(ymodem INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/ymodem.c
    )

    #target_link_libraries(fatfs INTERFACE pico_stdlib hardware_clocks hardware_spi)
    #target_include_directories(fatfs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
endif()