#ifndef VALIDACION_H
#define VALIDACION_H

#include <string>
#include <cctype>

// COMPROBAR SI UN TEXTO ESTA VACIO O SOLO CONTIENE ESPACIOS
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

// COMPROBAR QUE EL NOMBRE CONTENGA SOLO LETRAS Y ESPACIOS VALIDOS
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

// VALIDAR NOMBRE
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

// VALIDAR EDAD
inline bool validarEdad(int edad)
{
    return edad >= 1 && edad <= 120;
}

// VALIDAR SINTOMAS
inline bool validarSintomas(const std::string& sintomas)
{
    if (textoVacio(sintomas))
        return false;

    if (sintomas.length() > 199)
        return false;

    return true;
}

// VALIDAR PRIORIDAD (ESCALA DE TRIAJE 1 A 5)
inline bool validarPrioridad(int prioridad)
{
    return prioridad >= 1 && prioridad <= 5;
}

// VALIDAR TODOS LOS DATOS DEL PACIENTE
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
        error = "El nombre excede el limite de 99 caracteres.";
        return false;
    }

    if (!validarEdad(edad))
    {
        error = "La edad debe estar en un rango valido entre 1 y 120 anios.";
        return false;
    }

    if (!validarSintomas(sintomas))
    {
        error = "Ingrese los sintomas o motivo de consulta del paciente.";
        return false;
    }

    if (!validarPrioridad(prioridad))
    {
        error = "Seleccione un nivel de prioridad valido entre 1 y 5.";
        return false;
    }

    error = "";
    return true;
}

#endif // VALIDACION_H
