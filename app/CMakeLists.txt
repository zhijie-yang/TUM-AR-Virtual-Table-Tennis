project(app)
set(CMAKE_CXX_STANDARD 14)
set(EXEC_NAME virtualtennis)

# Set LIBS and use it to source libframework headers
set(LIBS
    ${CMAKE_SOURCE_DIR}/libs
)
# Add libframework headers to include path
include_directories(${LIBS})

add_executable(${PROJECT_NAME}
	main.cpp
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		libs::tennis
		libs::rendering
		libs::vision
		libs::network_client
)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data)
file(COPY ${CMAKE_SOURCE_DIR}/data/marker.png DESTINATION ${CMAKE_BINARY_DIR}/data/)
