cmake_minimum_required(VERSION 3.14)

project(MediPriorityTriage
    VERSION 1.0.0
    DESCRIPTION "MediPriority - Sistema de Triage y Priorizacion de Pacientes en Emergencias"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui")
set(GLFW_DIR  "${CMAKE_SOURCE_DIR}/external/glfw-3.5.1.bin.WIN64")

set(SOURCES
    src/main.cpp
    src/interfaz.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4)
    # Compilador Windows de 32 bits (ej. MinGW clasico): usa backend nativo Win32 + WGL
    list(APPEND SOURCES ${IMGUI_DIR}/backends/imgui_impl_win32.cpp)
else()
    # Compilador de 64 bits: usa backend GLFW + OpenGL3
    list(APPEND SOURCES ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp)
endif()

add_executable(MediPriority ${SOURCES})

target_include_directories(MediPriority PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
    "${GLFW_DIR}/include"
)

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        target_link_libraries(MediPriority PRIVATE opengl32 gdi32 user32 imm32)
    else()
        if(MINGW)
            target_link_libraries(MediPriority PRIVATE
                "${GLFW_DIR}/lib-mingw-w64/libglfw3.a"
                opengl32 gdi32 user32 shell32 imm32
            )
        elseif(MSVC)
            target_link_libraries(MediPriority PRIVATE
                "${GLFW_DIR}/lib-vc2022/glfw3.lib"
                opengl32 gdi32 user32 shell32 imm32
            )
        endif()
    endif()
endif()
