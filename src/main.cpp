#include <iostream>
#include <cstring>
#include <cassert>
#include "interfaz.h"
#include "Paciente.h"
#include "colaPrioridad.h"
#include "operaciones_colaPriori.h"
#include "validacion.h"

static int ejecutarPruebasUnitarias()
{
    std::cout << "[TEST] Iniciando bateria de pruebas de Cola de Prioridad...\n";

    Cola<Paciente> colaPrueba;
    assert(colaPrueba.verificar_vacio() == true);
    assert(colaPrueba.mostrar_tamanio() == 0);

    // 1. Prueba de insercion y ordenamiento por severidad (1 = Critica, 5 = Leve)
    colaPrueba.encolar(Paciente("Paciente Leve", 30, "Control general"), 5);
    colaPrueba.encolar(Paciente("Paciente Critico A", 65, "Paro cardiaco"), 1);
    colaPrueba.encolar(Paciente("Paciente Medio", 40, "Fiebre moderada"), 3);
    colaPrueba.encolar(Paciente("Paciente Critico B", 52, "Shock severo"), 1);

    assert(colaPrueba.mostrar_tamanio() == 4);

    // 2. Verificar estabilidad FIFO en empate de prioridad 1
    Paciente atendido;
    int prioAtendida = 0;

    bool ok1 = colaPrueba.desencolarConPrioridad(atendido, prioAtendida);
    assert(ok1 && prioAtendida == 1 && atendido.nombre == "Paciente Critico A");

    bool ok2 = colaPrueba.desencolarConPrioridad(atendido, prioAtendida);
    assert(ok2 && prioAtendida == 1 && atendido.nombre == "Paciente Critico B");

    // 3. Verificar orden de prioridades restantes (3 antes que 5)
    bool ok3 = colaPrueba.desencolarConPrioridad(atendido, prioAtendida);
    assert(ok3 && prioAtendida == 3 && atendido.nombre == "Paciente Medio");

    bool ok4 = colaPrueba.desencolarConPrioridad(atendido, prioAtendida);
    assert(ok4 && prioAtendida == 5 && atendido.nombre == "Paciente Leve");

    assert(colaPrueba.verificar_vacio() == true);

    // 4. Prueba de validacion de entradas
    std::string error;
    assert(validarPaciente("Juan Perez", 35, "Dolor toracico", 2, error) == true);
    assert(validarPaciente("12345", 35, "Dolor toracico", 2, error) == false);
    assert(validarPaciente("Juan Perez", 150, "Dolor toracico", 2, error) == false);
    assert(validarPaciente("Juan Perez", 35, "   ", 2, error) == false);
    assert(validarPaciente("Juan Perez", 35, "Dolor", 9, error) == false);

    std::cout << "[OK] Todas las pruebas unitarias de estructura de datos y validacion pasaron exitosamente.\n";
    return 0;
}

static void mostrarAyuda()
{
    std::cout << "Uso: MediPriority [opciones]\n"
              << "Opciones:\n"
              << "  (sin argumentos)  Inicia la interfaz grafica interactiva (Dear ImGui + OpenGL3)\n"
              << "  --test            Ejecuta la bateria de pruebas unitarias de la Cola de Prioridad\n"
              << "  --help            Muestra este mensaje de ayuda\n";
}

int main(int argc, char* argv[])
{
    if (argc > 1)
    {
        if (std::strcmp(argv[1], "--test") == 0)
        {
            return ejecutarPruebasUnitarias();
        }
        if (std::strcmp(argv[1], "--help") == 0 || std::strcmp(argv[1], "-h") == 0)
        {
            mostrarAyuda();
            return 0;
        }
    }

    std::cout << "========================================================\n";
    std::cout << " MediPriority - Sistema de Triage y Emergencias Medicas  \n";
    std::cout << " Catedra: Estructura de Datos                           \n";
    std::cout << " Estado: Iniciando interfaz grafica interactiva...       \n";
    std::cout << "========================================================\n";

    ejecutarInterfaz();

    return 0;
}
