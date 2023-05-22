set(DEMO_DIR ${CMAKE_CURRENT_SOURCE_DIR}) # Set root directory

add_executable(i2c_demo
    ${DEMO_DIR}/i2c_demo.c
)

target_include_directories(i2c_demo PUBLIC
    ${BOARD_INC_DIR}
    ${UTILS_INC_DIR}
)

target_link_libraries(i2c_demo PRIVATE
    BOARD_LIB
    UTILS_LIB
)

add_executable(spi_demo
    ${DEMO_DIR}/spi_demo.c
)

target_include_directories(spi_demo PUBLIC
    ${BOARD_INC_DIR}
    ${UTILS_INC_DIR}
)

target_link_libraries(spi_demo PRIVATE
    BOARD_LIB
    UTILS_LIB
)

add_executable(rtc_demo
    ${DEMO_DIR}/rtc_demo.c
)

target_include_directories(rtc_demo PUBLIC
    ${BOARD_INC_DIR}
    ${UTILS_INC_DIR}
)

target_link_libraries(rtc_demo PRIVATE
    BOARD_LIB
    UTILS_LIB
)