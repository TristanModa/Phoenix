add_executable(${PROJECT_NAME}
        main.c
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/phoenix/)
target_link_libraries(${PROJECT_NAME} PRIVATE
        VulfEngine
)