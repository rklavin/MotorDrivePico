if (NOT TARGET storage)
    add_library(storage INTERFACE)

    target_sources(storage INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/storage.c
    )

    #target_link_libraries(fatfs INTERFACE pico_stdlib hardware_clocks hardware_spi)
    #target_include_directories(fatfs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
endif()