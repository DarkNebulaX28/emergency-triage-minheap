#include "interfaz.h"
#include "Paciente.h"
#include "colaPrioridad.h"
#include "operaciones_colaPriori.h"
#include "validacion.h"

#include <string>
#include <cstring>
#include <cstdio>
#include <cmath>

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

Cola<Paciente> colaPacientes;
Cola<string>   colaAlertasClinicas;

ImFont* fuenteRegular = nullptr;
ImFont* fuenteBold = nullptr;

char nombre[100] = "";
char sintomas[256] = "";

int edad = 25;
int prioridadCombo = 0;
int posicionUsuario = 1;

string mensaje = "Sistema listo para admision de pacientes.";
bool mensajeError = false;

bool hayPacienteEnAtencion = false;
Paciente ultimoPacienteAtendido;
int prioridadUltimoAtendido = 1;
int totalPacientesAtendidos = 0;

int modoVistaCola = 0;
bool temaClaro = true;

struct AnimacionNodo
{
    bool inicializado = false;
    float progresoEntrada = 0.0f;
    float hoverLerp = 0.0f;
    float flashCambio = 0.0f;
};

static AnimacionNodo g_animNodos[512];
static float g_animBanner = 1.0f;
static float g_animConsultorio = 1.0f;
static float g_barrasKpiLerp[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

static AnimacionNodo& obtenerAnimNodo(int idTicket)
{
    int idx = (idTicket >= 0 && idTicket < 512) ? idTicket : (idTicket % 512);
    if (idx < 0) idx = 0;
    if (!g_animNodos[idx].inicializado)
    {
        g_animNodos[idx].inicializado = true;
        g_animNodos[idx].progresoEntrada = 0.0f;
        g_animNodos[idx].hoverLerp = 0.0f;
        g_animNodos[idx].flashCambio = 0.8f;
    }
    return g_animNodos[idx];
}

static void limpiarAnimaciones()
{
    for (int i = 0; i < 512; ++i)
    {
        g_animNodos[i].inicializado = false;
        g_animNodos[i].progresoEntrada = 0.0f;
        g_animNodos[i].hoverLerp = 0.0f;
        g_animNodos[i].flashCambio = 0.0f;
    }
}

static float maxf(float a, float b)
{
    return (a > b) ? a : b;
}

static float lerpf(float a, float b, float t)
{
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return a + (b - a) * t;
}

static float clampf(float v, float lo, float hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static float easeOutCubic(float t)
{
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    float inv = 1.0f - t;
    return 1.0f - (inv * inv * inv);
}

static const ImVec4 ACCENT_PRIMARY = ImVec4(0.102f, 0.451f, 0.910f, 1.0f);
static const ImVec4 ACCENT_DANGER  = ImVec4(0.851f, 0.188f, 0.145f, 1.0f);
static const ImVec4 ACCENT_WARNING = ImVec4(0.953f, 0.596f, 0.000f, 1.0f);
static const ImVec4 ACCENT_SUCCESS = ImVec4(0.118f, 0.557f, 0.243f, 1.0f);

ImVec4 obtenerColor(int prioridad)
{
    if (temaClaro)
    {
        switch (prioridad)
        {
            case 1: return ImVec4(0.851f, 0.188f, 0.145f, 1.0f);
            case 2: return ImVec4(0.910f, 0.443f, 0.039f, 1.0f);
            case 3: return ImVec4(0.820f, 0.540f, 0.000f, 1.0f);
            case 4: return ImVec4(0.118f, 0.557f, 0.243f, 1.0f);
            case 5: return ImVec4(0.102f, 0.451f, 0.910f, 1.0f);
            default: return ImVec4(0.373f, 0.388f, 0.408f, 1.0f);
        }
    }
    else
    {
        switch (prioridad)
        {
            case 1: return ImVec4(0.949f, 0.545f, 0.510f, 1.0f);
            case 2: return ImVec4(0.988f, 0.678f, 0.439f, 1.0f);
            case 3: return ImVec4(0.992f, 0.839f, 0.388f, 1.0f);
            case 4: return ImVec4(0.506f, 0.788f, 0.584f, 1.0f);
            case 5: return ImVec4(0.541f, 0.706f, 0.973f, 1.0f);
            default: return ImVec4(0.604f, 0.627f, 0.651f, 1.0f);
        }
    }
}

ImVec4 obtenerFondoTonalPrioridad(int prioridad)
{
    if (temaClaro)
    {
        switch (prioridad)
        {
            case 1: return ImVec4(0.992f, 0.937f, 0.933f, 1.0f);
            case 2: return ImVec4(0.996f, 0.953f, 0.910f, 1.0f);
            case 3: return ImVec4(0.996f, 0.969f, 0.886f, 1.0f);
            case 4: return ImVec4(0.910f, 0.965f, 0.922f, 1.0f);
            case 5: return ImVec4(0.918f, 0.949f, 0.996f, 1.0f);
            default: return ImVec4(0.973f, 0.976f, 0.980f, 1.0f);
        }
    }
    else
    {
        ImVec4 c = obtenerColor(prioridad);
        return ImVec4(c.x * 0.22f, c.y * 0.22f, c.z * 0.25f, 0.55f);
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

string obtenerNombreCortoPrioridad(int prioridad)
{
    switch (prioridad)
    {
        case 1: return "P1 · CRITICA";
        case 2: return "P2 · ALTA";
        case 3: return "P3 · MEDIA";
        case 4: return "P4 · BAJA";
        case 5: return "P5 · LEVE";
        default: return "P?";
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

void dibujarTitulo(const char* texto)
{
    if (fuenteBold != nullptr)
        ImGui::PushFont(fuenteBold);

    if (temaClaro)
        ImGui::TextColored(ImVec4(0.125f, 0.129f, 0.141f, 1.0f), "%s", texto);
    else
        ImGui::TextColored(ImVec4(0.910f, 0.918f, 0.929f, 1.0f), "%s", texto);

    if (fuenteBold != nullptr)
        ImGui::PopFont();
}

void configurarEstilo()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding    = 0.0f;
    style.ChildRounding     = 10.0f;
    style.FrameRounding     = 7.0f;
    style.PopupRounding     = 9.0f;
    style.ScrollbarRounding = 0.0f;
    style.ScrollbarSize     = 0.0f;
    style.GrabRounding      = 7.0f;

    style.WindowPadding = ImVec2(14, 10);
    style.FramePadding  = ImVec2(10, 5);
    style.ItemSpacing   = ImVec2(8, 6);

    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize  = 1.0f;
    style.FrameBorderSize  = 1.0f;

    if (temaClaro)
    {
        style.Colors[ImGuiCol_WindowBg]         = ImVec4(0.969f, 0.976f, 0.988f, 1.0f);
        style.Colors[ImGuiCol_ChildBg]          = ImVec4(1.000f, 1.000f, 1.000f, 1.0f);
        style.Colors[ImGuiCol_PopupBg]          = ImVec4(1.000f, 1.000f, 1.000f, 1.0f);
        style.Colors[ImGuiCol_Border]           = ImVec4(0.855f, 0.867f, 0.890f, 1.0f);
        style.Colors[ImGuiCol_Text]             = ImVec4(0.125f, 0.129f, 0.141f, 1.0f);
        style.Colors[ImGuiCol_TextDisabled]     = ImVec4(0.373f, 0.388f, 0.408f, 1.0f);
        style.Colors[ImGuiCol_FrameBg]          = ImVec4(0.945f, 0.953f, 0.965f, 1.0f);
        style.Colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.910f, 0.941f, 0.996f, 1.0f);
        style.Colors[ImGuiCol_FrameBgActive]    = ImVec4(1.000f, 1.000f, 1.000f, 1.0f);
        style.Colors[ImGuiCol_Button]           = ImVec4(0.102f, 0.451f, 0.910f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered]    = ImVec4(0.082f, 0.365f, 0.753f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive]     = ImVec4(0.067f, 0.302f, 0.635f, 1.0f);
        style.Colors[ImGuiCol_Header]           = ImVec4(0.910f, 0.941f, 0.996f, 1.0f);
        style.Colors[ImGuiCol_HeaderHovered]    = ImVec4(0.824f, 0.890f, 0.988f, 1.0f);
        style.Colors[ImGuiCol_HeaderActive]     = ImVec4(0.682f, 0.796f, 0.980f, 1.0f);
        style.Colors[ImGuiCol_Separator]        = ImVec4(0.886f, 0.894f, 0.910f, 1.0f);
        style.Colors[ImGuiCol_CheckMark]        = ImVec4(0.102f, 0.451f, 0.910f, 1.0f);
        style.Colors[ImGuiCol_TableHeaderBg]    = ImVec4(0.910f, 0.941f, 0.996f, 1.0f);
        style.Colors[ImGuiCol_TableRowBg]       = ImVec4(1.000f, 1.000f, 1.000f, 1.0f);
        style.Colors[ImGuiCol_TableRowBgAlt]    = ImVec4(0.973f, 0.976f, 0.984f, 1.0f);
    }
    else
    {
        style.Colors[ImGuiCol_WindowBg]         = ImVec4(0.118f, 0.125f, 0.141f, 1.0f);
        style.Colors[ImGuiCol_ChildBg]          = ImVec4(0.157f, 0.165f, 0.184f, 1.0f);
        style.Colors[ImGuiCol_PopupBg]          = ImVec4(0.157f, 0.165f, 0.184f, 1.0f);
        style.Colors[ImGuiCol_Border]           = ImVec4(0.235f, 0.251f, 0.275f, 1.0f);
        style.Colors[ImGuiCol_Text]             = ImVec4(0.910f, 0.918f, 0.929f, 1.0f);
        style.Colors[ImGuiCol_TextDisabled]     = ImVec4(0.604f, 0.627f, 0.651f, 1.0f);
        style.Colors[ImGuiCol_FrameBg]          = ImVec4(0.196f, 0.208f, 0.227f, 1.0f);
        style.Colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.235f, 0.251f, 0.282f, 1.0f);
        style.Colors[ImGuiCol_FrameBgActive]    = ImVec4(0.255f, 0.275f, 0.314f, 1.0f);
        style.Colors[ImGuiCol_Button]           = ImVec4(0.102f, 0.451f, 0.910f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered]    = ImVec4(0.196f, 0.525f, 0.945f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive]     = ImVec4(0.082f, 0.365f, 0.753f, 1.0f);
        style.Colors[ImGuiCol_Header]           = ImVec4(0.165f, 0.243f, 0.365f, 1.0f);
        style.Colors[ImGuiCol_HeaderHovered]    = ImVec4(0.200f, 0.298f, 0.443f, 1.0f);
        style.Colors[ImGuiCol_HeaderActive]     = ImVec4(0.235f, 0.353f, 0.525f, 1.0f);
        style.Colors[ImGuiCol_Separator]        = ImVec4(0.235f, 0.251f, 0.275f, 1.0f);
        style.Colors[ImGuiCol_CheckMark]        = ImVec4(0.541f, 0.706f, 0.973f, 1.0f);
        style.Colors[ImGuiCol_TableHeaderBg]    = ImVec4(0.188f, 0.220f, 0.275f, 1.0f);
        style.Colors[ImGuiCol_TableRowBg]       = ImVec4(0.157f, 0.165f, 0.184f, 1.0f);
        style.Colors[ImGuiCol_TableRowBgAlt]    = ImVec4(0.180f, 0.188f, 0.208f, 1.0f);
    }
}

static void dibujarIconoClinicoAnimado(float pulso)
{
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    float size = 24.0f;
    ImVec2 center(p.x + size * 0.5f, p.y + size * 0.5f);

    float radioHalo = (size * 0.5f) + 3.0f * pulso;
    dl->AddCircleFilled(center, radioHalo, IM_COL32(26, 115, 232, (int)(55.0f * (1.0f - pulso))));
    dl->AddRectFilled(p, ImVec2(p.x + size, p.y + size), IM_COL32(26, 115, 232, 255), 6.0f);

    float brazoLargo = 11.0f;
    float brazoGrosor = 3.2f;
    dl->AddRectFilled(
        ImVec2(center.x - brazoLargo * 0.5f, center.y - brazoGrosor * 0.5f),
        ImVec2(center.x + brazoLargo * 0.5f, center.y + brazoGrosor * 0.5f),
        IM_COL32(255, 255, 255, 255),
        1.5f
    );
    dl->AddRectFilled(
        ImVec2(center.x - brazoGrosor * 0.5f, center.y - brazoLargo * 0.5f),
        ImVec2(center.x + brazoGrosor * 0.5f, center.y + brazoLargo * 0.5f),
        IM_COL32(255, 255, 255, 255),
        1.5f
    );

    ImGui::Dummy(ImVec2(size + 2.0f, size));
}

static void dibujarIndicadorPulso(ImVec4 color, float fase)
{
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 center(pos.x + 6.0f, pos.y + 8.0f);

    ImU32 colSolido = ImGui::ColorConvertFloat4ToU32(color);
    ImVec4 colOnda = color;
    colOnda.w = (1.0f - fase) * 0.55f;

    dl->AddCircleFilled(center, 4.0f + fase * 5.0f, ImGui::ColorConvertFloat4ToU32(colOnda));
    dl->AddCircleFilled(center, 4.0f, colSolido);

    ImGui::Dummy(ImVec2(14.0f, 14.0f));
}

static void dibujarConectorPunteroAnimado(float anchoConector, float altoTarjeta, bool esUltimoNodo, float tiempoGlobal, ImVec4 colorOrigen)
{
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    float yCentro = p.y + altoTarjeta * 0.50f;

    ImVec2 inicio(p.x + 2.0f, yCentro);
    ImVec2 fin(p.x + anchoConector - (esUltimoNodo ? 12.0f : 6.0f), yCentro);

    ImU32 colorLinea = temaClaro ? IM_COL32(140, 155, 180, 220) : IM_COL32(110, 130, 165, 220);
    ImU32 colorPunta = ImGui::ColorConvertFloat4ToU32(ACCENT_PRIMARY);

    if (anchoConector >= 28.0f)
    {
        dl->AddText(
            ImVec2(p.x + 3.0f, yCentro - 18.0f),
            temaClaro ? IM_COL32(95, 99, 104, 255) : IM_COL32(160, 168, 180, 255),
            esUltimoNodo ? "NULL" : "sig*"
        );
    }

    dl->AddLine(inicio, fin, colorLinea, 2.2f);

    dl->AddTriangleFilled(
        ImVec2(fin.x + 5.0f, fin.y),
        ImVec2(fin.x - 3.5f, fin.y - 4.5f),
        ImVec2(fin.x - 3.5f, fin.y + 4.5f),
        colorPunta
    );

    if (!esUltimoNodo)
    {
        float t = std::fmod(tiempoGlobal * 0.95f, 1.0f);
        float px = lerpf(inicio.x, fin.x, easeOutCubic(t));
        ImU32 colParticula = ImGui::ColorConvertFloat4ToU32(colorOrigen);
        dl->AddCircleFilled(ImVec2(px, yCentro), 3.5f, colParticula);
    }
    else
    {
        dl->AddRect(
            ImVec2(fin.x + 6.0f, yCentro - 5.5f),
            ImVec2(fin.x + 13.0f, yCentro + 5.5f),
            IM_COL32(217, 48, 37, 200),
            2.0f, 0, 1.6f
        );
    }

    ImGui::Dummy(ImVec2(anchoConector, altoTarjeta));
}

void agregarPaciente()
{
    string nombreTexto = nombre;
    string sintomasTexto = sintomas;

    int prioridad = prioridadCombo + 1;
    string error;

    if (!validarPaciente(
            nombreTexto,
            edad,
            sintomasTexto,
            prioridad,
            error))
    {
        mensaje = error;
        mensajeError = true;
        g_animBanner = 0.0f;
        return;
    }

    int posEstimada = colaPacientes.obtenerPosicionInsercion(prioridad) + 1;

    Paciente nuevoPaciente(
        nombreTexto,
        edad,
        sintomasTexto
    );

    AnimacionNodo& anim = obtenerAnimNodo(nuevoPaciente.getId());
    anim.progresoEntrada = 0.0f;
    anim.hoverLerp = 0.0f;
    anim.flashCambio = 1.0f;

    colaPacientes.encolar(nuevoPaciente, prioridad);
    colaAlertasClinicas.encolar(
        "Ingreso [#" + std::to_string(nuevoPaciente.getId()) + "] " + nuevoPaciente.getNombre() + " - " + nuevoPaciente.getSintomas(),
        prioridad
    );

    mensaje = "Encolado #" + std::to_string(nuevoPaciente.getId()) + " (" + nuevoPaciente.getNombre() +
              ") en turno #" + std::to_string(posEstimada) + ".";
    mensajeError = false;
    g_animBanner = 0.0f;

    nombre[0] = '\0';
    sintomas[0] = '\0';
    edad = 25;
    prioridadCombo = 0;
}

void atenderPaciente()
{
    if (colaPacientes.verificar_vacio())
    {
        mensaje = "Caso limite: No se puede desencolar porque la cola esta vacia (frente = nullptr).";
        mensajeError = true;
        g_animBanner = 0.0f;
        return;
    }

    Paciente pacienteAtendido;
    int prioridadAtendida = 1;

    if (colaPacientes.desencolarConPrioridad(pacienteAtendido, prioridadAtendida))
    {
        string alertaDespachada;
        colaAlertasClinicas.desencolar(alertaDespachada);

        ultimoPacienteAtendido = pacienteAtendido;
        prioridadUltimoAtendido = prioridadAtendida;
        hayPacienteEnAtencion = true;
        ++totalPacientesAtendidos;
        g_animConsultorio = 0.0f;
        g_animBanner = 0.0f;

        mensaje = "En atencion: [#" + std::to_string(pacienteAtendido.getId()) + "] " +
                  pacienteAtendido.getNombre() + " (" + obtenerNombreCortoPrioridad(prioridadAtendida) + ")";
        mensajeError = false;
    }
}

void cargarCasoUnElemento()
{
    colaPacientes.limpiar();
    colaAlertasClinicas.limpiar();
    limpiarAnimaciones();

    Paciente unico("Mateo Alvarez", 49, "Insuficiencia respiratoria aguda (Caso limite: 1 nodo)");
    AnimacionNodo& anim = obtenerAnimNodo(unico.getId());
    anim.progresoEntrada = 0.0f;
    anim.flashCambio = 1.0f;

    colaPacientes.encolar(unico, 1);
    colaAlertasClinicas.encolar("Caso limite 1 nodo: frente == fin y sig == nullptr", 1);

    mensaje = "Caso limite (1 elemento): Observe que Frente y Fin apuntan al mismo nodo y sig -> NULL.";
    mensajeError = false;
    g_animBanner = 0.0f;
}

void cargarPacientesDemo()
{
    colaPacientes.limpiar();
    colaAlertasClinicas.limpiar();
    limpiarAnimaciones();

    struct DatosDemo { const char* n; int e; const char* s; int p; };
    const DatosDemo lote[6] = {
        { "Carlos Mendoza", 58, "Dolor toracico agudo e irradiacion", 2 },
        { "Lucia Salazar",  24, "Esguince de tobillo grado 1",        4 },
        { "Roberto Vega",   67, "Paro cardiorrespiratorio critico",   1 },
        { "Ana Gomez",      34, "Crisis hipertensiva y disnea",       2 },
        { "Miguel Torres",  19, "Fiebre 38.5C y dolor abdominal",     3 },
        { "Elena Paredes",  45, "Trauma craneoencefalico severo",     1 }
    };

    for (int i = 0; i < 6; ++i)
    {
        Paciente p(lote[i].n, lote[i].e, lote[i].s);
        AnimacionNodo& anim = obtenerAnimNodo(p.getId());
        anim.progresoEntrada = -0.10f * (float)i;
        anim.hoverLerp = 0.0f;
        anim.flashCambio = 0.6f;

        colaPacientes.encolar(p, lote[i].p);
        colaAlertasClinicas.encolar(
            "Alerta [#" + std::to_string(p.getId()) + "] " + p.getNombre() + ": " + p.getSintomas(),
            lote[i].p
        );
    }

    mensaje = "6 pacientes demo cargados en Cola<Paciente> y Cola<string> (ordenados por prioridad + FIFO).";
    mensajeError = false;
    g_animBanner = 0.0f;
}

void dibujarTarjetaAdaptable(
    Paciente paciente,
    int prioridad,
    int indice,
    int totalEnCola,
    float anchoTarjeta,
    float altoTarjeta,
    int& indiceSubirPrio,
    int& indiceEliminar,
    float dt,
    float pulsoGlobal)
{
    AnimacionNodo& anim = obtenerAnimNodo(paciente.getId());
    anim.progresoEntrada += dt * 3.2f;
    if (anim.progresoEntrada > 1.0f) anim.progresoEntrada = 1.0f;
    if (anim.flashCambio > 0.0f)
    {
        anim.flashCambio -= dt * 1.8f;
        if (anim.flashCambio < 0.0f) anim.flashCambio = 0.0f;
    }

    float tVisible = anim.progresoEntrada < 0.0f ? 0.0f : easeOutCubic(anim.progresoEntrada);
    float offsetVertical = (1.0f - tVisible) * 12.0f;

    ImVec4 color = obtenerColor(prioridad);
    ImVec4 fondo = obtenerFondoTonalPrioridad(prioridad);

    if (anim.flashCambio > 0.0f)
    {
        fondo.x = lerpf(fondo.x, color.x, anim.flashCambio * 0.25f);
        fondo.y = lerpf(fondo.y, color.y, anim.flashCambio * 0.25f);
        fondo.z = lerpf(fondo.z, color.z, anim.flashCambio * 0.25f);
    }

    const void* dirActual = colaPacientes.obtenerDireccionNodo(indice);

    ImGui::PushID(indice);

    ImVec2 cursorOrig = ImGui::GetCursorPos();
    float elevacionHover = anim.hoverLerp * 3.0f;
    ImGui::SetCursorPosY(cursorOrig.y + offsetVertical - elevacionHover);

    ImDrawList* parentDl = ImGui::GetWindowDrawList();
    ImVec2 cardScreenPos = ImGui::GetCursorScreenPos();
    float sombraAlpha = (temaClaro ? 0.09f : 0.28f) * tVisible + (anim.hoverLerp * 0.07f);
    parentDl->AddRectFilled(
        ImVec2(cardScreenPos.x + 2.0f, cardScreenPos.y + 4.0f + elevacionHover),
        ImVec2(cardScreenPos.x + anchoTarjeta + 2.0f, cardScreenPos.y + altoTarjeta + 4.0f + elevacionHover),
        IM_COL32(15, 23, 42, (int)(sombraAlpha * 255.0f)),
        10.0f
    );

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, (tVisible < 0.05f ? 0.05f : tVisible));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 7.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 3.0f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, fondo);
    ImGui::PushStyleColor(ImGuiCol_Border, color);

    float grosorBorde = (indice == 0 ? 2.4f : 1.3f) + (anim.hoverLerp * 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, grosorBorde);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);

    ImGuiWindowFlags cardFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::BeginChild(
        "TarjetaPaciente",
        ImVec2(anchoTarjeta, altoTarjeta),
        true,
        cardFlags
    );

    bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
    anim.hoverLerp = lerpf(anim.hoverLerp, hovered ? 1.0f : 0.0f, dt * 12.0f);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 pMin = ImGui::GetWindowPos();
    ImVec2 pMax = ImVec2(pMin.x + ImGui::GetWindowWidth(), pMin.y + 4.0f);
    dl->AddRectFilled(pMin, pMax, ImGui::ColorConvertFloat4ToU32(color), 10.0f);

    ImVec4 colorBadgeFrente = temaClaro ? ACCENT_SUCCESS : ImVec4(0.506f, 0.788f, 0.584f, 1.0f);
    ImVec4 colorBadgeFin    = temaClaro ? ACCENT_PRIMARY : ImVec4(0.541f, 0.706f, 0.973f, 1.0f);

    if (prioridad == 1)
    {
        dibujarIndicadorPulso(color, pulsoGlobal);
        ImGui::SameLine();
    }

    if (indice == 0 && totalEnCola == 1)
        ImGui::TextColored(colorBadgeFrente, "[FRENTE = FIN #1]");
    else if (indice == 0)
        ImGui::TextColored(colorBadgeFrente, "[FRENTE #%d]", indice + 1);
    else if (indice == totalEnCola - 1)
        ImGui::TextColored(colorBadgeFin, "[FIN #%d]", indice + 1);
    else
        ImGui::TextDisabled("[Pos #%d]", indice + 1);

    if (fuenteBold != nullptr)
        ImGui::PushFont(fuenteBold);

    ImGui::TextColored(
        color,
        "%s",
        (anchoTarjeta < 190.0f ? obtenerNombreCortoPrioridad(prioridad) : obtenerNombrePrioridad(prioridad)).c_str()
    );

    if (fuenteBold != nullptr)
        ImGui::PopFont();

    ImGui::Separator();

    ImGui::Text("#%d · %da · %s", paciente.getId(), paciente.getEdad(), paciente.getHoraIngreso().c_str());
    ImGui::TextWrapped("%s", paciente.getNombre().c_str());
    ImGui::TextDisabled("%s", paciente.getSintomas().c_str());

    float posYBotones = altoTarjeta - 44.0f;
    if (ImGui::GetCursorPosY() < posYBotones)
    {
        ImGui::SetCursorPosY(posYBotones);
    }

    ImGui::TextDisabled("Mem: %p", dirActual);

    if (prioridad > 1)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(color.x * 0.85f, color.y * 0.85f, color.z * 0.85f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        if (ImGui::SmallButton(anchoTarjeta < 175.0f ? "^ Prio" : "^ Urgencia"))
        {
            indiceSubirPrio = indice;
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
    }

    ImGui::PushStyleColor(ImGuiCol_Button, temaClaro ? ImVec4(0.91f, 0.92f, 0.94f, 1.0f) : ImVec4(0.25f, 0.26f, 0.28f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ACCENT_DANGER);
    if (ImGui::SmallButton("Retirar"))
    {
        indiceEliminar = indice;
    }
    ImGui::PopStyleColor(2);

    ImGui::EndChild();

    ImGui::PopStyleVar(5);
    ImGui::PopStyleColor(2);

    ImGui::PopID();
}

void dibujarVistaPrevia(float dt, float pulsoGlobal)
{
    ImGuiWindowFlags noScrollFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    float anchoDisp = ImGui::GetContentRegionAvail().x;
    float mitadAncho = (anchoDisp - 10.0f) * 0.50f;

    ImGui::BeginChild("SubPanelPrevia", ImVec2(mitadAncho, 0), false, noScrollFlags);

    dibujarTitulo("VISTA PREVIA DE ADMISION");

    int prioridad = prioridadCombo + 1;
    int posInsercion = colaPacientes.obtenerPosicionInsercion(prioridad);
    int totalActual = colaPacientes.mostrar_tamanio();

    ImVec4 color = obtenerColor(prioridad);
    ImVec4 fondo = obtenerFondoTonalPrioridad(prioridad);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, fondo);
    ImGui::PushStyleColor(ImGuiCol_Border, color);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);

    ImGui::BeginChild("VistaPreviaTarjeta", ImVec2(0, 0), true, noScrollFlags);

    if (fuenteBold != nullptr)
        ImGui::PushFont(fuenteBold);

    ImGui::TextColored(color, "%s", obtenerNombrePrioridad(prioridad).c_str());

    if (fuenteBold != nullptr)
        ImGui::PopFont();

    ImGui::TextDisabled("%s", obtenerTiempoObjetivo(prioridad));
    ImGui::Separator();

    ImGui::TextWrapped("Paciente: %s (%d anios)", nombre[0] ? nombre : "(Escriba en el formulario)", edad);
    ImGui::TextWrapped("Sintomas: %s", sintomas[0] ? sintomas : "(Sin especificar)");

    ImGui::Separator();

    ImVec4 colorInfo = temaClaro ? ACCENT_PRIMARY : ImVec4(0.541f, 0.706f, 0.973f, 1.0f);
    ImGui::TextColored(
        colorInfo,
        "Posicion estimada: Turno #%d (de %d)",
        posInsercion + 1,
        totalActual + 1
    );

    if (posInsercion == 0)
        ImGui::TextDisabled("-> Se insertara al FRENTE de la cola.");
    else if (posInsercion == totalActual)
        ImGui::TextDisabled("-> Se insertara al FIN de la cola (FIFO).");
    else
        ImGui::TextDisabled("-> Se insertara tras nodos con prioridad <= %d.", prioridad);

    ImGui::EndChild();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("SubPanelConsultorio", ImVec2(0, 0), false, noScrollFlags);

    dibujarTitulo("BOX MEDICO EN ATENCION");

    if (g_animConsultorio < 1.0f)
    {
        g_animConsultorio += dt * 3.0f;
        if (g_animConsultorio > 1.0f) g_animConsultorio = 1.0f;
    }

    float fadeBox = easeOutCubic(g_animConsultorio);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, fadeBox < 0.1f ? 0.1f : fadeBox);
    ImGui::BeginChild("TarjetaConsultorio", ImVec2(0, 0), true, noScrollFlags);

    if (!hayPacienteEnAtencion)
    {
        ImGui::TextDisabled("Consultorio disponible.");
        ImGui::TextWrapped(
            "Pulse 'ATENDER PACIENTE' para desencolar O(1) el nodo del FRENTE y asignarlo al box medico."
        );
    }
    else
    {
        ImVec4 colorAtendido = obtenerColor(prioridadUltimoAtendido);
        ImVec4 colorActivo = temaClaro ? ACCENT_SUCCESS : ImVec4(0.506f, 0.788f, 0.584f, 1.0f);

        dibujarIndicadorPulso(colorActivo, pulsoGlobal);
        ImGui::SameLine();
        ImGui::TextColored(colorActivo, "EN CONSULTA ACTIVA:");

        if (fuenteBold != nullptr)
            ImGui::PushFont(fuenteBold);

        ImGui::TextColored(colorAtendido, "%s", obtenerNombrePrioridad(prioridadUltimoAtendido).c_str());

        if (fuenteBold != nullptr)
            ImGui::PopFont();

        ImGui::Separator();
        ImGui::Text("Ticket: #%d | Hora: %s | Edad: %d", ultimoPacienteAtendido.getId(), ultimoPacienteAtendido.getHoraIngreso().c_str(), ultimoPacienteAtendido.getEdad());
        ImGui::TextWrapped("Paciente: %s", ultimoPacienteAtendido.getNombre().c_str());
        ImGui::TextWrapped("Cuadro: %s", ultimoPacienteAtendido.getSintomas().c_str());
    }

    ImGui::Separator();

    Paciente sigPaciente;
    int sigPrio = 0;
    if (colaPacientes.consultarFrenteConPrioridad(sigPaciente, sigPrio))
    {
        ImGui::TextColored(
            obtenerColor(sigPrio),
            "Proximo en Frente: #%d %s (P%d)",
            sigPaciente.getId(),
            sigPaciente.getNombre().c_str(),
            sigPrio
        );
    }
    else
    {
        ImGui::TextDisabled("Proximo en Frente: Ninguno (Cola vacia)");
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::EndChild();
}

