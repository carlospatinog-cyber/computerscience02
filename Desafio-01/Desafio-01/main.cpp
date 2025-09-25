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
    // Crear diccionario
    char** diccionario = new char*[cantidadPares];
    int* tamaños = new int[cantidadPares];

    for (int i = 0; i < cantidadPares; i++) {
        diccionario[i] = nullptr;
        tamaños[i] = 0;
    }

    // Calcular tamaño total del resultado
    int tamañoTotal = 0;
    for (int i = 0; i < cantidadPares; i++) {
        if (numeros[i] == 0) {
            tamaños[i] = 1;
            tamañoTotal += 1;
        } else {
            tamaños[i] = tamaños[numeros[i] - 1] + 1;
            tamañoTotal += tamaños[i];
        }
    }

    // Crear espacio para el resultado
    char* textoOriginal = new char[tamañoTotal + 1];
    int posResultado = 0;

    // Reconstruir el texto
    for (int i = 0; i < cantidadPares; i++) {
        if (numeros[i] == 0) {
            // Caso simple: nueva letra
            textoOriginal[posResultado] = letras[i];
            posResultado++;

            // Guardar en diccionario
            diccionario[i] = new char[2];
            diccionario[i][0] = letras[i];
            diccionario[i][1] = '\0';
        } else {
            // Caso con referencia al diccionario
            int indiceRef = numeros[i] - 1;

            // Copiar texto del diccionario
            if (diccionario[indiceRef] != nullptr) {
                int k = 0;
                while (diccionario[indiceRef][k] != '\0') {
                    textoOriginal[posResultado] = diccionario[indiceRef][k];
                    posResultado++;
                    k++;
                }
            }
            // Añadir la nueva letra
            textoOriginal[posResultado] = letras[i];
            posResultado++;

            // Guardar en diccionario
            int tamañoNuevo = tamaños[i];
            diccionario[i] = new char[tamañoNuevo + 1];

            if (diccionario[indiceRef] != nullptr) {
                int k = 0;
                while (diccionario[indiceRef][k] != '\0') {
                    diccionario[i][k] = diccionario[indiceRef][k];
                    k++;
                }
                diccionario[i][k] = letras[i];
                diccionario[i][k + 1] = '\0';
            }
        }
    }

    textoOriginal[posResultado] = '\0';

    // Liberar memoria del diccionario
    for (int i = 0; i < cantidadPares; i++) {
        if (diccionario[i] != nullptr) {
            delete[] diccionario[i];
        }
    }
    delete[] diccionario;
    delete[] tamaños;
    delete[] numeros;
    delete[] letras;

    return textoOriginal;
}

textoOriginal[posResultado] = '\0';

// Liberar memoria del diccionario
for (int i = 0; i < cantidadPares; i++) {
    if (diccionario[i] != nullptr) {
        delete[] diccionario[i];
    }
}
delete[] diccionario;
delete[] tamaños;
delete[] numeros;
delete[] letras;

return textoOriginal;
}

// Función para buscar un texto dentro de otro
bool buscarTexto(const char* textoGrande, const char* textoBuscar) {
    int tamañoGrande = 0;
    int tamañoBuscar = 0;

    while (textoGrande[tamañoGrande] != '\0') {
        tamañoGrande++;
    }
    while (textoBuscar[tamañoBuscar] != '\0') {
        tamañoBuscar++;
    }

    if (tamañoBuscar > tamañoGrande) {
        return false;
    }

    for (int i = 0; i <= tamañoGrande - tamañoBuscar; i++) {
        bool encontrado = true;
        for (int j = 0; j < tamañoBuscar; j++) {
            if (textoGrande[i + j] != textoBuscar[j]) {
                encontrado = false;
                break;
            }
        }
        if (encontrado) {
            return true;
        }
    }

    return false;
}

int main() {
    // DATOS DE ENTRADA (cambiar por los valores reales)
    const char* mensajeCifrado = "MensajeEncriptadoQueNosDan";
    const char* fragmentoConocido = "TextoQueSabemosQueEsta";

    // Variables para guardar la solución
    int mejorN = 0;
    char mejorClave = 0;
    bool usaRLE = true;
    char* mensajeFinal = nullptr;

    bool solucionEncontrada = false;

    cout << "Buscando solucion..." << endl;

    // Probar todas las combinaciones posibles
    for (int n = 1; n < 8; n++) {
        for (int clave = 0; clave < 256; clave++) {

            // Paso 1: Desencriptar (rotación derecha + XOR)
            char* textoDesencriptado = desencriptarMensaje(mensajeCifrado, n, clave);

            // Paso 2: Probar con RLE primero
            char* textoRLE = descomprimirRLE(textoDesencriptado);
            if (textoRLE != nullptr && buscarTexto(textoRLE, fragmentoConocido)) {
                mejorN = n;
                mejorClave = clave;
                usaRLE = true;
                mensajeFinal = textoRLE;
                solucionEncontrada = true;
                delete[] textoDesencriptado;
                break;
            }
            if (textoRLE != nullptr) {
                delete[] textoRLE;
            }

            // Paso 3: Probar con LZ78 si RLE no funcionó
            char* textoLZ78 = descomprimirLZ78(textoDesencriptado);
            if (textoLZ78 != nullptr && buscarTexto(textoLZ78, fragmentoConocido)) {
                mejorN = n;
                mejorClave = clave;
                usaRLE = false;
                mensajeFinal = textoLZ78;
                solucionEncontrada = true;
                delete[] textoDesencriptado;
                break;
            }
            if (textoLZ78 != nullptr) {
                delete[] textoLZ78;
            }

            delete[] textoDesencriptado;

            if (solucionEncontrada) {
                break;
            }
        }
        if (solucionEncontrada) {
            break;
        }
    }

    // Mostrar resultados
    if (solucionEncontrada) {
        cout << "\n=== SOLUCION ENCONTRADA ===" << endl;
        cout << "Metodo de compresion: " << (usaRLE ? "RLE" : "LZ78") << endl;
        cout << "Rotacion (n): " << mejorN << " bits a la IZQUIERDA" << endl;
        cout << "Clave (K): " << (int)mejorClave << endl;
        cout << "Mensaje original: " << mensajeFinal << endl;

        delete[] mensajeFinal;
    } else {
        cout << "No se pudo encontrar la solucion." << endl;
        cout << "Verifique los datos de entrada." << endl;
    }

    return 0;
}
