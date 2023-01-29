if (NOT TARGET console)
    add_library(console INTERFACE)

    target_sources(console INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/console.cpp
    )

    #target_link_libraries(fatfs INTERFACE pico_stdlib hardware_clocks hardware_spi)
    #target_include_directories(fatfs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
endif()