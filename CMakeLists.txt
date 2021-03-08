cmake_minimum_required(VERSION 3.5)

macro(copy_asset)
    foreach(ASSET IN ITEMS ${ARGN})
        target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${ASSET})
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${ASSET} ${CMAKE_CURRENT_BINARY_DIR}/${ASSET} COPYONLY)
    endforeach()
endmacro()


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(HunterGate)
HunterGate(
    URL "https://github.com/cpp-pm/hunter/archive/v0.23.294.tar.gz"
    SHA1 "0dd1ee8723d54a15822519c17a877c1f281fce39"
)

project(ProjectionTester)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

hunter_add_package(SDL2)
find_package(SDL2 CONFIG REQUIRED)

hunter_add_package(SDL_image)
find_package(SDL_image CONFIG REQUIRED)

hunter_add_package(glew)
find_package(glew CONFIG REQUIRED)

hunter_add_package(glm)
find_package(glm REQUIRED)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE WIN32
    AttributeInfo.h
    BaseApp.h
    Shader.h
    ShaderProgram.h
    Texture.h
    VertexBuffer.h
    imgui/imconfig.h
    imgui/imgui.cpp
    imgui/imgui.h
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_opengl3.h
    imgui/backends/imgui_impl_sdl.h
    imgui/backends/imgui_impl_sdl.cpp
    imgui/backends/imgui_impl_sdl.h
    main.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/imgui
            ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE SDL2::SDL2
            SDL2::SDL2main
            SDL_image::SDL_image
            glew::glew
            glm
)

copy_asset(
	background.jpg
	default.frag
	default.vert
	fill.frag
	fill.vert
	mike.png
)

