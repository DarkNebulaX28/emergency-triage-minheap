#ifndef VALIDACION_H
#define VALIDACION_H

#include <iostream>
#include "operaciones_colaPriori.h"
#include <string>


using namespace std;

bool leerEntero(string& mensaje, int& valor){
    
    if(!(cin >> valor)){
        cout << "ERROR: entrada invalida, ingrese un valor entero\n";
        return false;
    }

    return true;
}

bool leerTexto (string& mensaje, string& texto){

    if(!(getline(cin,texto))){
        cout << "ERROR: entrada invalida, ingrese un texto\n";
        return false;
    }
    return true;
}

#endif //VALIDACION_H