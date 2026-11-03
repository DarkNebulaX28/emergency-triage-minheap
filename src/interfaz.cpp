#include "interfaz.h"
#include "Paciente.h"
#include "colaPrioridad.h"
#include "operaciones_colaPriori.h"
#include "validacion.h"

#include <string>
#include <cstring>
#include <cstdio>

#include "imgui.h"
#include "imgui_impl_opengl3.h"

#if defined(_WIN32) && !defined(_WIN64) && !defined(FORCE_GLFW_BACKEND)
    #define USAR_BACKEND_WIN32 1
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <GL/gl.h>
    #include "imgui_impl_win32.h"
    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#else
    #define USAR_BACKEND_WIN32 0
    #include "imgui_impl_glfw.h"
    #include <GLFW/glfw3.h>
#endif

using namespace std;

// ==========================================
// VARIABLES GLOBALES DE ESTADO

Cola<Paciente> colaPacientes;

ImFont* fuenteRegular = nullptr;
ImFont* fuenteBold = nullptr;

char nombre[100] = "";
char sintomas[256] = "";

int edad = 25;
int prioridadCombo = 0;

string mensaje = "Sistema de triaje listo para admision de pacientes.";
bool mensajeError = false;

// Estado del consultorio activo (ultimo paciente desencolado)
bool hayPacienteEnAtencion = false;
Paciente ultimoPacienteAtendido;
int prioridadUltimoAtendido = 1;
int totalPacientesAtendidos = 0;

// Modo de vista del panel inferior (0 = Tarjetas Nodos Enlazados, 1 = Tabla de Memoria)
int modoVistaCola = 0;

// ==========================================
// COLORES Y METADATOS DE PRIORIDADES (MANCHESTER)

ImVec4 obtenerColor(int prioridad)
{
    switch (prioridad)
    {
        case 1: return ImVec4(0.95f, 0.22f, 0.27f, 1.0f); // Rojo - Critica
        case 2: return ImVec4(1.00f, 0.50f, 0.16f, 1.0f); // Naranja - Alta
        case 3: return ImVec4(0.95f, 0.76f, 0.20f, 1.0f); // Amarillo - Media
        case 4: return ImVec4(0.18f, 0.78f, 0.48f, 1.0f); // Verde - Baja
        case 5: return ImVec4(0.28f, 0.62f, 1.00f, 1.0f); // Azul - Leve
        default: return ImVec4(0.60f, 0.60f, 0.65f, 1.0f);
    }
}

string obtenerNombrePrioridad(int prioridad)
{
    switch (prioridad)
    {
        case 1: return "1 - CRITICA (Resucitacion)";
        case 2: return "2 - ALTA (Emergencia)";
        case 3: return "3 - MEDIA (Urgencia)";
        case 4: return "4 - BAJA (Menor)";
        case 5: return "5 - LEVE (No Urgente)";
        default: return "DESCONOCIDA";
    }
}

const char* obtenerTiempoObjetivo(int prioridad)
{
    switch (prioridad)
    {
        case 1: return "Atencion: Inmediata (0 min)";
        case 2: return "Atencion: <= 10 min";
        case 3: return "Atencion: <= 60 min";
        case 4: return "Atencion: <= 120 min";
        case 5: return "Atencion: <= 240 min";
        default: return "";
    }
}

// ==========================================
// FUNCION PARA TITULOS EN NEGRITA

void dibujarTitulo(const char* texto)
{
    if (fuenteBold != nullptr)
        ImGui::PushFont(fuenteBold);

    ImGui::Text("%s", texto);

    if (fuenteBold != nullptr)
        ImGui::PopFont();
}

// ==========================================
// ESTILO DE LA INTERFAZ

void configurarEstilo()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 12.0f;
    style.ChildRounding = 10.0f;
    style.FrameRounding = 7.0f;
    style.PopupRounding = 7.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 7.0f;

    style.WindowPadding = ImVec2(18, 16);
    style.FramePadding = ImVec2(10, 7);
    style.ItemSpacing = ImVec2(10, 9);

    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;

    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.035f, 0.045f, 0.075f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.065f, 0.080f, 0.125f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.16f, 0.20f, 0.29f, 1.0f);
    style.Colors[ImGuiCol_Text] = ImVec4(0.92f, 0.94f, 0.98f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.56f, 0.66f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.12f, 0.18f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.14f, 0.18f, 0.27f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.17f, 0.22f, 0.33f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.13f, 0.32f, 0.58f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.18f, 0.42f, 0.72f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.10f, 0.26f, 0.48f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.13f, 0.25f, 0.42f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.17f, 0.34f, 0.55f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.12f, 0.28f, 0.47f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.04f, 0.05f, 0.08f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.22f, 0.27f, 0.37f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.10f, 0.14f, 0.22f, 1.0f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.05f, 0.07f, 0.11f, 0.85f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.08f, 0.10f, 0.16f, 0.85f);
}

