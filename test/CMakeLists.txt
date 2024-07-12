cmake_minimum_required(VERSION 3.15...3.28)
include(cmake_helper/common/out_of_source.cmake)
include(cmake_helper/cross_compile/toolchian_file.cmake)
include(cmake_helper/common/custom_build_type.cmake)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE INTERNAL "Install path")

set(SDK_PATH ${CMAKE_SOURCE_DIR})

set(PROJECT_NAME "a" CACHE STRING "Project Name")

set(PROJECT_PATH "" CACHE PATH "Project Path") # since the type is PATH, set comman will convert it to absolute automatically.

set(EXTRA_SRCS "" CACHE FILEPATH "Extra Sources to join the compiling.")

set(EXTRA_INCS "" CACHE PATH "Extra Directories to include.")

set(EXTRA_LIBS "" CACHE FILEPATH "Extra Directories to include.")

project("${PROJECT_NAME}"
	VERSION 0.1.0
	DESCRIPTION "${PROJECT_NAME}"
	LANGUAGES C ASM
)
# for all targets

## common
set(compile_options_common
	-Wall -Werror -Wno-unused-variable -Wno-unused-function
	-ffunction-sections -fdata-sections
	-mstrict-align
)

set(link_options_common
	-mlittle-endian -march=rv32im -mabi=ilp32
	-static
	-nostartfiles
	-Wl,--fatal-warnings
	-Wl,--gc-sections
	-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/$CACHE{PROJECT_NAME}.map
	-Wl,--print-memory-usage
)

## if CMAKE_BUILD_TYPE is not Debug or RelWithDebInfo, strip all symbols
if(NOT $CACHE{CMAKE_BUILD_TYPE} STREQUAL "Debug" AND NOT $CACHE{CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	list(APPEND link_options_common -Wl,--strip-all)
endif()

## architecture specific
set(compile_options_arch -mlittle-endian -march=rv32im -mabi=ilp32)

## compiler specific

### set sysroot for clang
if(${IS_CLANG})
	message(FATAL_ERROR "clang is not supported!")
endif()

if(${IS_GCC})
	### With ld.bfd version 2.39 and newer new warnings are added. 
	### Skip those since we are not loaded by a elf loader.
	if(${CMAKE_C_COMPILER_VERSION} VERSION_GREATER_EQUAL 12.2.0)
		set(link_options_common ${link_options_common} -Wl,--no-warn-rwx-segments)
	endif()
	# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105523
	set(compile_options_common --param=min-pagesize=0 ${compile_options_common})
endif()

## this apply to all targets
add_compile_options(-DCV32E40P ${compile_options_arch} ${compile_options_common})

# targets
add_subdirectory(hw/cpu/cv32e40p EXCLUDE_FROM_ALL)

# app targets
if(NOT $CACHE{PROJECT_PATH} STREQUAL "")
	include($CACHE{PROJECT_PATH}/app.cmake)
endif()
add_executable($CACHE{PROJECT_NAME}.elf)

target_sources($CACHE{PROJECT_NAME}.elf
	PRIVATE
		${app_src}
		$CACHE{EXTRA_SRCS}
)

target_include_directories($CACHE{PROJECT_NAME}.elf
    PRIVATE
        ${app_inc}
		$CACHE{EXTRA_INCS}
)

target_link_options($CACHE{PROJECT_NAME}.elf
	PRIVATE
		${link_options_common}
)

target_link_libraries($CACHE{PROJECT_NAME}.elf
	PRIVATE
		cv32e40p
		${app_dep}
		$CACHE{EXTRA_LIBS}
)

# Cumtom targets

## dump
add_custom_command(
	TARGET $CACHE{PROJECT_NAME}.elf POST_BUILD
	COMMAND ${CMAKE_OBJDUMP} -dx $CACHE{PROJECT_NAME}.elf > $CACHE{PROJECT_NAME}.dump
	WORKING_DIRECTORY   "${CMAKE_BINARY_DIR}"
)

## bin
add_custom_command(
	TARGET $CACHE{PROJECT_NAME}.elf POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O binary $CACHE{PROJECT_NAME}.elf $CACHE{PROJECT_NAME}.bin
	WORKING_DIRECTORY   "${CMAKE_BINARY_DIR}"
)

## hex
add_custom_command(
	OUTPUT 
		$CACHE{PROJECT_NAME}.hex
	COMMAND hexdump -e '"%08x\\n"' -v $CACHE{PROJECT_NAME}.bin > $CACHE{PROJECT_NAME}.hex
	DEPENDS
		$CACHE{PROJECT_NAME}.elf
	WORKING_DIRECTORY   "${CMAKE_BINARY_DIR}"
)
add_custom_target(hex
	DEPENDS 
		$CACHE{PROJECT_NAME}.hex
)

# additional clean files
set_property(TARGET $CACHE{PROJECT_NAME}.elf 
	APPEND PROPERTY 
	ADDITIONAL_CLEAN_FILES 
		$CACHE{PROJECT_NAME}.map
		$CACHE{PROJECT_NAME}.bin
		$CACHE{PROJECT_NAME}.dump
)

# compile database
include(cmake_helper/common/compile_database.cmake)
