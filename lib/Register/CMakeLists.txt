if (NOT TARGET register)
    add_library(register INTERFACE)

    target_sources(register INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/register.cpp
    )

    #target_link_libraries(fatfs INTERFACE pico_stdlib hardware_clocks hardware_spi)
    #target_include_directories(fatfs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
endif()