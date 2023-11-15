cmake_minimum_required(VERSION 3.26)
project(fridge_injector)

set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/**)

find_package(glm CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)

set(GLFW_DIR ${CMAKE_SOURCE_DIR}/libs/glfw)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/libs/imgui)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)

add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)
include_directories(${GLFW_DIR}/include)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)
include_directories(${GLFW_DIR}/deps)

add_executable(fridge_injector
        ${SOURCES}
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_sources(fridge_injector PRIVATE assets/resource.rc)

target_link_libraries(fridge_injector PRIVATE
        freetype
        glfw
        glm::glm
        opengl32
        psapi
)