// ==========================================
// AGREGAR PACIENTE

void agregarPaciente()
{
    string nombreTexto = nombre;
    string sintomasTexto = sintomas;

    int prioridad = prioridadCombo + 1;
    string error;

    // Validar todos los datos del paciente
    if (!validarPaciente(
            nombreTexto,
            edad,
            sintomasTexto,
            prioridad,
            error))
    {
        mensaje = error;
        mensajeError = true;
        return;
    }

    int posEstimada = colaPacientes.obtenerPosicionInsercion(prioridad) + 1;

    // Crear el paciente
    Paciente nuevoPaciente(
        nombreTexto,
        edad,
        sintomasTexto
    );

    // Insertar en la cola de prioridad
    colaPacientes.encolar(
        nuevoPaciente,
        prioridad
    );

    mensaje = "Paciente #" + std::to_string(nuevoPaciente.id) + " (" + nuevoPaciente.nombre +
              ") encolado en posicion #" + std::to_string(posEstimada) + ".";
    mensajeError = false;

    // Limpiar formulario
    nombre[0] = '\0';
    sintomas[0] = '\0';

    edad = 25;
    prioridadCombo = 0;
}

// ==========================================
// ATENDER PACIENTE

void atenderPaciente()
{
    if (colaPacientes.verificar_vacio())
    {
        mensaje = "No hay pacientes en espera en la cola de prioridad.";
        mensajeError = true;
        return;
    }

    Paciente pacienteAtendido;
    int prioridadAtendida = 1;

    if (colaPacientes.desencolarConPrioridad(pacienteAtendido, prioridadAtendida))
    {
        ultimoPacienteAtendido = pacienteAtendido;
        prioridadUltimoAtendido = prioridadAtendida;
        hayPacienteEnAtencion = true;
        ++totalPacientesAtendidos;

        mensaje = "Desencolado al consultorio: [#" + std::to_string(pacienteAtendido.id) + "] " +
                  pacienteAtendido.nombre + " (" + obtenerNombrePrioridad(prioridadAtendida) + ")";
        mensajeError = false;
    }
}

// ==========================================
// CARGAR PACIENTES DEMO PARA DEFENSA ACADEMICA

void cargarPacientesDemo()
{
    colaPacientes.encolar(Paciente("Carlos Mendoza", 58, "Dolor toracico agudo e irradiacion a brazo izquierdo"), 2);
    colaPacientes.encolar(Paciente("Lucia Salazar", 24, "Esguince de tobillo grado 1 al caminar"), 4);
    colaPacientes.encolar(Paciente("Roberto Vega", 67, "Paro cardiorrespiratorio / perdida de consciencia"), 1);
    colaPacientes.encolar(Paciente("Ana Gomez", 34, "Crisis hipertensiva severa y disnea aguda"), 2);
    colaPacientes.encolar(Paciente("Miguel Torres", 19, "Fiebre moderada de 38.5C y dolor abdominal"), 3);
    colaPacientes.encolar(Paciente("Elena Paredes", 45, "Trauma craneoencefalico severo con hemorragia"), 1);

    mensaje = "Se cargaron 6 pacientes de prueba (observe el orden por prioridad y desempate FIFO).";
    mensajeError = false;
}

// ==========================================
// TARJETA DE PACIENTE (NODO DE LA COLA)

