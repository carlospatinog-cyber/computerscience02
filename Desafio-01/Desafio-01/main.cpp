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

// Función para desencriptar - ORDEN CORRECTO
char* desencriptarMensaje(const char* textoCifrado, int n, char clave) {
    // Contar cuántas letras tiene el mensaje
    int tamaño = 0;
    while (textoCifrado[tamaño] != '\0') {
        tamaño++;
    }

    // Crear espacio para guardar el resultado
    char* textoDescifrado = new char[tamaño + 1];

    // Procesar cada letra del mensaje
    for (int i = 0; i < tamaño; i++) {
        unsigned char letra = textoCifrado[i];

        // ORDEN CORRECTO para desencriptar:
        // 1. XOR con la clave (revierte el XOR de la encriptación)
        // 2. Rotación a la DERECHA (revierte la rotación a la IZQUIERDA de la encriptación)

        letra = letra ^ clave;          // Paso 1: XOR
        letra = rotarDerecha(letra, n); // Paso 2: Rotación derecha

        textoDescifrado[i] = letra;
    }

    // Poner marca de final de texto
    textoDescifrado[tamaño] = '\0';

    return textoDescifrado;
}

// Función para descomprimir RLE
char* descomprimirRLE(const char* textoComprimido) {
    // Contar tamaño del texto comprimido
    int tamañoComp = 0;
    while (textoComprimido[tamañoComp] != '\0') {
        tamañoComp++;
    }

    // Calcular cuánto espacio necesitamos
    int espacioNecesario = 0;
    int i = 0;
    while (i < tamañoComp) {
        if (textoComprimido[i] >= '0' && textoComprimido[i] <= '9') {
            // Leer el número
            int numero = 0;
            while (i < tamañoComp && textoComprimido[i] >= '0' && textoComprimido[i] <= '9') {
                numero = numero * 10 + (textoComprimido[i] - '0');
                i++;
            }
            espacioNecesario += numero;
        } else {
            i++;
        }
    }

