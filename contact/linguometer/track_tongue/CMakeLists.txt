SET(YARP_DIR "$ENV{YARP_BUILD}" CACHE LOCATION "where is yarp?")
FIND_PACKAGE(YARP)

PROJECT(track_tongue)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

ADD_EXECUTABLE(track_tongue main.cpp GlutOrientation.cpp IntegralImage.cpp TongueFinder.cpp)
