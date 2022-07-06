project(framework)
add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(
    framework
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)