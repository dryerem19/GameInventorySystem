cmake_minimum_required(VERSION 3.5)

project(inventory_system LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IMGUI_SOURCE
    external/imgui/imgui.h
    external/imgui/imgui.cpp
    external/imgui/imconfig.h
    external/imgui/imgui_internal.h

    external/imgui/imstb_textedit.h
    external/imgui/imstb_rectpack.h
    external/imgui/imstb_truetype.h

    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp

    external/imgui/imgui_impl_glfw.h
    external/imgui/imgui_impl_glfw.cpp
    external/imgui/imgui_impl_opengl3.h
    external/imgui/imgui_impl_opengl3.cpp
    external/imgui/imgui_impl_opengl3_loader.h
)

set(PROJECT_SOURCE
    item.h
    storage.h
    storage.cpp
)

add_executable(${PROJECT_NAME}
    main.cpp
    ${IMGUI_SOURCE}
    ${PROJECT_SOURCE}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    external/imgui
)

target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/lib)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/external/include")

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)
target_link_libraries(${PROJECT_NAME} PRIVATE glew32)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/data/fa-solid-900.ttf"
        "${PROJECT_BINARY_DIR}/fa-solid-900.ttf"
)
