#ifndef PACIENTE_H
#define PACIENTE_H

#include <string>
#include <iostream>
#include <ctime>
#include <cstdio>

using namespace std;

struct Paciente {
    int id;
    string nombre;
    int edad;
    string sintomas;
    string horaIngreso;

    static int siguienteId() {
        static int contadorId = 1;
        return contadorId++;
    }

    static string obtenerHoraActual() {
        std::time_t ahora = std::time(nullptr);
        std::tm* tiempoLocal = std::localtime(&ahora);
        if (tiempoLocal == nullptr) {
            return "00:00:00";
        }
        char buffer[16];
        std::snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d",
                      tiempoLocal->tm_hour,
                      tiempoLocal->tm_min,
                      tiempoLocal->tm_sec);
        return string(buffer);
    }

    Paciente()
        : id(0), nombre(""), edad(0), sintomas(""), horaIngreso("--:--:--") {}

    Paciente(string nom, int ed, string sint)
        : id(siguienteId()),
          nombre(nom),
          edad(ed),
          sintomas(sint),
          horaIngreso(obtenerHoraActual()) {}

    Paciente(int idManual, string nom, int ed, string sint, string hora)
        : id(idManual),
          nombre(nom),
          edad(ed),
          sintomas(sint),
          horaIngreso(hora) {}

    friend ostream& operator<<(ostream& os, const Paciente& p) {
        os << "[#" << p.id << " | " << p.horaIngreso << "] "
           << "Paciente: " << p.nombre
           << " | Edad: " << p.edad
           << " | Sintomas: " << p.sintomas;
        return os;
    }
};

#endif // PACIENTE_H
