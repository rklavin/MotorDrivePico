if (NOT TARGET components)
    add_library(components INTERFACE)

    target_sources(components INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/components.cpp
            ${CMAKE_CURRENT_LIST_DIR}/MAX11645.cpp
            ${CMAKE_CURRENT_LIST_DIR}/DRV8876.cpp
            ${CMAKE_CURRENT_LIST_DIR}/MCP47A1.cpp
    )

    #target_link_libraries(fatfs INTERFACE pico_stdlib hardware_clocks hardware_spi)
    #target_include_directories(fatfs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
endif()