void dibujarTarjeta(Paciente paciente, int prioridad, int indice, int totalEnCola, int& indiceSubirPrio, int& indiceEliminar)
{
    ImVec4 color = obtenerColor(prioridad);

    ImVec4 fondo(
        color.x * 0.20f,
        color.y * 0.20f,
        color.z * 0.20f,
        0.48f
    );

    const void* dirActual = colaPacientes.obtenerDireccionNodo(indice);
    const void* dirSig = colaPacientes.obtenerDireccionSiguiente(indice);

    ImGui::PushID(indice);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, fondo);
    ImGui::PushStyleColor(ImGuiCol_Border, color);

    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, indice == 0 ? 2.8f : 1.8f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);

    ImGui::BeginChild(
        "TarjetaPaciente",
        ImVec2(265, 200),
        true
    );

    // Etiqueta de posicion en la lista enlazada (FRENTE / FIN / NODO)
    if (indice == 0 && totalEnCola == 1)
    {
        ImGui::TextColored(ImVec4(0.40f, 1.00f, 0.65f, 1.0f), "[FRENTE / FIN] Turno #1");
    }
    else if (indice == 0)
    {
        ImGui::TextColored(ImVec4(0.40f, 1.00f, 0.65f, 1.0f), "[FRENTE -> Sig. en pasar] #1");
    }
    else if (indice == totalEnCola - 1)
    {
        ImGui::TextColored(ImVec4(0.75f, 0.82f, 0.95f, 1.0f), "[FIN DE COLA] Turno #%d", indice + 1);
    }
    else
    {
        ImGui::TextDisabled("Nodo #%d en cola", indice + 1);
    }

    if (fuenteBold != nullptr)
        ImGui::PushFont(fuenteBold);

    ImGui::TextColored(
        color,
        "%s",
        obtenerNombrePrioridad(prioridad).c_str()
    );

    if (fuenteBold != nullptr)
        ImGui::PopFont();

    ImGui::Separator();

    ImGui::Text(
        "ID: #%d | Hora: %s | Edad: %d",
        paciente.id,
        paciente.horaIngreso.c_str(),
        paciente.edad
    );

    ImGui::TextWrapped(
        "Paciente: %s",
        paciente.nombre.c_str()
    );

    ImGui::TextWrapped(
        "Sintomas: %s",
        paciente.sintomas.c_str()
    );

    ImGui::Spacing();
    ImGui::TextDisabled("Nodo*: %p -> sig: %p", dirActual, dirSig);

    // Botones rapidos para re-categorizar (subir urgencia) o retirar
    if (prioridad > 1)
    {
        if (ImGui::SmallButton("^ Subir Urgencia"))
        {
            indiceSubirPrio = indice;
        }
        ImGui::SameLine();
    }

    if (ImGui::SmallButton("Retirar"))
    {
        indiceEliminar = indice;
    }

    ImGui::EndChild();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);

    ImGui::PopID();
}

// ==========================================
// VISTA PREVIA Y BOX DE ATENCION

