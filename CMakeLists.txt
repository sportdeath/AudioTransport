cmake_minimum_required (VERSION 3.4)
project (Vocoder)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no c++11 support.")
endif()

#Adding cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/modules/)

#######################
## External Libraries
#######################

# PortAudio
find_package(PORTAUDIO REQUIRED)
include_directories(${PORTAUDIO_INCLUDE_DIRS})
add_definitions(${PORTAUDIO_DEFINITIONS})
set(LIBS ${LIBS} ${PORTAUDIO_LIBRARIES})
if(NOT PORTAUDIO_FOUND)
  message(ERROR “ PORTAUDIO not found!”)
endif(NOT PORTAUDIO_FOUND)

# FFTW3
find_package(FFTW REQUIRED)
include_directories(${FFTW_INCLUDES})
set(LIBS ${LIBS} ${FFTW_LIBRARIES})
if(NOT FFTW_FOUND)
  message(ERROR " FFTW not found!")
endif(NOT FFTW_FOUND)

##################
## Testing Macro
##################

macro(MAKE_TEST test_name)
  add_executable(${test_name}
         test/${test_name}.cpp
         )
  target_link_libraries(${test_name} ${LIBS})
  add_test(${test_name} ${test_name})
endmacro()

################
## Sub-Projects
################
add_subdirectory(AudioUtilities)
include_directories("${PROJECT_SOURCE_DIR}/AudioUtilities/include")
set(LIBS ${LIBS} AudioUtilities)

#######################
## Internal Libraries
#######################

set(PROJECT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(VOCODER_SOURCES
  ${PROJECT_SOURCE_DIR}/PhaseVocoder.cpp
  ${PROJECT_SOURCE_DIR}/ClassicVocoder.cpp
  ${PROJECT_SOURCE_DIR}/BidirectionalVocoder.cpp
  ${PROJECT_SOURCE_DIR}/TransportVocoder.cpp
)

include_directories("${PROJECT_INCLUDE_DIR}")

####################
## Library Creation
####################

add_library(${PROJECT_NAME} SHARED ${VOCODER_SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
set(LIBS ${LIBS} ${PROJECT_NAME})

#####################
## Make Tests
#####################

MAKE_TEST(ClassicVocoderTest)
MAKE_TEST(BidirectionalVocoderTest)
MAKE_TEST(TransportVocoderTest)
