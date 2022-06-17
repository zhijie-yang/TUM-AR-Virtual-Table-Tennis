project(tennis_tests)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        libs::tennis
)

add_test(tennis_tests ${PROJECT_NAME})