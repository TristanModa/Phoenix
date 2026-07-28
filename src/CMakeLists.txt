# Create the executable
add_executable(${PROJECT_NAME}
        main.c
        application.h
        application.c
)

# Configure the executable
target_compile_definitions(${PROJECT_NAME} PRIVATE
        VERSION_MAJOR="${PROJECT_VERSION_MAJOR}"
        VERSION_MINOR="${PROJECT_VERSION_MINOR}"
        VERSION_PATCH="${PROJECT_VERSION_PATCH}"
)