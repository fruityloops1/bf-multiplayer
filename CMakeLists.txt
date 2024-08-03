cmake_minimum_required(VERSION 3.21)
project(subsdk)

## Error if not using switch toolchain file
if (NOT SWITCH)
  message(FATAL_ERROR "Not targeting switch, make sure to specify -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain.cmake")
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_language(ASM C CXX)

## Title Id
set(CONFIG_TITLE_ID 0x010028600ebda000)
add_compile_definitions(EXL_PROGRAM_ID=${CONFIG_TITLE_ID} EXL_LOAD_KIND=Module EXL_LOAD_KIND_ENUM=2 NNSDK IMGUI_USER_CONFIG="imgui_backend/nvn_imgui_config.h" private=public protected=public DEBUGMENU)

set(NN_SDK_MAJOR 4) # idk
set(NN_SDK_MINOR 0) # idk
set(NN_SDK_PATCH 0) # idk
set(NN_SDK_TYPE "Release")
set(NN_WARE_MAJOR 4) # idk
set(NN_WARE_MINOR 0) # idk
set(NN_WARE_PATCH 0) # idk

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/module/subsdk9.json.template ${CMAKE_CURRENT_BINARY_DIR}/subsdk9.json)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/penet)

## Project sources
set (INCLUDE_DIRS 
  ${PROJECT_SOURCE_DIR}/src/
  ${PROJECT_SOURCE_DIR}/src/lib 
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/lib/sead/include
  ${PROJECT_SOURCE_DIR}/lib/NintendoSDK/include
  ${PROJECT_SOURCE_DIR}/lib/NintendoSDK/include/nvn
  ${PROJECT_SOURCE_DIR}/lib/imgui
  ${PROJECT_SOURCE_DIR}/lib/enet-nx/include
  ${PROJECT_SOURCE_DIR}/lib/penet/Include
)
include_directories(${INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES_ASM ${PROJECT_SOURCE_DIR}/src/*.s)
file(GLOB_RECURSE SOURCES_C ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE SOURCES_CXX ${PROJECT_SOURCE_DIR}/src/*.cpp)

file(GLOB IMGUI_SOURCES ${PROJECT_SOURCE_DIR}/lib/imgui/*.cpp)
file(GLOB ENET_SOURCES ${PROJECT_SOURCE_DIR}/lib/enet-nx/source/*.c ${PROJECT_SOURCE_DIR}/lib/enet-nx/source/*.cpp)

## Include nx tools
include(${CMAKE_SOURCE_DIR}/cmake/SwitchTools.cmake)

## subsdk9
set(CMAKE_EXECUTABLE_SUFFIX ".nss")
add_executable(subsdk9 ${SOURCES_ASM} ${SOURCES_C} ${SOURCES_CXX} ${IMGUI_SOURCES} ${ENET_SOURCES})
target_link_libraries(subsdk9 PUBLIC penet)
target_include_directories(penet PUBLIC ${INCLUDE_DIRS})


## Prepare subsdk ld
set(SUBSDK_LD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/module/subsdk9.ld)
set(GAME_VERSION 110)
set(SYMS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/syms/${GAME_VERSION}")

execute_process(
    COMMAND bash -c "find ${SYMS_PATH} -type f -name '*.sym' -exec cat {} + > ${CMAKE_BINARY_DIR}/syms.ld"
)

execute_process(COMMAND uname OUTPUT_VARIABLE uname)
if (uname MATCHES "^MINGW")
  string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" SUBSDK_LD_PATH "${SUBSDK_LD_PATH}")
endif ()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/module/subsdk9.specs.template ${CMAKE_CURRENT_BINARY_DIR}/subsdk9.specs)

## Build nso & generate npdm
add_nso_target_subsdk(subsdk9)
set_target_properties(subsdk9 PROPERTIES CONFIG_JSON "${CMAKE_CURRENT_BINARY_DIR}/subsdk9.json")
__generate_npdm(subsdk9)
set_target_properties(subsdk9 PROPERTIES LINK_FLAGS "-specs ${CMAKE_BINARY_DIR}/subsdk9.specs -g ${ARCH} -nostartfiles -nostdlib")
