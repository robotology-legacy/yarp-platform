
PROJECT(lmtools)

# Pick up any scripts - they are all in the config subdirectory
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/config)

# Add the path to header files
INCLUDE_DIRECTORIES(include)

# Use FFMPEG library
FIND_PACKAGE(FFMPEG REQUIRED)
INCLUDE_DIRECTORIES(${FFMPEG_INCLUDE_DIR})
LINK_DIRECTORIES(${FFMPEG_LINK_DIRECTORIES})
LINK_LIBRARIES(${FFMPEG_LIBRARIES})

# Create main executable
ADD_EXECUTABLE(mprocessdv src/mProcessDV/main.cpp)

# Use SDL library
FIND_PACKAGE(SDL REQUIRED)
INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR})
LINK_LIBRARIES(${SDL_LIBRARY})
FIND_PACKAGE(SDL_ttf REQUIRED)
INCLUDE_DIRECTORIES(${SDLTTF_INCLUDE_DIR})
LINK_LIBRARIES(${SDLTTF_LIBRARY})

# Create word presentation executable
ADD_EXECUTABLE(mwords src/mWords/main.cpp)
