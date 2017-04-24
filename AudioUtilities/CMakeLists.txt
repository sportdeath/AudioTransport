cmake_minimum_required (VERSION 3.4)
project (AudioUtilities)

#######################
## Internal Libraries
#######################

set(PROJECT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(AUDIO_UTILITIES_SOURCES
  ${PROJECT_SOURCE_DIR}/InputStream.cpp
  ${PROJECT_SOURCE_DIR}/OutputStream.cpp
  ${PROJECT_SOURCE_DIR}/ChooseDevices.cpp
  ${PROJECT_SOURCE_DIR}/ChooseMIDIDevices.cpp
  ${PROJECT_SOURCE_DIR}/MIDIStream.cpp
  ${PROJECT_SOURCE_DIR}/STFT.cpp
  ${PROJECT_SOURCE_DIR}/ISTFT.cpp
  ${PROJECT_SOURCE_DIR}/Window.cpp
  ${PROJECT_SOURCE_DIR}/CoordinateTransforms.cpp
  ${PROJECT_SOURCE_DIR}/Plotting.cpp
)

include_directories("${PROJECT_INCLUDE_DIR}")

####################
## Library Creation
####################

add_library(${PROJECT_NAME} SHARED ${AUDIO_UTILITIES_SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
set(LIBS ${LIBS} ${PROJECT_NAME})

############
## Testing
############

MAKE_TEST(InputStreamTest)
MAKE_TEST(OutputStreamTest)
MAKE_TEST(InputOutputStreamTest)
MAKE_TEST(ChooseDevicesTest)
MAKE_TEST(ChooseMIDIDevicesTest)
MAKE_TEST(MIDIStreamTest)
MAKE_TEST(STFTTest)
MAKE_TEST(CoordinateTransformsTest)
