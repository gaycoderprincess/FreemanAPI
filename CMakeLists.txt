cmake_minimum_required(VERSION 3.27)
project(FreemanAPI)

SET(CMAKE_SYSTEM_NAME Windows)

SET(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
SET(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
SET(CMAKE_RC_COMPILER i686-w64-mingw32-windres)
SET(CMAKE_RANLIB i686-w64-mingw32-ranlib)

SET(CMAKE_CXX_STANDARD 20)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -masm=intel -s -static")

add_compile_definitions(NYA_COMMON_NO_D3D NYA_HL_COL_FALLBACK)
add_library(FreemanAPI SHARED
        ../nya-common/nya_commontimer.cpp
        main.cpp)
target_include_directories(FreemanAPI PUBLIC ../nya-common)
target_include_directories(FreemanAPI PUBLIC ../FlatOutUCMenuLib/include)
target_link_options(FreemanAPI PRIVATE "-Wl,--exclude-all-symbols")
set_target_properties(FreemanAPI PROPERTIES PREFIX "")
set_target_properties(FreemanAPI PROPERTIES SUFFIX "_gcp.dll")