void dibujarVistaPrevia()
{
    float anchoDisp = ImGui::GetContentRegionAvail().x;
    float mitadAncho = (anchoDisp - 12.0f) * 0.50f;

    // ---------------------------------------------------------
    // SUB-PANEL 1: VISTA PREVIA DE INGRESO Y POSICION ESTIMADA
    // ---------------------------------------------------------
    ImGui::BeginChild("SubPanelPrevia", ImVec2(mitadAncho, 0), false);

    dibujarTitulo("VISTA PREVIA DE ADMISION");
    ImGui::Spacing();

    int prioridad = prioridadCombo + 1;
    int posInsercion = colaPacientes.obtenerPosicionInsercion(prioridad);
    int totalActual = colaPacientes.mostrar_tamanio();

    ImVec4 color = obtenerColor(prioridad);
    ImVec4 fondo(
        color.x * 0.22f,
        color.y * 0.22f,
        color.z * 0.22f,
        0.42f
    );

    ImGui::PushStyleColor(ImGuiCol_ChildBg, fondo);
    ImGui::PushStyleColor(ImGuiCol_Border, color);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);

    ImGui::BeginChild(
        "VistaPreviaTarjeta",
        ImVec2(0, 0),
        true
    );

    if (fuenteBold != nullptr)
        ImGui::PushFont(fuenteBold);

    ImGui::TextColored(
        color,
        "%s",
        obtenerNombrePrioridad(prioridad).c_str()
    );

    if (fuenteBold != nullptr)
        ImGui::PopFont();

    ImGui::TextDisabled("%s", obtenerTiempoObjetivo(prioridad));
    ImGui::Separator();

    ImGui::TextWrapped(
        "Paciente: %s",
        nombre[0] ? nombre : "(Escriba el nombre en el formulario)"
    );

    ImGui::Text(
        "Edad: %d anios",
        edad
    );

    ImGui::TextWrapped(
        "Sintomas: %s",
        sintomas[0] ? sintomas : "(Escriba los sintomas)"
    );

    ImGui::Spacing();
    ImGui::Separator();

    // Informacion de posicion calculada por obtenerPosicionInsercion()
    ImGui::TextColored(
        ImVec4(0.45f, 0.85f, 1.0f, 1.0f),
        "Posicion de insercion: Turno #%d (de %d)",
        posInsercion + 1,
        totalActual + 1
    );

    if (posInsercion == 0)
    {
        ImGui::TextDisabled("-> Se insertara al FRENTE de la cola.");
    }
    else if (posInsercion == totalActual)
    {
        ImGui::TextDisabled("-> Se insertara al FIN de la cola (FIFO).");
    }
    else
    {
        ImGui::TextDisabled("-> Se insertara tras nodos con prioridad <= %d.", prioridad);
    }

    ImGui::EndChild();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);

    ImGui::EndChild();

    ImGui::SameLine();

    // ---------------------------------------------------------
    // SUB-PANEL 2: CONSULTORIO ACTIVO (ULTIMO PACIENTE ATENDIDO)
    // ---------------------------------------------------------
    ImGui::BeginChild("SubPanelConsultorio", ImVec2(0, 0), false);

    dibujarTitulo("BOX MEDICO EN ATENCION");
    ImGui::Spacing();

    ImGui::BeginChild("TarjetaConsultorio", ImVec2(0, 0), true);

    if (!hayPacienteEnAtencion)
    {
        ImGui::TextDisabled("Consultorio disponible.");
        ImGui::Spacing();
        ImGui::TextWrapped(
            "Presione 'ATENDER PACIENTE' para desencolar (O(1)) el nodo del FRENTE y asignarlo al box medico."
        );
    }
    else
    {
        ImVec4 colorAtendido = obtenerColor(prioridadUltimoAtendido);
        ImGui::TextColored(ImVec4(0.35f, 0.95f, 0.65f, 1.0f), "EN CONSULTA ACTIVA (Extraido de Frente):");

        if (fuenteBold != nullptr)
            ImGui::PushFont(fuenteBold);

        ImGui::TextColored(
            colorAtendido,
            "%s",
            obtenerNombrePrioridad(prioridadUltimoAtendido).c_str()
        );

        if (fuenteBold != nullptr)
            ImGui::PopFont();

        ImGui::Separator();
        ImGui::Text("ID Ticket: #%d | Ingreso: %s", ultimoPacienteAtendido.id, ultimoPacienteAtendido.horaIngreso.c_str());
        ImGui::TextWrapped("Nombre: %s", ultimoPacienteAtendido.nombre.c_str());
        ImGui::Text("Edad: %d anios", ultimoPacienteAtendido.edad);
        ImGui::TextWrapped("Cuadro Clinico: %s", ultimoPacienteAtendido.sintomas.c_str());
    }

    ImGui::Spacing();
    ImGui::Separator();

    // Mostrar quien sigue en el frente de la cola (consultarFrente)
    Paciente sigPaciente;
    int sigPrio = 0;
    if (colaPacientes.consultarFrenteConPrioridad(sigPaciente, sigPrio))
    {
        ImGui::TextColored(
            obtenerColor(sigPrio),
            "Proximo en Frente: #%d %s (Prio %d)",
            sigPaciente.id,
            sigPaciente.nombre.c_str(),
            sigPrio
        );
    }
    else
    {
        ImGui::TextDisabled("Proximo en Frente: Ninguno (Cola vacia)");
    }

    ImGui::EndChild();
    ImGui::EndChild();
}

// ==========================================
// INTERFAZ PRINCIPAL

