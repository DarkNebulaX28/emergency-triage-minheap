#ifndef VALIDACION_H
#define VALIDACION_H

#include <string>
#include <cctype>
#include <cstring>
#include <cassert>
#include <iostream>
#include "Paciente.h"
#include "colaPrioridad.h"
#include "operaciones_colaPriori.h"
#include "interfaz.h"

inline bool textoVacio(const std::string& texto)
{
    for (char caracter : texto)
    {
        if (!std::isspace(static_cast<unsigned char>(caracter)))
        {
            return false;
        }
    }
    return true;
}

inline bool nombreContieneSoloLetras(const std::string& nombre)
{
    int cantidadLetras = 0;
    for (char caracter : nombre)
    {
        unsigned char c = static_cast<unsigned char>(caracter);
        if (std::isalpha(c) || c >= 128)
        {
            ++cantidadLetras;
        }
        else if (std::isspace(c) || c == '.' || c == '-')
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return cantidadLetras >= 2;
}

inline bool validarNombre(const std::string& nombre)
{
    if (textoVacio(nombre))
        return false;

    if (nombre.length() > 99)
        return false;

    if (!nombreContieneSoloLetras(nombre))
        return false;

    return true;
}

inline bool validarEdad(int edad)
{
    return edad >= 1 && edad <= 120;
}

inline bool validarSintomas(const std::string& sintomas)
{
    if (textoVacio(sintomas))
        return false;

    if (sintomas.length() > 199)
        return false;

    return true;
}

inline bool validarPrioridad(int prioridad)
{
    return prioridad >= 1 && prioridad <= 5;
}

inline bool validarPosicionCola(int posicionHumana, int totalElementos, std::string& error)
{
    if (totalElementos <= 0)
    {
        error = "Caso limite: La cola esta vacia (frente = nullptr, fin = nullptr).";
        return false;
    }
    if (posicionHumana < 1 || posicionHumana > totalElementos)
    {
        error = "Posicion invalida. Ingrese una posicion entre 1 y " + std::to_string(totalElementos) + ".";
        return false;
    }
    return true;
}

inline bool validarPaciente(
    const std::string& nombre,
    int edad,
    const std::string& sintomas,
    int prioridad,
    std::string& error)
{
    if (textoVacio(nombre))
    {
        error = "Ingrese el nombre completo del paciente.";
        return false;
    }

    if (!nombreContieneSoloLetras(nombre))
    {
        error = "El nombre solo debe contener letras (minimo 2 caracteres, sin numeros).";
        return false;
    }

    if (nombre.length() > 99)
    {
        error = "El nombre es demasiado largo (maximo 99 caracteres).";
        return false;
    }

    if (!validarEdad(edad))
    {
        error = "La edad debe estar entre 1 y 120 anios.";
        return false;
    }

    if (textoVacio(sintomas))
    {
        error = "Ingrese los sintomas o motivo clinico del paciente.";
        return false;
    }

    if (sintomas.length() > 199)
    {
        error = "La descripcion de sintomas es demasiado larga.";
        return false;
    }

    if (!validarPrioridad(prioridad))
    {
        error = "La prioridad debe estar entre 1 (Critica) y 5 (Leve).";
        return false;
    }

    return true;
}

inline void mostrarMenuOpciones()
{
    std::cout << "\n=============================================================\n"
              << "  MEDIPRIORITY - MENU DE OPCIONES DISPONIBLES (GRUPO 2)      \n"
              << "=============================================================\n"
              << "  1. Insertar paciente en Cola de Prioridad (encolar)        \n"
              << "  2. Atender paciente del Frente O(1) (desencolar)           \n"
              << "  3. Consultar paciente en el Frente (consultarFrente)       \n"
              << "  4. Consultar o eliminar en posicion especifica (1..N)      \n"
              << "  5. Mostrar recorrido completo y punteros Frente/Fin        \n"
              << "  6. Demostrar reutilizacion de Template Cola<string>        \n"
              << "  7. Abrir Interfaz Grafica Interactiva (Dear ImGui)         \n"
              << "  0. Salir                                                   \n"
              << "=============================================================\n"
              << "Uso por linea de comandos:\n"
              << "  MediPriority.exe          -> Abre la Interfaz Grafica (UI)\n"
              << "  MediPriority.exe --menu   -> Abre el Menu Interactivo en Consola\n"
              << "  MediPriority.exe --test   -> Ejecuta Pruebas Unitarias del TDA\n"
              << "  MediPriority.exe --help   -> Muestra este menu de opciones\n";
}

inline int ejecutarPruebasUnitarias()
{
    std::cout << "[TEST] Iniciando bateria de pruebas de Cola de Prioridad (Grupo 2)...\n";

    Cola<Paciente> colaPrueba;
    assert(colaPrueba.verificar_vacio() == true);
    assert(colaPrueba.mostrar_tamanio() == 0);

    colaPrueba.encolar(Paciente("Paciente Leve", 30, "Control general"), 5);
    colaPrueba.encolar(Paciente("Paciente Critico A", 65, "Paro cardiaco"), 1);
    colaPrueba.encolar(Paciente("Paciente Medio", 40, "Fiebre moderada"), 3);
    colaPrueba.encolar(Paciente("Paciente Critico B", 52, "Shock severo"), 1);

    assert(colaPrueba.mostrar_tamanio() == 4);

    Paciente atendido;
    int prioAtendida = 0;

    bool ok1 = colaPrueba.desencolarConPrioridad(atendido, prioAtendida);
    assert(ok1 && prioAtendida == 1 && atendido.getNombre() == "Paciente Critico A");

    bool ok2 = colaPrueba.desencolarConPrioridad(atendido, prioAtendida);
    assert(ok2 && prioAtendida == 1 && atendido.getNombre() == "Paciente Critico B");

    bool ok3 = colaPrueba.desencolarConPrioridad(atendido, prioAtendida);
    assert(ok3 && prioAtendida == 3 && atendido.getNombre() == "Paciente Medio");

    bool ok4 = colaPrueba.desencolarConPrioridad(atendido, prioAtendida);
    assert(ok4 && prioAtendida == 5 && atendido.getNombre() == "Paciente Leve");

    assert(colaPrueba.verificar_vacio() == true);

    Cola<std::string> colaAlertas;
    colaAlertas.encolar("Alerta Azul: Consulta externa", 5);
    colaAlertas.encolar("Codigo Rojo: Paro cardiorrespiratorio", 1);
    std::string alertaExtraida;
    int prioAlerta = 0;
    assert(colaAlertas.desencolarConPrioridad(alertaExtraida, prioAlerta) == true);
    assert(prioAlerta == 1 && alertaExtraida == "Codigo Rojo: Paro cardiorrespiratorio");

    std::string error;
    assert(validarPaciente("Juan Perez", 35, "Dolor toracico", 2, error) == true);
    assert(validarPaciente("12345", 35, "Dolor toracico", 2, error) == false);
    assert(validarPaciente("Juan Perez", 150, "Dolor toracico", 2, error) == false);
    assert(validarPaciente("Juan Perez", 35, "   ", 2, error) == false);
    assert(validarPaciente("Juan Perez", 35, "Dolor", 9, error) == false);

    std::cout << "[OK] Todas las pruebas unitarias de estructura de datos, templates y validacion pasaron exitosamente.\n";
    return 0;
}

inline int ejecutarMenuConsola()
{
    Cola<Paciente> colaConsola;
    Cola<std::string> bitacoraEventos;
    int opcion = -1;

    while (opcion != 0)
    {
        mostrarMenuOpciones();
        std::cout << "Seleccione una opcion: ";
        if (!(std::cin >> opcion))
        {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }
        std::cin.ignore(1000, '\n');

        if (opcion == 1)
        {
            std::string nom, sint, err;
            int ed = 0, prio = 1;
            std::cout << "Nombre completo: ";
            std::getline(std::cin, nom);
            std::cout << "Edad (1-120): ";
            std::cin >> ed;
            std::cin.ignore(1000, '\n');
            std::cout << "Sintomas: ";
            std::getline(std::cin, sint);
            std::cout << "Prioridad (1=Critica .. 5=Leve): ";
            std::cin >> prio;
            std::cin.ignore(1000, '\n');

            if (validarPaciente(nom, ed, sint, prio, err))
            {
                Paciente p(nom, ed, sint);
                colaConsola.encolar(p, prio);
                bitacoraEventos.encolar("Ingreso paciente: " + nom, prio);
                std::cout << "[OK] Paciente encolado correctamente.\n";
            }
            else
            {
                std::cout << "[ERROR] " << err << "\n";
            }
        }
        else if (opcion == 2)
        {
            Paciente p;
            int prio = 0;
            if (colaConsola.desencolarConPrioridad(p, prio))
            {
                std::cout << "[ATENDIDO] Prioridad " << prio << " -> " << p << "\n";
            }
            else
            {
                std::cout << "[CASO LIMITE] Cola vacia (frente = nullptr).\n";
            }
        }
        else if (opcion == 3)
        {
            Paciente p;
            int prio = 0;
            if (colaConsola.consultarFrenteConPrioridad(p, prio))
            {
                std::cout << "[FRENTE] Prioridad " << prio << " -> " << p << "\n";
            }
            else
            {
                std::cout << "[CASO LIMITE] Cola vacia (frente = nullptr).\n";
            }
        }
        else if (opcion == 4)
        {
            int pos = 1;
            std::string err;
            std::cout << "Posicion a eliminar (1.." << colaConsola.mostrar_tamanio() << "): ";
            std::cin >> pos;
            if (validarPosicionCola(pos, colaConsola.mostrar_tamanio(), err))
            {
                Paciente elim;
                colaConsola.eliminarEnPosicion(pos - 1, elim);
                std::cout << "[ELIMINADO] " << elim << "\n";
            }
            else
            {
                std::cout << "[ERROR] " << err << "\n";
            }
        }
        else if (opcion == 5)
        {
            colaConsola.mostrarEstado();
        }
        else if (opcion == 6)
        {
            std::cout << "\n--- Estado de Cola<string> (Reutilizacion de Template) ---\n";
            bitacoraEventos.mostrarEstado();
        }
        else if (opcion == 7)
        {
            ejecutarInterfaz();
            break;
        }
    }
    return 0;
}

inline int ejecutarSistemaMediPriority(int argc, char* argv[])
{
    if (argc > 1)
    {
        if (std::strcmp(argv[1], "--test") == 0)
        {
            return ejecutarPruebasUnitarias();
        }
        if (std::strcmp(argv[1], "--menu") == 0)
        {
            return ejecutarMenuConsola();
        }
        if (std::strcmp(argv[1], "--help") == 0 || std::strcmp(argv[1], "-h") == 0)
        {
            mostrarMenuOpciones();
            return 0;
        }
    }

    std::cout << "========================================================\n"
              << " MediPriority - Sistema de Triage y Emergencias Medicas  \n"
              << " Grupo 2: Colas de Prioridad Dinamicas (Sin STL)         \n"
              << " Estado: Iniciando interfaz grafica interactiva...       \n"
              << "========================================================\n";

    ejecutarInterfaz();
    return 0;
}

#endif
