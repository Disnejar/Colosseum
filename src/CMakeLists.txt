# Example usage:
#  mkdir build
#  cd build
#  cmake -g "Visual Studio 14 2015" ..

cmake_minimum_required(VERSION 2.8)
project(Colosseum C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

# GLFW
set(GLFW_DIR ../glfw) # Set this to point to an up-to-date GLFW repo
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)
include_directories(${GLFW_DIR}/include)

# Dear ImGui
set(IMGUI_DIR ../imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)
link_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)
set(IMGUI_SRC ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp)

# Libraries
find_package(Vulkan REQUIRED)
#find_library(VULKAN_LIBRARY
  #NAMES vulkan vulkan-1)
#set(LIBRARIES "glfw;${VULKAN_LIBRARY}")
set(LIBRARIES "glfw;Vulkan::Vulkan")

# Use vulkan headers from glfw:
include_directories(${GLFW_DIR}/deps)

set(STB_DIR ../stb)
include_directories(${STB_DIR})
link_directories(${STB_DIR})

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS *.cpp)

add_executable(Colloseum ${SRC} ${IMGUI_SRC})
target_link_libraries(Colloseum ${LIBRARIES})
target_compile_definitions(Colloseum PUBLIC -DImTextureID=ImU64)
target_compile_definitions(Colloseum
    PRIVATE
        # If the debug configuration pass the DEBUG define to the compiler 
        $<$<CONFIG:Debug>:COLOSSEUM_DEBUG_MODE>
)