void dibujarInterfaz()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin(
        "Sistema de Triaje",
        nullptr,
        flags
    );

    // ======================================
    // ENCABEZADO Y BARRA DE METRICAS (KPIs)
    // ======================================

    if (fuenteBold != nullptr)
        ImGui::PushFont(fuenteBold);

    ImGui::TextColored(
        ImVec4(0.35f, 0.72f, 1.0f, 1.0f),
        "MEDIPRIORITY - SISTEMA DE TRIAJE Y PRIORIZACION EN EMERGENCIAS"
    );

    if (fuenteBold != nullptr)
        ImGui::PopFont();

    ImGui::SameLine();
    ImGui::TextDisabled("| Cola de Prioridad Dinamica (Sin STL)");

    // Botones de utilidad academica en la esquina superior derecha
    float anchoAccionesTop = 310.0f;
    if (ImGui::GetWindowWidth() > 750.0f)
    {
        ImGui::SameLine(ImGui::GetWindowWidth() - anchoAccionesTop);
    }

    if (ImGui::Button("Cargar Demo (6)", ImVec2(150, 28)))
    {
        cargarPacientesDemo();
    }
    ImGui::SameLine();
    if (ImGui::Button("Vaciar Cola", ImVec2(130, 28)))
    {
        colaPacientes.limpiar();
        mensaje = "Cola de prioridad vaciada correctamente (memoria liberada).";
        mensajeError = false;
    }

    // Barra de contadores por nivel de prioridad
    ImGui::Spacing();
    for (int p = 1; p <= 5; ++p)
    {
        int cuentaNivel = colaPacientes.contarPorPrioridad(p);
        ImGui::TextColored(obtenerColor(p), "[N%d: %d]", p, cuentaNivel);
        ImGui::SameLine();
    }
    ImGui::TextDisabled(
        "| En Espera: %d | Atendidos: %d | Saltos/Ops Punteros: %d",
        colaPacientes.mostrar_tamanio(),
        totalPacientesAtendidos,
        colaPacientes.obtenerTotalOperaciones()
    );

    ImGui::Separator();
    ImGui::Spacing();

    float ancho = ImGui::GetContentRegionAvail().x;
    float alto = ImGui::GetContentRegionAvail().y;

    float anchoFormulario = ancho * 0.44f;
    float altoSuperior = alto * 0.52f;

    // ======================================
    // PANEL IZQUIERDO: FORMULARIO
    // ======================================

    ImGui::BeginChild(
        "PanelFormulario",
        ImVec2(anchoFormulario, altoSuperior),
        true
    );

    dibujarTitulo("REGISTRAR PACIENTE");
    ImGui::Spacing();

    // Nombre
    ImGui::Text("Nombre completo (solo letras)");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText(
        "##Nombre",
        nombre,
        sizeof(nombre)
    );

    // Edad
    ImGui::Text("Edad (1 - 120 anios)");
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputInt("##Edad", &edad))
    {
        if (edad < 0) edad = 0;
        if (edad > 150) edad = 150;
    }

    // Sintomas
    ImGui::Text("Sintomas / Motivo clinico");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextMultiline(
        "##Sintomas",
        sintomas,
        sizeof(sintomas),
        ImVec2(-1, 52)
    );

    // Prioridad (Combo + Botones rapidos de color Manchester)
    ImGui::Text("Nivel de prioridad (Escala Manchester)");

    const char* prioridades[] = {
        "1 - CRITICA (Resucitacion - Inmediato)",
        "2 - ALTA (Emergencia - <= 10 min)",
        "3 - MEDIA (Urgencia - <= 60 min)",
        "4 - BAJA (Menor - <= 120 min)",
        "5 - LEVE (No Urgente - <= 240 min)"
    };

    ImGui::SetNextItemWidth(-1);
    ImGui::Combo(
        "##Prioridad",
        &prioridadCombo,
        prioridades,
        IM_ARRAYSIZE(prioridades)
    );

    // Botones rapidos de seleccion de color
    const char* etiquetasCortas[] = { "1-Critica", "2-Alta", "3-Media", "4-Baja", "5-Leve" };
    float anchoBtnPrio = (ImGui::GetContentRegionAvail().x - 32.0f) / 5.0f;
    for (int i = 0; i < 5; ++i)
    {
        ImVec4 colPrio = obtenerColor(i + 1);
        bool seleccionado = (prioridadCombo == i);
        ImGui::PushStyleColor(ImGuiCol_Button, seleccionado ? colPrio : ImVec4(colPrio.x * 0.45f, colPrio.y * 0.45f, colPrio.z * 0.45f, 0.85f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colPrio);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, colPrio);
        if (ImGui::Button(etiquetasCortas[i], ImVec2(anchoBtnPrio, 24)))
        {
            prioridadCombo = i;
        }
        ImGui::PopStyleColor(3);
        if (i < 4) ImGui::SameLine();
    }

    ImGui::Spacing();

    // ======================================
    // BOTON REGISTRAR

    if (fuenteBold != nullptr)
        ImGui::PushFont(fuenteBold);

    if (ImGui::Button(
            "REGISTRAR PACIENTE EN COLA",
            ImVec2(-1, 38)))
    {
        agregarPaciente();
    }

    if (fuenteBold != nullptr)
        ImGui::PopFont();

    // ======================================
    // MENSAJES DE VALIDACION Y ESTADO

    if (mensajeError)
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.35f, 0.35f, 1.0f),
            "[!] %s",
            mensaje.c_str()
        );
    }
    else
    {
        ImGui::TextColored(
            ImVec4(0.35f, 0.90f, 0.62f, 1.0f),
            "[OK] %s",
            mensaje.c_str()
        );
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // ======================================
    // PANEL DERECHO: VISTA PREVIA Y BOX MEDICO

    ImGui::BeginChild(
        "PanelVistaPrevia",
        ImVec2(0, altoSuperior),
        true
    );

    dibujarVistaPrevia();

    ImGui::EndChild();

    // ======================================
    // PANEL INFERIOR: COLA DE PACIENTES E INSPECTOR DE PUNTEROS

    ImGui::Spacing();

    ImGui::BeginChild(
        "PanelCola",
        ImVec2(0, 0),
        true
    );

    int cantidad = colaPacientes.mostrar_tamanio();

    dibujarTitulo("COLA DE ATENCION PRIORIZADA");

    ImGui::SameLine();
    ImGui::TextDisabled("| Total: %d pacientes", cantidad);

    ImGui::SameLine();
    if (ImGui::RadioButton("Tarjetas de Nodos (Frente -> Fin)", modoVistaCola == 0))
    {
        modoVistaCola = 0;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Tabla de Memoria y Punteros (Nodo*)", modoVistaCola == 1))
    {
        modoVistaCola = 1;
    }

    float anchoBoton = 195.0f;
    if (ImGui::GetWindowWidth() > 800.0f)
    {
        ImGui::SameLine(ImGui::GetWindowWidth() - anchoBoton - 18.0f);
    }

    // ======================================
    // BOTON ATENDER

    if (fuenteBold != nullptr)
        ImGui::PushFont(fuenteBold);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.56f, 0.36f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f, 0.68f, 0.44f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.11f, 0.46f, 0.30f, 1.0f));

    if (ImGui::Button(
            "ATENDER PACIENTE",
            ImVec2(anchoBoton, 38)))
    {
        atenderPaciente();
    }

    ImGui::PopStyleColor(3);

    if (fuenteBold != nullptr)
        ImGui::PopFont();

    ImGui::Separator();
    ImGui::Spacing();

    // ======================================
    // MOSTRAR PACIENTES

    int indiceSubirPrio = -1;
    int indiceEliminar = -1;

    if (cantidad == 0)
    {
        ImGui::Spacing();
        ImGui::TextDisabled(
            "Cola vacia (frente = nullptr, fin = nullptr). Registre un paciente o pulse 'Cargar Demo (6)'."
        );
    }
    else if (modoVistaCola == 0)
    {
        // Sub-contenedor con Scroll Horizontal habilitado para que nunca se corten las tarjetas
        ImGui::BeginChild(
            "ScrollHorizontalTarjetas",
            ImVec2(0, 0),
            false,
            ImGuiWindowFlags_HorizontalScrollbar
        );

        for (int i = 0; i < cantidad; i++)
        {
            Paciente pacienteActual;
            int prioridad = 0;

            colaPacientes.obtenerElemento(
                i,
                pacienteActual,
                prioridad
            );

            dibujarTarjeta(
                pacienteActual,
                prioridad,
                i,
                cantidad,
                indiceSubirPrio,
                indiceEliminar
            );

            ImGui::SameLine();

            // Dibujar conector visual de puntero sig ->
            ImGui::BeginGroup();
            ImGui::Dummy(ImVec2(0, 75));
            if (i < cantidad - 1)
            {
                ImGui::TextColored(ImVec4(0.45f, 0.75f, 1.0f, 1.0f), "sig -->");
            }
            else
            {
                ImGui::TextDisabled("sig -> NULL");
            }
            ImGui::EndGroup();

            if (i < cantidad - 1)
            {
                ImGui::SameLine();
            }
        }

        ImGui::EndChild();
    }
    else
    {
        // Vista de Tabla de Memoria y Punteros para sustentacion ante el docente
        ImGuiTableFlags tableFlags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_Resizable;

        if (ImGui::BeginTable("TablaNodosMemoria", 9, tableFlags))
        {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Posicion", ImGuiTableColumnFlags_WidthFixed, 95.0f);
            ImGui::TableSetupColumn("Nodo* (Mem)", ImGuiTableColumnFlags_WidthFixed, 115.0f);
            ImGui::TableSetupColumn("sig* (Mem)", ImGuiTableColumnFlags_WidthFixed, 115.0f);
            ImGui::TableSetupColumn("Prioridad", ImGuiTableColumnFlags_WidthFixed, 165.0f);
            ImGui::TableSetupColumn("ID / Hora", ImGuiTableColumnFlags_WidthFixed, 115.0f);
            ImGui::TableSetupColumn("Paciente", ImGuiTableColumnFlags_WidthFixed, 165.0f);
            ImGui::TableSetupColumn("Edad", ImGuiTableColumnFlags_WidthFixed, 55.0f);
            ImGui::TableSetupColumn("Sintomas", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Acciones", ImGuiTableColumnFlags_WidthFixed, 145.0f);
            ImGui::TableHeadersRow();

            for (int i = 0; i < cantidad; ++i)
            {
                Paciente pac;
                int prio = 0;
                colaPacientes.obtenerElemento(i, pac, prio);
                const void* ptrNodo = colaPacientes.obtenerDireccionNodo(i);
                const void* ptrSig = colaPacientes.obtenerDireccionSiguiente(i);

                ImGui::TableNextRow();
                ImGui::PushID(1000 + i);

                ImGui::TableSetColumnIndex(0);
                if (i == 0)
                    ImGui::TextColored(ImVec4(0.40f, 1.0f, 0.65f, 1.0f), "[%d] FRENTE", i);
                else if (i == cantidad - 1)
                    ImGui::TextColored(ImVec4(0.75f, 0.85f, 1.0f, 1.0f), "[%d] FIN", i);
                else
                    ImGui::Text("[%d] Nodo", i);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%p", ptrNodo);

                ImGui::TableSetColumnIndex(2);
                if (ptrSig != nullptr)
                    ImGui::Text("%p", ptrSig);
                else
                    ImGui::TextDisabled("nullptr (NULL)");

                ImGui::TableSetColumnIndex(3);
                ImGui::TextColored(obtenerColor(prio), "%s", obtenerNombrePrioridad(prio).c_str());

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("#%d (%s)", pac.id, pac.horaIngreso.c_str());

                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%s", pac.nombre.c_str());

                ImGui::TableSetColumnIndex(6);
                ImGui::Text("%d", pac.edad);

                ImGui::TableSetColumnIndex(7);
                ImGui::TextWrapped("%s", pac.sintomas.c_str());

                ImGui::TableSetColumnIndex(8);
                if (prio > 1)
                {
                    if (ImGui::SmallButton("^ Prio"))
                        indiceSubirPrio = i;
                    ImGui::SameLine();
                }
                if (ImGui::SmallButton("Eliminar"))
                    indiceEliminar = i;

                ImGui::PopID();
            }

            ImGui::EndTable();
        }
    }

    // Procesar mutaciones diferidas (fuera del bucle de renderizado)
    if (indiceSubirPrio >= 0)
    {
        Paciente pTemp;
        int prioActual = 1;
        if (colaPacientes.obtenerElemento(indiceSubirPrio, pTemp, prioActual) && prioActual > 1)
        {
            colaPacientes.actualizarPrioridad(indiceSubirPrio, prioActual - 1);
            mensaje = "Prioridad escalada para " + pTemp.nombre + " a Nivel " + std::to_string(prioActual - 1) + ".";
            mensajeError = false;
        }
    }
    else if (indiceEliminar >= 0)
    {
        Paciente pEliminado;
        if (colaPacientes.eliminarEnPosicion(indiceEliminar, pEliminado))
        {
            mensaje = "Paciente retirado de la cola: " + pEliminado.nombre + ".";
            mensajeError = false;
        }
    }

    ImGui::EndChild();

    ImGui::End();
}

