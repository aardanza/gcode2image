# TOP Level Configuration
# ------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.6.3)

# Project name & Configuration
# ------------------------------------------------------------------------------------------------------
SET(PROJECT_MAIN_NAME "gcode2image")
PROJECT(${PROJECT_MAIN_NAME})

# The version number.
set (GCODE2IMAGE_VERSION_MAJOR 1)
set (GCODE2IMAGE_VERSION_MINOR 0)

# Configuration types
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
IF(DEFINED CMAKE_BUILD_TYPE AND CMAKE_VERSION VERSION_GREATER "2.8")
  SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})
ENDIF()

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

#OpenCV
SET(OpenCV_STATIC OFF)			# force opencv to link dynamic
IF(NOT WIN32)
    LINK_DIRECTORIES(${OpenCV_DIR}/lib)
    SET(OpenCV_LIBS opencv_core opencv_videoio opencv_imgproc opencv_imgcodecs opencv_videostab opencv_flann opencv_ml opencv_photo opencv_video opencv_features2d opencv_calib3d)
ELSE()
    FIND_PACKAGE(OpenCV REQUIRED)
    IF(OpenCV_DIR STREQUAL NOT_FOUND)
      MESSAGE(FATAL_ERROR "Install OpenCV")
    ENDIF()
ENDIF()

#gpr G-code parser
ADD_SUBDIRECTORY( 3rdparty/gpr )
SET_TARGET_PROPERTIES(gpr PROPERTIES FOLDER "3rdparty")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/src/version.h.in"
  "${PROJECT_SOURCE_DIR}/src/version.h"
  )
  
INCLUDE_DIRECTORIES("3rdparty/gpr/src"
	${OPENCV_INCLUDE_DIR}
	)

# Source
#---------------------------------------------------#
FILE(GLOB ${PROJECT_MAIN_NAME}_SRC
    "src/*.cpp"
)
FILE(GLOB ${PROJECT_MAIN_NAME}_HEADERS
    "src/*.h"
)

ADD_EXECUTABLE(${PROJECT_MAIN_NAME} 
	${${PROJECT_MAIN_NAME}_SRC} 
	${${PROJECT_MAIN_NAME}_HEADERS} )
TARGET_LINK_LIBRARIES(${PROJECT_MAIN_NAME}  gpr  ${OpenCV_LIBS})
