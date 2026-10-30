cmake_minimum_required(VERSION 3.16)

# ==============================================================================
# MediPriority - Sistema de Triage y Priorización de Pacientes en Emergencias
# Cátedra: Estructura de Datos
# Coordinador / Setup: Anthony Aimacaña
# ==============================================================================

project(MediPriority
    VERSION 1.0.0
    DESCRIPTION "Sistema de Triage con Min-Heap Binario Manual y Dear ImGui"
    LANGUAGES CXX C
)

# Estándar C++17 obligatorio
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opciones de compilación estrictas para calidad de código
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Inclusión de directorios base del proyecto
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

# Fuentes iniciales del proyecto
set(SOURCES
    src/main.cpp
)

# Definición del ejecutable principal
add_executable(${PROJECT_NAME} ${SOURCES})

# Mensaje de estado informativo para el evaluador
message(STATUS "--------------------------------------------------------")
message(STATUS "Configuración de CMake completada para: ${PROJECT_NAME}")
message(STATUS "Estándar C++: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Arquitectura: Screaming Architecture (Core, Triage, Presentation)")
message(STATUS "--------------------------------------------------------")
