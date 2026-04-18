
cmake_minimum_required(VERSION 3.30)
project(HillExplorer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(raylib)
# Create executable

file(GLOB SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB HEADERS CONFIGURE_DEPENDS "*.cpp")
add_executable(${PROJECT_NAME}
  ${SOURCES} 
  ${HEADERS}
)
target_link_libraries(${PROJECT_NAME} raylib)

# (Optional) For Linux: link additional libraries if needed
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} m pthread dl rt X11)
endif()
