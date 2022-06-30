project(rendering)

find_package(OpenGL REQUIRED)

file(GLOB IMGUI_SRC ../../vendor/imgui/*.cpp)
file(GLOB IMGUI_INC ../../vendor/imgui/*.h)

configure_file(configuration/root_directory.h.in configuration/root_directory.h)

if(WIN32)
  set(LIBS ${OpenGL_LIBRARY})
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(UNIX AND NOT APPLE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  find_package(OpenGL REQUIRED)
  add_definitions(${OPENGL_DEFINITIONS})
  find_package(X11 REQUIRED)
  # note that the order is important for setting the libs
  # use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
  set(LIBS X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread)
  set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
elseif(APPLE)
  INCLUDE_DIRECTORIES(/System/Library/Frameworks)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
  FIND_LIBRARY(IOKit_LIBRARY IOKit)
  FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
  MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
  SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
  SET(APPLE_LIBS ${APPLE_LIBS})
  set(LIBS ${LIBS} ${APPLE_LIBS})
else()
  set(LIBS )
endif(WIN32)

add_library(STB_IMAGE "src/librendering/stb_image.cpp")
target_include_directories(STB_IMAGE
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_library(GLAD "src/glad/glad.c")
target_include_directories(GLAD
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${OPENGL_INCLUDE_DIRS}
)
target_link_libraries(GLAD OpenGL::GL)

set(LIBS ${LIBS} glfw STB_IMAGE GLAD assimp)

list(APPEND IMGUI_SRC "../../vendor/imgui/backends/imgui_impl_glfw.cpp" "../../vendor/imgui/backends/imgui_impl_opengl3.cpp")
list(APPEND IMGUI_INC "../../vendor/imgui/backends/imgui_impl_glfw.h" "../../vendor/imgui/backends/imgui_impl_opengl3.h")

add_library(${PROJECT_NAME}
	"src/librendering/rendering_manager.cpp"
	${IMGUI_SRC}
	${IMGUI_INC}
)
add_library(libs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} 
        -DSOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../../data/*.*"
        -DTARGET_PATH="${CMAKE_BINARY_DIR}/app/"
        -P ${CMAKE_SOURCE_DIR}/cmake/data_copy.cmake
    COMMENT "Running data copy script..."
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

message(STATUS "Using libraries: ${LIBS}")
target_link_libraries(${PROJECT_NAME}
	PUBLIC
		glm
	PRIVATE
		${OPENGL_LIBRARIES}
		${LIBS}
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/configuration>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		"../../vendor/imgui"
		"../../vendor/glfw/include"
		${OPENGL_INCLUDE_DIRS}
)

target_compile_features(${PROJECT_NAME}
	PRIVATE
		cxx_std_11
)
