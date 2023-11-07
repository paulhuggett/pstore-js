# Run with cmake-js

cmake_minimum_required (VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0042 NEW)
#set (CMAKE_CXX_STANDARD 17)

project (pstore-native)

add_library (${PROJECT_NAME}
  SHARED
  src/database.hpp
  src/database.cpp
  ${CMAKE_JS_SRC}
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
set_target_properties (${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_include_directories (${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})
target_link_libraries (${PROJECT_NAME} ${CMAKE_JS_LIB})

# define NAPI_VERSION
target_compile_definitions (${PROJECT_NAME} PRIVATE NAPI_VERSION=4)



# include N-API wrappers
execute_process (
  COMMAND node -p "require ('node-addon-api').include"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE NODE_ADDON_API_DIR
)
string (REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string (REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
message (STATUS "include dir=${NODE_ADDON_API_DIR}")
target_include_directories (${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})



include (${CMAKE_INSTALL_PREFIX}/lib/peejay/peejay.cmake)
include (${CMAKE_INSTALL_PREFIX}/lib/pstore/pstore.cmake)
target_link_libraries (${PROJECT_NAME} PRIVATE
    pstore::pstore-core
    pstore::pstore-brokerface
)

if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
  # Generate node.lib
  execute_process(COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF} /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
endif()
