#include <iostream>
#include <cstring>

using namespace std;

// Función para rotar bits a la izquierda.
unsigned char rotarIzquierda(unsigned char valor, int desplazamiento) {
    return (valor >> desplazamiento) | (valor << (8 - desplazamiento));
}

// Función para rotar bits a la DERECHA (para desencriptar)
unsigned char rotarDerecha(unsigned char valor, int desplazamiento) {
    return (valor >> desplazamiento) | (valor << (8 - desplazamiento));
}


// Función para desencriptar
char* desencriptar(const char* mensajeEncriptado, int n, char K) {
    int largo = strlen(mensajeEncriptado);
    char* resultado = new char[largo + 1];

    for (int i = 0; i < largo; i++) {
        unsigned char letra = mensajeEncriptado[i];
        letra = letra ^ K;          // Operación XOR
        letra = rotarDerecha(letra, n); // Rotación
        resultado[i] = letra;
    }
    resultado[largo] = '\0';

    return resultado;
}

