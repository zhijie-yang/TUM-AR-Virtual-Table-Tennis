project(vision)
set(CMAKE_CXX_STANDARD 14)

find_package( OpenCV REQUIRED )

add_library(${PROJECT_NAME}
	"src/libvision/vision_manager.cpp"
)

add_library(libs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		glm
	PRIVATE
		${OpenCV_LIBS}
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		"${OpenCV_INCLUDE_DIRS}"
)

target_compile_features(${PROJECT_NAME}
	PRIVATE
		cxx_std_11
)
