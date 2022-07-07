project(network)

cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 14)

find_package(Protobuf REQUIRED)
# find_package(gRPC_GRPC++ REQUIRED)
# add_library()
find_library(PROTOBUF protobuf REQUIRED)
find_library(GRPC grpc REQUIRED)
find_library(GRPC++ grpc++ REQUIRED)
find_library(GRPC++_REFLECTION grpc++_reflection REQUIRED)
message(${PROTOBUF})
message(${GRPC})
message(${GRPC++})
message(${GRPC++_REFLECTION})
find_program(CRPC_CPP_PLUGIN grpc_cpp_plugin)

set(PROTOS
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/network.proto
)

set(PROTO_FILE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
get_filename_component(PROTO_FILE ${PROTO_FILE_DIR}/network.proto ABSOLUTE)
set(PROTO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto_src)
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})
set(GRPC_COMP protoc)

add_custom_command(
    OUTPUT ${PROTO_SRC_DIR}/network.grpc.pb.h ${PROTO_SRC_DIR}/network.grpc.pb.cc
    COMMAND ${GRPC_COMP}
    ARGS -I ${PROTO_FILE_DIR}
      --grpc_out ${PROTO_SRC_DIR}
      --plugin=protoc-gen-grpc=${CRPC_CPP_PLUGIN}
      ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
)

add_custom_command(
    OUTPUT ${PROTO_SRC_DIR}/network.pb.h ${PROTO_SRC_DIR}/network.pb.cc
    COMMAND ${GRPC_COMP}
    ARGS --cpp_out ${PROTO_SRC_DIR}
      -I ${PROTO_FILE_DIR}
      ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
)

# add_library(framework INTERFACE)

# target_include_directories(
#     FRAMEWORK
#     INTERFACE $<BUILD_INTERFACE:${FRAMEWORK_INCLUDE_DIR}>
#               $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
# )

target_include_directories(
    framework
    INTERFACE ${FRAMEWORK_INCLUDE_DIR}
)



include_directories(${PROTO_SRC_DIR})
include_directories(${PROTOBUF_INCLUDE_DIRS})

add_library(${PROJECT_NAME}_grpc_handler
    ${PROTO_SRC_DIR}/network.grpc.pb.h
    ${PROTO_SRC_DIR}/network.grpc.pb.cc
)

add_library(${PROJECT_NAME}_protobuf
    ${PROTO_SRC_DIR}/network.pb.h
    ${PROTO_SRC_DIR}/network.pb.cc
)

# Set LIBS and use it to source libframework headers
set(LIBS
    ${CMAKE_SOURCE_DIR}/libs
)
# Add libframework headers to include path
include_directories(${LIBS})

# add_library(libs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# target_link_libraries(${PROJECT_NAME}
# 	PUBLIC
#     glm
#     GRPC
# 	GRPC++
#     GRPC++_REFLECTION
#     framework
#     ${PROJECT_NAME}_grpc_handler
#     ${PROJECT_NAME}_protobuf
# )

# target_include_directories(${PROJECT_NAME}
# 	PUBLIC
# 		$<INSTALL_INTERFACE:include>
# 		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
# 	PRIVATE
# 		${CMAKE_CURRENT_SOURCE_DIR}/src
# )

# target_compile_features(${PROJECT_NAME}
# 	PRIVATE
# 		cxx_std_11
# )


add_executable(${PROJECT_NAME}_client
    src/lib${PROJECT_NAME}/client.cpp
)

target_link_libraries(${PROJECT_NAME}_client
    PUBLIC
    glm
    ${PROTOBUF}
    ${GRPC}
    ${GRPC++}
    ${GRPC++_REFLECTION}
    ${PROJECT_NAME}_grpc_handler
    ${PROJECT_NAME}_protobuf
)

add_executable(${PROJECT_NAME}_server
    src/lib${PROJECT_NAME}/server.cpp
)

target_link_libraries(${PROJECT_NAME}_server
    PUBLIC
    glm
    ${PROTOBUF}
    ${GRPC}
    ${GRPC++}
    ${GRPC++_REFLECTION}
    ${PROJECT_NAME}_grpc_handler
    ${PROJECT_NAME}_protobuf
)

# if(BUILD_TESTING)
# 	add_subdirectory(tests)
# endif()