// ==========================================
// CARGA DE FUENTES CON RUTAS MULTIPLES

static void cargarFuentesSistema(ImGuiIO& io)
{
    const char* rutasRegular[] = {
        "fonts/Inter-Regular.ttf",
        "../fonts/Inter-Regular.ttf",
        "../../fonts/Inter-Regular.ttf"
    };
    const char* rutasBold[] = {
        "fonts/Inter-Bold.ttf",
        "../fonts/Inter-Bold.ttf",
        "../../fonts/Inter-Bold.ttf"
    };

    for (const char* ruta : rutasRegular)
    {
        FILE* f = std::fopen(ruta, "rb");
        if (f != nullptr)
        {
            std::fclose(f);
            fuenteRegular = io.Fonts->AddFontFromFileTTF(ruta, 16.5f);
            break;
        }
    }

    for (const char* ruta : rutasBold)
    {
        FILE* f = std::fopen(ruta, "rb");
        if (f != nullptr)
        {
            std::fclose(f);
            fuenteBold = io.Fonts->AddFontFromFileTTF(ruta, 18.5f);
            break;
        }
    }

    if (fuenteRegular != nullptr)
    {
        io.FontDefault = fuenteRegular;
    }

    if (fuenteBold == nullptr)
    {
        fuenteBold = fuenteRegular;
    }
}

