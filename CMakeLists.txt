cmake_minimum_required (VERSION 3.9)
cmake_policy (SET CMP0042 NEW)
set (CMAKE_CXX_STANDARD 14)

project (pstore-native)

add_library (${PROJECT_NAME}
  SHARED
  src/database.hpp
  src/database.cpp
  ${CMAKE_JS_SRC}
)
set_target_properties (${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_include_directories (${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})
target_link_libraries (${PROJECT_NAME} ${CMAKE_JS_LIB})
target_compile_definitions (${PROJECT_NAME} PRIVATE NAPI_VERSION=3)



# include N-API wrappers
execute_process (
  COMMAND node -p "require ('node-addon-api').include"
  WORKING_DIRECTORI ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE NODE_ADDON_API_DIR
)
string (REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string (REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories (${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})



# FIXME: use find!
target_include_directories (${PROJECT_NAME} PRIVATE /usr/local/include)
target_link_libraries (${PROJECT_NAME} PRIVATE
  /usr/local/lib/pstore/libpstore-core.a
  /usr/local/lib/pstore/libpstore-os.a
  /usr/local/lib/pstore/libpstore-support.a
  /usr/local/lib/pstore/libpstore-broker-intf.a
)
message (STATUS "adding /usr/local/include")

