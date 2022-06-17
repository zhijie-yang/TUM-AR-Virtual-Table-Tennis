project(app)

set(EXEC_NAME virtualtennis)

add_executable(${PROJECT_NAME}
	main.cpp
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		libs::tennis
		libs::rendering
		libs::vision
)
