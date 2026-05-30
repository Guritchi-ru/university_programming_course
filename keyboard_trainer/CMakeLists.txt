cmake_minimum_required(VERSION 3.10)
project(KeyboardTrainer C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build unit tests" ON)

include_directories(src)

add_library(trainer_lib STATIC
    src/trainer.c
    src/stats.c
    src/logger.c
    src/config_parser.c
)

add_executable(keyboard_trainer src/main.c)
target_link_libraries(keyboard_trainer trainer_lib m)

if(BUILD_TESTS)
    find_path(CMOCKA_INCLUDE_DIR
        NAMES cmocka.h
        PATHS
            /usr/include
            /usr/local/include
            /opt/local/include
            /usr/include/x86_64-linux-gnu
    )
    
    find_library(CMOCKA_LIBRARY
        NAMES cmocka libcmocka
        PATHS
            /usr/lib
            /usr/local/lib
            /opt/local/lib
            /usr/lib/x86_64-linux-gnu
    )
    
    if(CMOCKA_INCLUDE_DIR AND CMOCKA_LIBRARY)
        message(STATUS "Found CMocka: ${CMOCKA_INCLUDE_DIR}, ${CMOCKA_LIBRARY}")
        set(CMOCKA_FOUND TRUE)
    else()
        message(WARNING "CMocka not found. Tests will be disabled.")
        set(CMOCKA_FOUND FALSE)
    endif()
    
    if(CMOCKA_FOUND)
        add_executable(test_stats tests/test_stats.c)
        add_executable(test_trainer tests/test_trainer.c)
        add_executable(test_config tests/test_config.c)
        
        target_link_libraries(test_stats trainer_lib ${CMOCKA_LIBRARY} m)
        target_link_libraries(test_trainer trainer_lib ${CMOCKA_LIBRARY} m)
        target_link_libraries(test_config trainer_lib ${CMOCKA_LIBRARY} m)
        
        target_include_directories(test_stats PRIVATE src ${CMOCKA_INCLUDE_DIR})
        target_include_directories(test_trainer PRIVATE src ${CMOCKA_INCLUDE_DIR})
        target_include_directories(test_config PRIVATE src ${CMOCKA_INCLUDE_DIR})
        
        enable_testing()
        add_test(NAME StatsTest COMMAND test_stats)
        add_test(NAME TrainerTest COMMAND test_trainer)
        add_test(NAME ConfigTest COMMAND test_config)
        
        message(STATUS "Tests enabled with CMocka")
    else()
        message(STATUS "Building without tests (CMocka not found)")
    endif()
endif()