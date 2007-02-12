# SET(YARP_DIR "${CMAKE_SOURCE_DIR}" CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)

PROJECT(mirrorCollector)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

ADD_EXECUTABLE(mirrorCollector main.cpp CollectorCommands.h fakes.h)
