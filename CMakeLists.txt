cmake_minimum_required(VERSION 2.8)
project( ComputerVision )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( Stereo 01Stereo.cpp )
target_link_libraries( Stereo ${OpenCV_LIBS} )

add_executable( Solarization 02Solarization.cpp )
target_link_libraries( Solarization ${OpenCV_LIBS} )

add_executable( OrientationHistograms 03OrientationHistograms.cpp )
target_link_libraries( OrientationHistograms ${OpenCV_LIBS} )

# file(GLOB FILES "*.cpp")
# 
# foreach(SOURCE_FILE ${FILES})
#     string(REGEX REPLACE ".cpp$" "" EXE_NAME "${SOURCE_FILE}")
# 
#     add_executable(${EXE_NAME} ${SOURCE_FILE})
#     target_link_libraries(${EXE_NAME} ${OpenCV_LIBS})
# endforeach(SOURCE_FILE)
