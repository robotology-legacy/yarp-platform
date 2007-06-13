SET(YARP_DIR "$ENV{YARP_BUILD}" CACHE LOCATION "where is yarp?")
FIND_PACKAGE(YARP)

PROJECT(track_tongue)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

ADD_LIBRARY(tracker_tools GlutOrientation.cpp IntegralImage.cpp Finder.cpp)

LINK_LIBRARIES(tracker_tools)

ADD_EXECUTABLE(track_tongue main_tongue.cpp TongueFinder.cpp)

ADD_EXECUTABLE(track_mouth main_mouth.cpp MouthFinder.cpp)
