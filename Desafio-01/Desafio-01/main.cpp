#include <iostream>

using namespace std;

// Función para rotar bits a la IZQUIERDA (para verificar)
unsigned char rotarIzquierda(unsigned char letra, int n) {
    return (letra << n) | (letra >> (8 - n));
}

// Función para rotar bits a la DERECHA (para desencriptar)
unsigned char rotarDerecha(unsigned char letra, int n) {
    return (letra >> n) | (letra << (8 - n));
}



