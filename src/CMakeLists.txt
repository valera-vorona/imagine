project(imagine VERSION 0.0.1 LANGUAGES CXX)

# this line with the files *.cmake in cmake/ dir are necessary to link GLFW library 
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

find_package(GLFW3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(nlohmann_json REQUIRED)

set(SOURCE_EXE
    main.cpp
    misc.h
    misc.cpp
    Model.h
    Model.cpp
    Loader.h
    Loader.cpp
    Cache.h
    Cache.cpp
    View.h
    View.cpp
    NormalView.h
    NormalView.cpp
    FullScreenView.h
    FullScreenView.cpp
    Browser.h
    DummyBrowser.h
    DummyBrowser.cpp
    FileBrowser.h
    FileBrowser.cpp
    device.h
)

add_executable(${PROJECT_NAME} ${SOURCE_EXE})
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")

target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${OpenCV_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW3_LIBRARY} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${OpenCV_LIBRARIES} ${nlohmann_json_LIBRARY})

