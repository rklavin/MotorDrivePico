if (NOT TARGET shell)
    add_library(shell INTERFACE)

    target_sources(shell INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/shell.cpp
    )

    #target_link_libraries(fatfs INTERFACE pico_stdlib hardware_clocks hardware_spi)
    #target_include_directories(fatfs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
endif()