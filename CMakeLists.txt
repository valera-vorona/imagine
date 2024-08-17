cmake_minimum_required(VERSION 3.22)

project(imagine VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_C_STANDARD 89)

add_subdirectory(src)

# install
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES .imagine DESTINATION $ENV{HOME}/.${PROJECT_NAME})

