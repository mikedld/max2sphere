cmake_minimum_required(VERSION 3.14)

option(USE_VCPKG "Use vcpkg to resolve dependencies" ON)
if(USE_VCPKG)
    include(vcpkg.cmake)
endif()

project(max2sphere)

include(CheckLibraryExists)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_definitions(
        _CRT_SECURE_NO_WARNINGS
        _USE_MATH_DEFINES) # M_PI
    add_compile_options(/WX /W4 /wd4244 /wd4267 /wd4305)
else()
    add_compile_options(-Werror -Wall -Wextra)
endif()

add_executable(max2sphere
    bitmaplib.c
    bitmaplib.h
    max2sphere.c
    max2sphere.h
    $<$<BOOL:${MSVC}>:winthreads.c>
    $<$<BOOL:${MSVC}>:winthreads.h>
    $<$<BOOL:${MSVC}>:wintime.c>
    $<$<BOOL:${MSVC}>:wintime.h>)

check_library_exists(m sqrt "" HAVE_LIBM)
if(HAVE_LIBM)
    target_link_libraries(max2sphere PRIVATE m)
endif()

macro(m2s_image_format ID PKG TGT)
    set(ENABLE_${ID} AUTO CACHE STRING "Enable ${ID} support")
    set_property(CACHE ENABLE_${ID} PROPERTY STRINGS "ON;OFF;AUTO")
    if(ENABLE_${ID})
        set(REQ)
        if(NOT ENABLE_${ID} STREQUAL AUTO)
            set(REQ REQUIRED)
        endif()
        find_package(${PKG} ${REQ})
        if(${PKG}_FOUND)
            target_compile_definitions(max2sphere PRIVATE ADD${ID})
            target_link_libraries(max2sphere PRIVATE ${TGT})
        endif()
    endif()
endmacro()

m2s_image_format(JPEG JPEG JPEG::JPEG)
m2s_image_format(PNG PNG PNG::PNG)
m2s_image_format(TIFF TIFF TIFF::TIFF)
m2s_image_format(EXR OpenEXR OpenEXR::OpenEXR)
