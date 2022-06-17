project(rendering)

file(GLOB IMGUI_SRC ../../vendor/imgui/*.cpp)
file(GLOB IMGUI_INC ../../vendor/imgui/*.h)

list(APPEND IMGUI_SRC "../../vendor/imgui/backends/imgui_impl_sdl.cpp" "../../vendor/imgui/backends/imgui_impl_sdlrenderer.cpp")
list(APPEND IMGUI_INC "../../vendor/imgui/backends/imgui_impl_sdl.h" "../../vendor/imgui/backends/imgui_impl_sdlrenderer.h")

add_library(${PROJECT_NAME}
	"src/librendering/rendering_manager.cpp"
	${IMGUI_SRC}
	${IMGUI_INC}
)

add_library(libs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		glm
	PRIVATE
		SDL2
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		"../../vendor/imgui"
)

target_compile_features(${PROJECT_NAME}
	PRIVATE
		cxx_std_11
)
