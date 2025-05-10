cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/sys/cmake/toolchain.cmake")
project(BowsersFuryMultiplayer VERSION 1.0 LANGUAGES ASM C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(config/config.cmake)

file(GLOB_RECURSE SOURCES_ASM ${PROJECT_SOURCE_DIR}/src/*.S)
file(GLOB_RECURSE SOURCES_C ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE SOURCES_CXX ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB IMGUI_SOURCES ${PROJECT_SOURCE_DIR}/lib/imgui/*.cpp)
file(GLOB ENET_SOURCES ${PROJECT_SOURCE_DIR}/lib/enet-nx/source/*.c ${PROJECT_SOURCE_DIR}/lib/enet-nx/source/*.cpp)
set(SOURCES ${SOURCES_ASM} ${SOURCES_C} ${SOURCES_CXX} ${IMGUI_SOURCES} ${ENET_SOURCES})

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/penet)

add_executable(${PROJECT_NAME} ${SOURCES})

set(INCLUDE_DIRS
  ${PROJECT_SOURCE_DIR}/src/
  ${PROJECT_SOURCE_DIR}/src/lib 
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/lib/sead/include
  ${PROJECT_SOURCE_DIR}/lib/NintendoSDK/include
  ${PROJECT_SOURCE_DIR}/lib/NintendoSDK/include/nvn
  ${PROJECT_SOURCE_DIR}/lib/imgui
  ${PROJECT_SOURCE_DIR}/lib/enet-nx/include
  ${PROJECT_SOURCE_DIR}/lib/penet/Include
  ${PROJECT_SOURCE_DIR}/sys/hakkun/include
)

include_directories(
  ${INCLUDE_DIRS}
)

target_include_directories(penet PUBLIC ${INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC penet)

include(sys/cmake/module.cmake)
include(sys/cmake/deploy.cmake)
include(sys/cmake/sail.cmake)
usesail(${PROJECT_NAME})