static void dibujarBarraDistribucionPrioridades(float dt)
{
    int total = colaPacientes.mostrar_tamanio();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float anchoTotal = ImGui::GetContentRegionAvail().x;
    float altoBarra = 6.0f;

    dl->AddRectFilled(
        pos,
        ImVec2(pos.x + anchoTotal, pos.y + altoBarra),
        temaClaro ? IM_COL32(226, 232, 240, 255) : IM_COL32(45, 50, 60, 255),
        3.0f
    );

    float xCursor = pos.x;
    for (int p = 1; p <= 5; ++p)
    {
        int cuenta = colaPacientes.contarPorPrioridad(p);
        float objetivoFrac = (total > 0) ? ((float)cuenta / (float)total) : 0.0f;
        g_barrasKpiLerp[p - 1] = lerpf(g_barrasKpiLerp[p - 1], objetivoFrac, dt * 8.0f);

        float anchoSeg = anchoTotal * g_barrasKpiLerp[p - 1];
        if (anchoSeg > 1.0f)
        {
            ImU32 colSeg = ImGui::ColorConvertFloat4ToU32(obtenerColor(p));
            dl->AddRectFilled(
                ImVec2(xCursor, pos.y),
                ImVec2(xCursor + anchoSeg, pos.y + altoBarra),
                colSeg,
                3.0f
            );
            xCursor += anchoSeg;
        }
    }

    ImGui::Dummy(ImVec2(anchoTotal, altoBarra + 2.0f));
}

