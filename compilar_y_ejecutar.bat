@echo off
setlocal EnableDelayedExpansion
title MediPriority - Compilador y Ejecutor Automatico
echo ======================================================================
echo  MediPriority - Sistema de Triage y Priorizacion de Pacientes
echo  Compilando proyecto C++17 con Dear ImGui + OpenGL3...
echo ======================================================================

set "GPP_CMD="
where g++ >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    set "GPP_CMD=g++"
) else if exist "C:\MinGW\bin\g++.exe" (
    set "PATH=C:\MinGW\bin;%PATH%"
    set "GPP_CMD=C:\MinGW\bin\g++.exe"
) else if exist "C:\msys64\mingw64\bin\g++.exe" (
    set "PATH=C:\msys64\mingw64\bin;%PATH%"
    set "GPP_CMD=C:\msys64\mingw64\bin\g++.exe"
) else if exist "C:\msys64\ucrt64\bin\g++.exe" (
    set "PATH=C:\msys64\ucrt64\bin;%PATH%"
    set "GPP_CMD=C:\msys64\ucrt64\bin\g++.exe"
) else if exist "C:\Program Files\CodeBlocks\MinGW\bin\g++.exe" (
    set "PATH=C:\Program Files\CodeBlocks\MinGW\bin;%PATH%"
    set "GPP_CMD=C:\Program Files\CodeBlocks\MinGW\bin\g++.exe"
) else if exist "C:\Program Files (x86)\CodeBlocks\MinGW\bin\g++.exe" (
    set "PATH=C:\Program Files (x86)\CodeBlocks\MinGW\bin;%PATH%"
    set "GPP_CMD=C:\Program Files (x86)\CodeBlocks\MinGW\bin\g++.exe"
)

if "%GPP_CMD%"=="" (
    echo [ERROR] No se encontro un compilador g++ en el sistema.
    echo Instale MinGW / MSYS2 / CodeBlocks o agregue g++ al PATH de Windows.
    pause
    exit /b 1
)

echo [INFO] Usando compilador: %GPP_CMD%

"%GPP_CMD%" -dumpmachine | findstr "64" >nul
if %ERRORLEVEL% EQU 0 (
    echo [INFO] Compilador de 64 bits detectado. Enlazando con GLFW 64-bit...
    "%GPP_CMD%" -std=c++17 -O2 src/main.cpp src/interfaz.cpp external/imgui/imgui.cpp external/imgui/imgui_draw.cpp external/imgui/imgui_tables.cpp external/imgui/imgui_widgets.cpp external/imgui/backends/imgui_impl_opengl3.cpp external/imgui/backends/imgui_impl_glfw.cpp -Isrc -Iexternal/imgui -Iexternal/imgui/backends -Iexternal/glfw-3.5.1.bin.WIN64/include external/glfw-3.5.1.bin.WIN64/lib-mingw-w64/libglfw3.a -lopengl32 -lgdi32 -luser32 -lshell32 -limm32 -o MediPriority.exe
) else (
    echo [INFO] Compilador de 32 bits detectado. Enlazando con Win32 + OpenGL3...
    "%GPP_CMD%" -std=c++17 -O2 src/main.cpp src/interfaz.cpp external/imgui/imgui.cpp external/imgui/imgui_draw.cpp external/imgui/imgui_tables.cpp external/imgui/imgui_widgets.cpp external/imgui/backends/imgui_impl_opengl3.cpp external/imgui/backends/imgui_impl_win32.cpp -Isrc -Iexternal/imgui -Iexternal/imgui/backends -lopengl32 -lgdi32 -luser32 -limm32 -o MediPriority.exe
)

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Fallo la compilacion.
    pause
    exit /b 1
)

echo [OK] Compilacion exitosa: MediPriority.exe generado.
echo [INFO] Ejecutando pruebas unitarias automaticas...
MediPriority.exe --test

echo [INFO] Iniciando interfaz grafica MediPriority...
start "" MediPriority.exe
