cmake_minimum_required(VERSION 3.16)
project(LFunctionServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Fetch Crow (Updated to v1.2.0)
include(FetchContent)
FetchContent_Declare(
    Crow
    URL https://github.com/CrowCpp/Crow/archive/refs/tags/v1.2.0.tar.gz
)
# This downloads Crow and defines the variable 'crow_SOURCE_DIR'
FetchContent_MakeAvailable(Crow)

# 2. Find Math Libraries
find_library(FLINT_LIB flint REQUIRED)
find_library(GMP_LIB gmp REQUIRED)
find_library(MPFR_LIB mpfr REQUIRED)
find_package(OpenMP REQUIRED)

# 3. Add Executable
add_executable(LFunctionServer src/main.cpp)

# 4. Link Libraries
target_link_libraries(LFunctionServer PRIVATE 
    ${FLINT_LIB} 
    ${MPFR_LIB} 
    ${GMP_LIB}
    OpenMP::OpenMP_CXX  
)

# 5. Include Directories (THE FIX)
# We explicitly add the 'include' folder from the downloaded Crow source.
target_include_directories(LFunctionServer PRIVATE 
    ${crow_SOURCE_DIR}/include
    /usr/include/flint
)

# 6. Copy HTML to 'templates' folder (Required by Crow)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/templates)
configure_file(src/index.html ${CMAKE_BINARY_DIR}/templates/index.html COPYONLY)