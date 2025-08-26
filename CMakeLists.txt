cmake_minimum_required (VERSION 3.30)

project ("CellSpace")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "-g")
set(CMAKE_CXX_FLAGS "-g")
set(VCPKG_ROOT $ENV{VCPKG_ROOT})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(APP_ROOT ${CMAKE_CURRENT_LIST_DIR})

#set(IMNODEFLOW_DIR ${CMAKE_CURRENT_LIST_DIR}/lib/ImNodeFlow)
#set(IMGUIFILE_DIALOG_DIR ${CMAKE_CURRENT_LIST_DIR}/lib/ImGuiFileDialog)
#include(FetchContent)

set(cpps
	srcs/main.cpp

  srcs/CellSpace.cpp
  srcs/CellGrid.cpp
  srcs/RuleSet.cpp
  srcs/CAParser.cpp
  #srcs/Cell.cpp

  srcs/HephGuiSetup.cpp
  srcs/HephGui.cpp
)

set(hpps
  srcs/CellSpace.hpp
  srcs/CellGrid.hpp
  srcs/Cell.hpp
  srcs/RuleSet.hpp
  srcs/CAParser.hpp

  srcs/guiInfo.hpp
  srcs/HephGui.hpp
)

add_executable(${PROJECT_NAME} ${cpps} ${hpps})


target_link_libraries(${PROJECT_NAME} PRIVATE hephaestus)

include(FindVulkan)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

find_package(Boost REQUIRED COMPONENTS json)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::json)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

target_include_directories(${PROJECT_NAME}
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_BINARY_DIR}
	srcs
)
target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_DEFINE_MATH_OPERATORS)