// ==========================================
// EJECUCION DE LA INTERFAZ

#if USAR_BACKEND_WIN32

static HGLRC g_hRC = nullptr;
static HDC g_hDC = nullptr;
static int g_Ancho = 1280;
static int g_Alto = 800;

static LRESULT WINAPI WndProcWin32(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
        case WM_SIZE:
            if (wParam != SIZE_MINIMIZED)
            {
                g_Ancho = LOWORD(lParam);
                g_Alto = HIWORD(lParam);
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU)
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

void ejecutarInterfaz()
{
    WNDCLASSEXW wc = {
        sizeof(wc), CS_OWNDC, WndProcWin32, 0L, 0L,
        GetModuleHandle(nullptr), nullptr, ::LoadCursor(nullptr, IDC_ARROW),
        nullptr, nullptr, L"MediPriorityTriajeClass", nullptr
    };
    ::RegisterClassExW(&wc);

    HWND hwnd = ::CreateWindowW(
        wc.lpszClassName,
        L"Sistema de Triaje - Colas de Prioridad (MediPriority)",
        WS_OVERLAPPEDWINDOW,
        80, 60, 1280, 800,
        nullptr, nullptr, wc.hInstance, nullptr
    );

    if (hwnd == nullptr)
    {
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return;
    }

    g_hDC = ::GetDC(hwnd);
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    const int pf = ::ChoosePixelFormat(g_hDC, &pfd);
    ::SetPixelFormat(g_hDC, pf, &pfd);
    g_hRC = ::wglCreateContext(g_hDC);
    ::wglMakeCurrent(g_hDC, g_hRC);

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    cargarFuentesSistema(io);
    configurarEstilo();

    ImGui_ImplWin32_InitForOpenGL(hwnd);
    ImGui_ImplOpenGL3_Init("#version 130");

    bool salir = false;
    while (!salir)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                salir = true;
        }
        if (salir)
            break;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        dibujarInterfaz();

        ImGui::Render();
        glViewport(0, 0, g_Ancho, g_Alto);
        glClearColor(0.035f, 0.045f, 0.075f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ::SwapBuffers(g_hDC);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    ::wglMakeCurrent(nullptr, nullptr);
    ::wglDeleteContext(g_hRC);
    ::ReleaseDC(hwnd, g_hDC);
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

#else

void ejecutarInterfaz()
{
    if (!glfwInit())
    {
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(
        1280,
        800,
        "Sistema de Triaje - Colas de Prioridad (MediPriority)",
        nullptr,
        nullptr
    );

    if (window == nullptr)
    {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    cargarFuentesSistema(io);
    configurarEstilo();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        dibujarInterfaz();

        ImGui::Render();

        int ancho, alto;
        glfwGetFramebufferSize(window, &ancho, &alto);
        glViewport(0, 0, ancho, alto);

        glClearColor(0.035f, 0.045f, 0.075f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

#endif
