project(tennis)
set(CMAKE_CXX_STANDARD 14)
# Set LIBS and use it to source libframework headers
set(LIBS ${CMAKE_SOURCE_DIR}/libs)
# Add libframework headers to include path
include_directories(${LIBS})
find_package(Protobuf REQUIRED)

add_library(${PROJECT_NAME}
	"src/libtennis/priv-impl.cpp"
	"include/libtennis/collisionDetect.h"
	"src/libtennis/tennis_manager.cpp"
	"include/libtennis/tennis_manager.h")

add_library(libs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		glm
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		${PROTOBUF_INCLUDE_DIRS}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(${PROJECT_NAME}
	PRIVATE
		cxx_std_11
)

if(BUILD_TESTING)
	add_subdirectory(tests)
endif()