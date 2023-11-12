# Run with cmake-js <https://github.com/cmake-js/cmake-js>

cmake_minimum_required (VERSION 3.15)
# Don't leave MSVC runtime library flags in the default
# CMAKE_<LANG>_FLAGS_<CONFIG> cache entries and ignore the
# CMAKE_MSVC_RUNTIME_LIBRARY abstraction
cmake_policy(SET CMP0091 NEW)
# Enable the MACOSX_RPATH property by default.
cmake_policy(SET CMP0042 NEW)

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

# define NAPI_VERSION.
# In the absence of a need for features available only in a specific Node-API
# version, version 3 is a good choice as it is the version of Node-API that was
# active when Node-API left experimental status.
target_compile_definitions (${PROJECT_NAME} PRIVATE NAPI_VERSION=3)


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
target_link_libraries (${PROJECT_NAME} PRIVATE pstore::pstore-core)

if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
  # Generate node.lib
  execute_process(COMMAND ${CMAKE_AR}
    /def:${CMAKE_JS_NODELIB_DEF}
    /out:${CMAKE_JS_NODELIB_TARGET}
    ${CMAKE_STATIC_LINKER_FLAGS}
  )
endif()
