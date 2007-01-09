SET(YARP_DIR "$ENV{YARP_BUILD}" CACHE LOCATION "where is yarp?")
FIND_PACKAGE(YARP)

PROJECT(click_segment)

ADD_EXECUTABLE(click_segment main.cpp)
