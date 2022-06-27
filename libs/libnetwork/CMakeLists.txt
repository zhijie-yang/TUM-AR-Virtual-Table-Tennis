project(network)

cmake_minimum_required(VERSION 3.13)

find_package(Protobuf REQUIRED)
# find_package(gRPC_GRPC++ REQUIRED)
# add_library()
# link_directories(/usr/lib)
find_library(GRPC++ grpc++)
find_library(GRPC++_REFLECTION grpc++_reflection)
find_program(CRPC_CPP_PLUGIN grpc_cpp_plugin)

set(PROTOS
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/network.proto
)

message(${CMAKE_CURRENT_BINARY_DIR})
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

include_directories(${PROTO_SRC_DIR})

add_library(${PROJECT_NAME}
	"src/lib${PROJECT_NAME}/server.cpp"
    ${PROTO_SRC_DIR}/network.grpc.pb.h
    ${PROTO_SRC_DIR}/network.grpc.pb.cc
    ${PROTO_SRC_DIR}/network.pb.h
    ${PROTO_SRC_DIR}/network.pb.cc
)

add_library(libs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
	PUBLIC
    Protobuf
	GRPC++
    GRPC++_REFLECTION
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
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