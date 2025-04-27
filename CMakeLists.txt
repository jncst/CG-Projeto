cmake_minimum_required(VERSION 3.11)

# Project Name
PROJECT(engine)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)

set(TOOLKITS_FOLDER "C:/Users/2004j/Documents/Ano3/Sem2/CG/toolkits" CACHE PATH "Path to Toolkits folder")

add_executable(generator src/generator.cpp)
add_executable(${PROJECT_NAME} 
    src/engine.cpp 
    src/parserXML.cpp
)

if(WIN32)
    target_sources(${PROJECT_NAME} PRIVATE ${TOOLKITS_FOLDER}/tinyxml2/tinyxml2.cpp)
    target_include_directories(${PROJECT_NAME} PRIVATE ${TOOLKITS_FOLDER}/tinyxml2)
endif()


find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif()

if(WIN32)

    message(STATUS "Toolkits_DIR set to: ${TOOLKITS_FOLDER}")

    if (NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
        message(ERROR ": GLUT not found")
    endif()

    if (NOT EXISTS "${TOOLKITS_FOLDER}/glew/GL/glew.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glew/glew32.lib")
        message(ERROR ": GLEW not found")
    endif()

    include_directories(${TOOLKITS_FOLDER}/glut ${TOOLKITS_FOLDER}/glew ${TOOLKITS_FOLDER}/tinyxml2)

    target_link_libraries(${PROJECT_NAME} 
        ${OPENGL_LIBRARIES} 
        ${TOOLKITS_FOLDER}/glut/glut32.lib 
        ${TOOLKITS_FOLDER}/glew/glew32.lib
    )

    if (EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll")
        file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR})
    endif()

    if (EXISTS "${TOOLKITS_FOLDER}/glew/glew32.dll")
        file(COPY ${TOOLKITS_FOLDER}/glew/glew32.dll DESTINATION ${CMAKE_BINARY_DIR})
    endif()

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

else() # Linux and Mac

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")

    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIR})
    link_directories(${GLUT_LIBRARY_DIRS})
    add_definitions(${GLUT_DEFINITIONS})

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(TINYXML2 REQUIRED tinyxml2)
    include_directories(${TINYXML2_INCLUDE_DIRS})
    link_directories(${TINYXML2_LIBRARY_DIRS})
    target_link_libraries(${PROJECT_NAME} ${TINYXML2_LIBRARIES})

    if (NOT APPLE)
        find_package(GLEW REQUIRED)
        include_directories(${GLEW_INCLUDE_DIRS})
        link_libraries(${GLEW_LIBRARIES})
        target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})
    else()
        target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
    endif()

endif()
