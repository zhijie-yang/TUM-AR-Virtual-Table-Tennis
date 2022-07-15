# Install script for directory: /Users/liyif/Desktop/virtualtennis/vendor/assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Library/Developer/CommandLineTools/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibassimp5.2.0-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/liyif/Desktop/virtualtennis/cmake-build-debug/vendor/assimp/lib/libassimpd.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimpd.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimpd.a")
    execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimpd.a")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/anim.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/aabb.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/ai_assert.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/camera.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/color4.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/color4.inl"
    "/Users/liyif/Desktop/virtualtennis/cmake-build-debug/vendor/assimp/code/../include/assimp/config.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/ColladaMetaData.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/commonMetaData.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/defs.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/cfileio.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/light.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/material.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/material.inl"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/matrix3x3.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/matrix3x3.inl"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/matrix4x4.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/matrix4x4.inl"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/mesh.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/ObjMaterial.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/pbrmaterial.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/GltfMaterial.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/postprocess.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/quaternion.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/quaternion.inl"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/scene.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/metadata.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/texture.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/types.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/vector2.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/vector2.inl"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/vector3.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/vector3.inl"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/version.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/cimport.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/importerdesc.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/Importer.hpp"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/DefaultLogger.hpp"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/ProgressHandler.hpp"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/IOStream.hpp"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/IOSystem.hpp"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/Logger.hpp"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/LogStream.hpp"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/NullLogger.hpp"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/cexport.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/Exporter.hpp"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/DefaultIOStream.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/DefaultIOSystem.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/ZipArchiveIOSystem.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/SceneCombiner.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/fast_atof.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/qnan.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/BaseImporter.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/Hash.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/MemoryIOWrapper.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/ParsingUtils.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/StreamReader.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/StreamWriter.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/StringComparison.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/StringUtils.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/SGSpatialSort.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/GenericProperty.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/SpatialSort.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/SkeletonMeshBuilder.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/SmallVector.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/SmoothingGroups.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/SmoothingGroups.inl"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/StandardShapes.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/RemoveComments.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/Subdivision.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/Vertex.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/LineSplitter.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/TinyFormatter.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/Profiler.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/LogAux.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/Bitmap.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/XMLTools.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/IOStreamBuffer.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/CreateAnimMesh.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/XmlParser.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/BlobIOSystem.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/MathFunctions.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/Exceptional.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/ByteSwapper.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/Base64.hpp"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/Compiler/poppack1.h"
    "/Users/liyif/Desktop/virtualtennis/vendor/assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

