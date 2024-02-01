cmake_minimum_required(VERSION 3.14)

option(PRODUCTION_BUILD "Make this a production build" OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else()
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

project(SFMLTemplate)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include external dependencies using FetchContent
include(FetchContent)

# Add SFML dependency
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG master
)
FetchContent_MakeAvailable(SFML)

# Add OpenAI
set(ALSOFT_DLOPEN  OFF CACHE INTERNAL "")
set(ALSOFT_UTILS OFF CACHE INTERNAL "")
set(ALSOFT_EXAMPLES OFF CACHE INTERNAL "")
set(ALSOFT_INSTALL OFF CACHE INTERNAL "")
set(ALSOFT_INSTALL_CONFIG OFF CACHE INTERNAL "")
set(ALSOFT_INSTALL_HRTF_DATA OFF CACHE INTERNAL "")
set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF CACHE INTERNAL "")
set(ALSOFT_INSTALL_EXAMPLES OFF CACHE INTERNAL "")
set(ALSOFT_INSTALL_UTILS OFF CACHE INTERNAL "")
set(ALSOFT_UPDATE_BUILD_VERSION OFF CACHE INTERNAL "")

FetchContent_Declare(
  openal
  GIT_REPOSITORY https://github.com/kcat/openal-soft.git
  GIT_TAG master
)
FetchContent_MakeAvailable(openal)

# Gather source files
file(GLOB_RECURSE Sources 
    "src/*.cpp"
)

# Gather header files
file(GLOB_RECURSE Headers 
    "src/*.h"
)

# Create static library
add_library(Library STATIC 
    ${Sources}
    ${Headers} # Helps with IDE navigation
)

add_dependencies(Library OpenAL)

FetchContent_GetProperties(openal)
if(openal_POPULATED)
    # Define the path to the OpenAL DLL
    set(OPENAL_DLL "${openal_BINARY_DIR}/OpenAL32.dll")

    # Copy the OpenAL DLL to the output directory after building the Library target
    add_custom_command(TARGET Library POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${OPENAL_DLL}
        $<TARGET_FILE_DIR:Library>
    )
endif()

if(PRODUCTION_BUILD)
    target_compile_definitions(Library PUBLIC RESOURCES_PATH="./resources/") 
    target_compile_definitions(Library PUBLIC PRODUCTION_BUILD=1)
else()
    target_compile_definitions(Library PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
    target_compile_definitions(Library PUBLIC PRODUCTION_BUILD=0)
endif()

target_link_libraries(Library PUBLIC 
    sfml-system
    sfml-window
    sfml-graphics
    sfml-audio
    sfml-network    
)

# Specify include directories
target_include_directories(Library PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SFML_SOURCE_DIR}/include
)

# Create the executable for the project
add_executable(${PROJECT_NAME} 
    src/Main.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    Library
)