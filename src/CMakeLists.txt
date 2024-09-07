project(imagine VERSION 0.0.1 LANGUAGES CXX)

# this line with the files *.cmake in cmake/ dir are necessary to link GLFW library 
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

find_package(GLFW3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(nlohmann_json REQUIRED)

set(SOURCE_EXE
    main.cpp
    Model.h
    Model.cpp
    MainView.h
    MainView.cpp
    Browser.h
    DummyBrowser.h
    DummyBrowser.cpp
    FileBrowser.h
    FileBrowser.cpp
    device.h
)

add_executable(${PROJECT_NAME} ${SOURCE_EXE})

#target_include_directories(${PROJECT_NAME})# PRIVATE ${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW3_LIBRARY} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${nlohmann_json_LIBRARY})

#target_include_directories(${PROJECT_NAME}
#    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
#)

