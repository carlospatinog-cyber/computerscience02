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

    // Crear espacio para el resultado
    char* textoOriginal = new char[espacioNecesario + 1];
    int posicion = 0;

    // Descomprimir
    i = 0;
    while (i < tamañoComp) {
        if (textoComprimido[i] >= '0' && textoComprimido[i] <= '9') {
            // Leer cuántas veces se repite
            int repeticiones = 0;
            while (i < tamañoComp && textoComprimido[i] >= '0' && textoComprimido[i] <= '9') {
                repeticiones = repeticiones * 10 + (textoComprimido[i] - '0');
                i++;
            }

            // Leer la letra que se repite
            if (i < tamañoComp) {
                char letra = textoComprimido[i];
                i++;
                // Repetir la letra
                for (int j = 0; j < repeticiones; j++) {
                    textoOriginal[posicion] = letra;
                    posicion++;
                }
            }
        } else {
            i++;
        }
    }

    textoOriginal[posicion] = '\0';
    return textoOriginal;
}

// Función para descomprimir LZ78
char* descomprimirLZ78(const char* textoComprimido) {
    // Contar tamaño del texto comprimido
    int tamañoComp = 0;
    while (textoComprimido[tamañoComp] != '\0') {
        tamañoComp++;
    }

    // Contar cuántos pares (numero,letra) hay
    int cantidadPares = 0;
    for (int i = 0; i < tamañoComp; i++) {
        if (textoComprimido[i] == '(') {
            cantidadPares++;
        }
    }

    if (cantidadPares == 0) {
        return nullptr;
    }

    // Arrays para guardar los números y letras
    int* numeros = new int[cantidadPares];
    char* letras = new char[cantidadPares];
    int parActual = 0;

    // Leer todos los pares
    for (int i = 0; i < tamañoComp; i++) {
        if (textoComprimido[i] == '(') {
            i++; // Saltar el '('

            // Leer el número
            numeros[parActual] = 0;
            while (i < tamañoComp && textoComprimido[i] >= '0' && textoComprimido[i] <= '9') {
                numeros[parActual] = numeros[parActual] * 10 + (textoComprimido[i] - '0');
                i++;
            }

            // Saltar la coma y espacios
            while (i < tamañoComp && (textoComprimido[i] == ',' || textoComprimido[i] == ' ')) {
                i++;
            }

            // Leer la letra
            if (i < tamañoComp && textoComprimido[i] != ')') {
                letras[parActual] = textoComprimido[i];
                i++;
            }

            parActual++;
        }
    }