void dibujarInterfaz()
{
    ImGuiIO& io = ImGui::GetIO();
    float dt = io.DeltaTime > 0.0f && io.DeltaTime < 0.2f ? io.DeltaTime : (1.0f / 60.0f);
    float tiempoGlobal = (float)ImGui::GetTime();
    float pulsoGlobal = std::fmod(tiempoGlobal * 0.85f, 1.0f);

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Sistema de Triaje", nullptr, flags);

    dibujarIconoClinicoAnimado(pulsoGlobal);
    ImGui::SameLine();

    if (fuenteBold != nullptr)
        ImGui::PushFont(fuenteBold);

    ImGui::TextColored(ACCENT_PRIMARY, "MEDIPRIORITY");
    ImGui::SameLine(0.0f, 8.0f);
    ImGui::TextColored(
        temaClaro ? ImVec4(0.125f, 0.129f, 0.141f, 1.0f) : ImVec4(0.910f, 0.918f, 0.929f, 1.0f),
        "|  Sistema de Triaje (Grupo 2 - Colas de Prioridad)"
    );

    if (fuenteBold != nullptr)
        ImGui::PopFont();

    float anchoAccionesTop = 535.0f;
    if (ImGui::GetWindowWidth() > 960.0f)
    {
        ImGui::SameLine(ImGui::GetWindowWidth() - anchoAccionesTop);
    }

    ImGui::PushStyleColor(ImGuiCol_Button, temaClaro ? ImVec4(0.910f, 0.941f, 0.996f, 1.0f) : ImVec4(0.235f, 0.251f, 0.263f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, temaClaro ? ImVec4(0.824f, 0.890f, 0.988f, 1.0f) : ImVec4(0.300f, 0.320f, 0.340f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, temaClaro ? ACCENT_PRIMARY : ImVec4(0.910f, 0.918f, 0.929f, 1.0f));
    if (ImGui::Button(temaClaro ? "Tema: Claro" : "Tema: Oscuro", ImVec2(110, 26)))
    {
        temaClaro = !temaClaro;
        configurarEstilo();
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ACCENT_WARNING);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    if (ImGui::Button("Caso: 1 Nodo", ImVec2(112, 26)))
    {
        cargarCasoUnElemento();
    }
    ImGui::PopStyleColor(2);

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    if (ImGui::Button("Cargar Demo (6)", ImVec2(138, 26)))
    {
        cargarPacientesDemo();
    }
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ACCENT_DANGER);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.15f, 0.12f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    if (ImGui::Button("Caso: Vacia", ImVec2(112, 26)))
    {
        colaPacientes.limpiar();
        colaAlertasClinicas.limpiar();
        limpiarAnimaciones();
        mensaje = "Caso limite (Cola vacia): frente = nullptr, fin = nullptr, cantidad = 0.";
        mensajeError = false;
        g_animBanner = 0.0f;
    }
    ImGui::PopStyleColor(3);

    for (int p = 1; p <= 5; ++p)
    {
        int cuentaNivel = colaPacientes.contarPorPrioridad(p);
        ImGui::TextColored(obtenerColor(p), "[N%d: %d]", p, cuentaNivel);
        ImGui::SameLine();
    }
    ImGui::TextDisabled(
        "| En Espera: %d | Atendidos: %d | Ops Punteros: %d | Template Cola<string>: %d",
        colaPacientes.mostrar_tamanio(),
        totalPacientesAtendidos,
        colaPacientes.obtenerTotalOperaciones(),
        colaAlertasClinicas.mostrar_tamanio()
    );

    dibujarBarraDistribucionPrioridades(dt);

    if (!temaClaro){
        ImGui::GetStyle().Colors[ImGuiCol_Text] =
            ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    
        ImGui::GetStyle().Colors[ImGuiCol_TextDisabled] =
            ImVec4(0.80f, 0.82f, 0.85f, 1.0f);
    }

    ImGuiWindowFlags childNoScroll = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    float anchoDisponible = ImGui::GetContentRegionAvail().x;
    float altoDisponible  = ImGui::GetContentRegionAvail().y;

    float anchoFormulario = anchoDisponible * 0.44f;
    float altoSuperior    = maxf(255.0f, altoDisponible * 0.45f);
    float altoInferior    = maxf(180.0f, altoDisponible - altoSuperior - 8.0f);

    ImGui::BeginChild(
        "PanelFormulario",
        ImVec2(anchoFormulario, altoSuperior),
        true,
        childNoScroll
    );

    dibujarTitulo("REGISTRAR PACIENTE (INSERCION POR PRIORIDAD)");

    float anchoFilaForm = ImGui::GetContentRegionAvail().x;
    float anchoColNombre = (anchoFilaForm - 8.0f) * 0.72f;
    float anchoColEdad   = (anchoFilaForm - 8.0f) * 0.28f;

    ImGui::BeginGroup();
    ImGui::Text("Nombre completo (solo letras)");
    ImGui::SetNextItemWidth(anchoColNombre);
    ImGui::InputText("##Nombre", nombre, sizeof(nombre));
    ImGui::EndGroup();

    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::Text("Edad (1-120)");
    ImGui::SetNextItemWidth(anchoColEdad);
    if (ImGui::InputInt("##Edad", &edad))
    {
        if (edad < 0) edad = 0;
        if (edad > 120) edad = 120;
    }
    ImGui::EndGroup();

    ImGui::Text("Sintomas / Motivo clinico");
    float altoSintomas = clampf(altoSuperior * 0.16f, 34.0f, 52.0f);
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextMultiline(
        "##Sintomas",
        sintomas,
        sizeof(sintomas),
        ImVec2(-1, altoSintomas)
    );

    ImGui::Text("Nivel de prioridad (Escala Manchester)");

    const char* prioridades[] = {
        "1 - CRITICA (Resucitacion - Inmediato)",
        "2 - ALTA (Emergencia - <= 10 min)",
        "3 - MEDIA (Urgencia - <= 60 min)",
        "4 - BAJA (Menor - <= 120 min)",
        "5 - LEVE (No Urgente - <= 240 min)"
    };

    ImGui::SetNextItemWidth(-1);
    ImGui::Combo("##Prioridad", &prioridadCombo, prioridades, IM_ARRAYSIZE(prioridades));

    const char* etiquetasCortas[] = { "1-Critica", "2-Alta", "3-Media", "4-Baja", "5-Leve" };
    float anchoBtnPrio = (ImGui::GetContentRegionAvail().x - 32.0f) / 5.0f;
    for (int i = 0; i < 5; ++i)
    {
        ImVec4 colPrio = obtenerColor(i + 1);
        ImVec4 fondoTonal = obtenerFondoTonalPrioridad(i + 1);
        bool seleccionado = (prioridadCombo == i);

        ImGui::PushStyleColor(ImGuiCol_Button, seleccionado ? colPrio : fondoTonal);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colPrio);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, colPrio);
        ImGui::PushStyleColor(ImGuiCol_Text, seleccionado ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : colPrio);
        if (ImGui::Button(etiquetasCortas[i], ImVec2(anchoBtnPrio, 23)))
        {
            prioridadCombo = i;
        }
        ImGui::PopStyleColor(4);
        if (i < 4) ImGui::SameLine();
    }

    if (fuenteBold != nullptr)
        ImGui::PushFont(fuenteBold);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    if (ImGui::Button("REGISTRAR PACIENTE EN COLA", ImVec2(-1, 32)))
    {
        agregarPaciente();
    }
    ImGui::PopStyleColor();

    if (fuenteBold != nullptr)
        ImGui::PopFont();

    if (g_animBanner < 1.0f)
    {
        g_animBanner += dt * 4.0f;
        if (g_animBanner > 1.0f) g_animBanner = 1.0f;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, easeOutCubic(g_animBanner));
    if (mensajeError)
    {
        ImGui::TextColored(ACCENT_DANGER, "[!] %s", mensaje.c_str());
    }
    else
    {
        ImGui::TextColored(
            temaClaro ? ACCENT_SUCCESS : ImVec4(0.506f, 0.788f, 0.584f, 1.0f),
            "[OK] %s",
            mensaje.c_str()
        );
    }
    ImGui::PopStyleVar();

    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild(
        "PanelVistaPrevia",
        ImVec2(0, altoSuperior),
        true,
        childNoScroll
    );

    dibujarVistaPrevia(dt, pulsoGlobal);

    ImGui::EndChild();

    ImGui::BeginChild(
        "PanelCola",
        ImVec2(0, altoInferior),
        true,
        childNoScroll
    );

    int cantidad = colaPacientes.mostrar_tamanio();

    dibujarTitulo("COLA DE PRIORIDAD");
    ImGui::SameLine();

    if (ImGui::RadioButton("Nodos (Frente->Fin)", modoVistaCola == 0))
        modoVistaCola = 0;
    ImGui::SameLine();
    if (ImGui::RadioButton("Punteros RAM (Nodo*)", modoVistaCola == 1))
        modoVistaCola = 1;
    ImGui::SameLine();
    if (ImGui::RadioButton("Template Cola<string>", modoVistaCola == 2))
        modoVistaCola = 2;

    ImGui::SameLine();
    ImGui::TextDisabled("| Pos:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(72.0f);
    if (ImGui::InputInt("##PosUsuario", &posicionUsuario, 1, 1))
    {
        if (posicionUsuario < 1) posicionUsuario = 1;
    }
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    if (ImGui::SmallButton("Consultar Pos"))
    {
        string errPos;
        if (!validarPosicionCola(posicionUsuario, cantidad, errPos))
        {
            mensaje = errPos;
            mensajeError = true;
        }
        else
        {
            Paciente pCons;
            int prioCons = 0;
            colaPacientes.obtenerElemento(posicionUsuario - 1, pCons, prioCons);
            mensaje = "Pos #" + std::to_string(posicionUsuario) + ": [#" + std::to_string(pCons.getId()) + "] " +
                      pCons.getNombre() + " (" + obtenerNombreCortoPrioridad(prioCons) + ")";
            mensajeError = false;
        }
        g_animBanner = 0.0f;
    }
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ACCENT_DANGER);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    if (ImGui::SmallButton("Eliminar Pos"))
    {
        string errPos;
        if (!validarPosicionCola(posicionUsuario, cantidad, errPos))
        {
            mensaje = errPos;
            mensajeError = true;
        }
        else
        {
            Paciente pElim;
            colaPacientes.eliminarEnPosicion(posicionUsuario - 1, pElim);
            string alertaElim;
            colaAlertasClinicas.eliminarEnPosicion(posicionUsuario - 1, alertaElim);
            mensaje = "Eliminado en posicion #" + std::to_string(posicionUsuario) + ": " + pElim.getNombre() + ".";
            mensajeError = false;
        }
        g_animBanner = 0.0f;
    }
    ImGui::PopStyleColor(2);

    float anchoBotonAtender = 178.0f;
    if (ImGui::GetWindowWidth() > 960.0f)
    {
        ImGui::SameLine(ImGui::GetWindowWidth() - anchoBotonAtender - 16.0f);
    }

    if (fuenteBold != nullptr)
        ImGui::PushFont(fuenteBold);

    ImGui::PushStyleColor(ImGuiCol_Button, ACCENT_SUCCESS);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.09f, 0.46f, 0.20f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.07f, 0.38f, 0.16f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    if (ImGui::Button("ATENDER PACIENTE", ImVec2(anchoBotonAtender, 28)))
    {
        atenderPaciente();
    }

    ImGui::PopStyleColor(4);

    if (fuenteBold != nullptr)
        ImGui::PopFont();

    ImGui::Separator();

    int indiceSubirPrio = -1;
    int indiceEliminar = -1;

    float anchoAreaCola = ImGui::GetContentRegionAvail().x;
    float altoAreaCola  = ImGui::GetContentRegionAvail().y;

    if (cantidad == 0)
    {
        ImGui::Dummy(ImVec2(0, altoAreaCola * 0.28f));
        ImGui::TextDisabled(
            "   Caso Limite (Estructura Vacia): frente = nullptr, fin = nullptr, cantidad = 0. Pulse 'Caso: 1 Nodo' o 'Cargar Demo (6)'."
        );
    }
    else if (modoVistaCola == 0)
    {
        int filas = (cantidad > 6) ? 2 : 1;
        int colsPorFila = (filas == 1) ? cantidad : ((cantidad + 1) / 2);
        if (colsPorFila < 1) colsPorFila = 1;

        float anchoConector = (colsPorFila >= 6) ? 30.0f : 42.0f;
        float espacioEntreItems = 4.0f;
        float totalConectoresAncho = colsPorFila * (anchoConector + espacioEntreItems * 2.0f);
        float anchoTarjetaCalc = (anchoAreaCola - totalConectoresAncho) / (float)colsPorFila;
        float anchoTarjeta = clampf(anchoTarjetaCalc, 135.0f, 255.0f);
        float altoTarjeta  = (filas == 1)
            ? clampf(altoAreaCola - 8.0f, 130.0f, 210.0f)
            : clampf((altoAreaCola - 10.0f) * 0.48f, 105.0f, 145.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(espacioEntreItems, 6.0f));

        for (int i = 0; i < cantidad; i++)
        {
            Paciente pacienteActual;
            int prioridad = 0;
            colaPacientes.obtenerElemento(i, pacienteActual, prioridad);

            dibujarTarjetaAdaptable(
                pacienteActual,
                prioridad,
                i,
                cantidad,
                anchoTarjeta,
                altoTarjeta,
                indiceSubirPrio,
                indiceEliminar,
                dt,
                pulsoGlobal
            );

            ImGui::SameLine();

            dibujarConectorPunteroAnimado(
                anchoConector,
                altoTarjeta,
                i == cantidad - 1,
                tiempoGlobal + i * 0.2f,
                obtenerColor(prioridad)
            );

            bool finDeFila = ((i + 1) % colsPorFila == 0) && (i < cantidad - 1);
            if (!finDeFila && i < cantidad - 1)
            {
                ImGui::SameLine();
            }
        }

        ImGui::PopStyleVar();
    }
    else if (modoVistaCola == 1)
    {
        ImGuiTableFlags tableFlags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_SizingStretchProp;

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(6.0f, 3.0f));
        if (ImGui::BeginTable("TablaNodosMemoria", 9, tableFlags, ImVec2(0, altoAreaCola - 4.0f)))
        {
            ImGui::TableSetupColumn("Posicion", ImGuiTableColumnFlags_WidthFixed, 95.0f);
            ImGui::TableSetupColumn("Nodo* (Mem)", ImGuiTableColumnFlags_WidthFixed, 105.0f);
            ImGui::TableSetupColumn("sig* (Mem)", ImGuiTableColumnFlags_WidthFixed, 105.0f);
            ImGui::TableSetupColumn("Prioridad", ImGuiTableColumnFlags_WidthFixed, 155.0f);
            ImGui::TableSetupColumn("ID / Hora", ImGuiTableColumnFlags_WidthFixed, 105.0f);
            ImGui::TableSetupColumn("Paciente", ImGuiTableColumnFlags_WidthFixed, 150.0f);
            ImGui::TableSetupColumn("Edad", ImGuiTableColumnFlags_WidthFixed, 45.0f);
            ImGui::TableSetupColumn("Sintomas", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Acciones", ImGuiTableColumnFlags_WidthFixed, 135.0f);
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
                if (i == 0 && cantidad == 1)
                    ImGui::TextColored(ACCENT_SUCCESS, "[1] FRENTE=FIN");
                else if (i == 0)
                    ImGui::TextColored(ACCENT_SUCCESS, "[1] FRENTE");
                else if (i == cantidad - 1)
                    ImGui::TextColored(ACCENT_PRIMARY, "[%d] FIN", i + 1);
                else
                    ImGui::Text("[%d] Nodo", i + 1);

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
                ImGui::Text("#%d (%s)", pac.getId(), pac.getHoraIngreso().c_str());

                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%s", pac.getNombre().c_str());

                ImGui::TableSetColumnIndex(6);
                ImGui::Text("%d", pac.getEdad());

                ImGui::TableSetColumnIndex(7);
                ImGui::Text("%s", pac.getSintomas().c_str());

                ImGui::TableSetColumnIndex(8);
                if (prio > 1)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                    if (ImGui::SmallButton("^ Prio"))
                        indiceSubirPrio = i;
                    ImGui::PopStyleColor();
                    ImGui::SameLine();
                }
                ImGui::PushStyleColor(ImGuiCol_Button, ACCENT_DANGER);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                if (ImGui::SmallButton("Eliminar"))
                    indiceEliminar = i;
                ImGui::PopStyleColor(2);

                ImGui::PopID();
            }

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
    }
    else
    {
        ImGui::TextDisabled(
            "Demostracion de Reutilizacion de Plantilla (Template): la misma clase Cola<T> instanciada como Cola<std::string>:"
        );
        ImGuiTableFlags tableFlags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_SizingStretchProp;

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(6.0f, 4.0f));
        if (ImGui::BeginTable("TablaColaString", 4, tableFlags, ImVec2(0, altoAreaCola - 24.0f)))
        {
            ImGui::TableSetupColumn("Referencia", ImGuiTableColumnFlags_WidthFixed, 110.0f);
            ImGui::TableSetupColumn("Nodo<string>*", ImGuiTableColumnFlags_WidthFixed, 115.0f);
            ImGui::TableSetupColumn("Prioridad", ImGuiTableColumnFlags_WidthFixed, 160.0f);
            ImGui::TableSetupColumn("Dato almacenado (T = std::string)", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            int cantAlertas = colaAlertasClinicas.mostrar_tamanio();
            for (int i = 0; i < cantAlertas; ++i)
            {
                string textoAlerta;
                int prioAlerta = 0;
                colaAlertasClinicas.obtenerElemento(i, textoAlerta, prioAlerta);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (i == 0 && cantAlertas == 1)
                    ImGui::TextColored(ACCENT_SUCCESS, "[1] FRENTE=FIN");
                else if (i == 0)
                    ImGui::TextColored(ACCENT_SUCCESS, "[1] FRENTE");
                else if (i == cantAlertas - 1)
                    ImGui::TextColored(ACCENT_PRIMARY, "[%d] FIN", i + 1);
                else
                    ImGui::Text("[%d] Nodo", i + 1);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%p", colaAlertasClinicas.obtenerDireccionNodo(i));

                ImGui::TableSetColumnIndex(2);
                ImGui::TextColored(obtenerColor(prioAlerta), "%s", obtenerNombrePrioridad(prioAlerta).c_str());

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%s", textoAlerta.c_str());
            }

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
    }

    if (indiceSubirPrio >= 0)
    {
        Paciente pTemp;
        int prioActual = 1;
        if (colaPacientes.obtenerElemento(indiceSubirPrio, pTemp, prioActual) && prioActual > 1)
        {
            colaPacientes.actualizarPrioridad(indiceSubirPrio, prioActual - 1);
            colaAlertasClinicas.actualizarPrioridad(indiceSubirPrio, prioActual - 1);
            AnimacionNodo& anim = obtenerAnimNodo(pTemp.getId());
            anim.progresoEntrada = 0.25f;
            anim.flashCambio = 1.0f;
            mensaje = "Prioridad escalada para " + pTemp.getNombre() + " a Nivel " + std::to_string(prioActual - 1) + ".";
            mensajeError = false;
            g_animBanner = 0.0f;
        }
    }
    else if (indiceEliminar >= 0)
    {
        Paciente pEliminado;
        if (colaPacientes.eliminarEnPosicion(indiceEliminar, pEliminado))
        {
            string alertaElim;
            colaAlertasClinicas.eliminarEnPosicion(indiceEliminar, alertaElim);
            mensaje = "Paciente retirado de la cola: " + pEliminado.getNombre() + ".";
            mensajeError = false;
            g_animBanner = 0.0f;
        }
    }

    ImGui::EndChild();

    ImGui::End();
}

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
            fuenteRegular = io.Fonts->AddFontFromFileTTF(ruta, 15.0f);
            break;
        }
    }

    for (const char* ruta : rutasBold)
    {
        FILE* f = std::fopen(ruta, "rb");
        if (f != nullptr)
        {
            std::fclose(f);
            fuenteBold = io.Fonts->AddFontFromFileTTF(ruta, 17.0f);
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
        L"MediPriority - Sistema de Triaje y Priorizacion de Pacientes en Emergencias (Grupo 2)",
        WS_OVERLAPPEDWINDOW,
        60, 40, 1340, 820,
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
    io.IniFilename = nullptr;

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

        RECT rcClient;
        if (::GetClientRect(hwnd, &rcClient))
        {
            int w = rcClient.right - rcClient.left;
            int h = rcClient.bottom - rcClient.top;
            if (w > 0 && h > 0)
            {
                g_Ancho = w;
                g_Alto = h;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        dibujarInterfaz();

        ImGui::Render();
        glViewport(0, 0, g_Ancho, g_Alto);
        if (temaClaro)
            glClearColor(0.969f, 0.976f, 0.988f, 1.0f);
        else
            glClearColor(0.118f, 0.125f, 0.141f, 1.0f);
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
        1340,
        820,
        "MediPriority - Sistema de Triaje y Priorizacion de Pacientes en Emergencias (Grupo 2)",
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
    io.IniFilename = nullptr;

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

        if (temaClaro)
            glClearColor(0.969f, 0.976f, 0.988f, 1.0f);
        else
            glClearColor(0.118f, 0.125f, 0.141f, 1.0f);
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
