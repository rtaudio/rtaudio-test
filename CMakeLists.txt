cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 14)

project (RTaudioTest)

set (RTaudioTest_VERSION_MAJOR 1)
set (RTaudioTest_VERSION_MINOR 0)

add_subdirectory (rtt)
add_subdirectory (autil)

# list(APPEND FFTW_LIBRARIES "${FFTW_libm_LIBRARY}")
# include_directories ("${PROJECT_SOURCE_DIR}/MathFunctions")

#find_library(ITAFFT_LIB ITAFFT ITAFFT HINT ~/arm-dev/libs/ita/ITAFFT/Lib/arm )
#find_path (ITAFFT_INC ITAFFT.h HINT ~/arm-dev/libs/ita/ITAFFT/include)

#find_library(ITATOOLKIT_LIB ITAToolkit HINT ~/arm-dev/libs/ita/ITAToolkit/Lib/arm )
#find_path (ITATOOLKIT_INC ITAAudiofileReader.h HINT ~/arm-dev/libs/ita/ITAToolkit/Include)

#include_directories(./ "${PROJECT_BINARY_DIR}" "${ITATOOLKIT_INC}" "${ITAFFT_INC}")

add_executable(rtaudio-test rtaudio-test.cpp)

target_include_directories (rtaudio-test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries (rtaudio-test LINK_PUBLIC rtt autil)



# enable cxx11
target_compile_features(rtaudio-test PRIVATE cxx_range_for)


#find_library(PTHREAD_LIB NAMES pthread )


#execute_process(COMMAND ./gcc-tune-flags
#    OUTPUT_VARIABLE GCC_TUNE_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
#add_definitions(${GCC_TUNE_FLAGS})

#debug
add_definitions("-g -ggdb")
