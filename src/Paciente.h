#ifndef PACIENTE_H
#define PACIENTE_H

#include <string>
#include <iostream>
#include <ctime>
#include <cstdio>

using namespace std;

class Paciente {
private:
    int id;
    string nombre;
    int edad;
    string sintomas;
    string horaIngreso;

    static int siguienteId();
    static string obtenerHoraActual();

public:
    Paciente();
    Paciente(string nom, int ed, string sint);
    Paciente(int idManual, string nom, int ed, string sint, string hora);

    int getId() const;
    string getNombre() const;
    int getEdad() const;
    string getSintomas() const;
    string getHoraIngreso() const;

    void setNombre(const string& nom);
    void setEdad(int ed);
    void setSintomas(const string& sint);

    friend ostream& operator<<(ostream& os, const Paciente& p);
};

#endif
