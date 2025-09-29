#include <iostream> //directiva para incluir la biblioteca estándar de C++ que proporciona funciones para operaciones de entrada y salida (input/output
#include <fstream> //librería que permite trabajar con archivos externos (también llamados ficheros) para la lectura y escritura de datos

using namespace std; // indicar al compilador que todos los identificadores (como cin, cout, vector, etc.) se consideren parte del espacio de nombres std (la librería estándar), eliminando la necesidad de escribir std:: antes de cada uno.

// --- Funciones basicas para la lectura del dataset ---
//evalua  si un carácter es un  numero (0-9)
bool esDigito(char c) {
    return c >= '0' && c <= '9';
}

//evalua  si un carácter es una letra valida (excluye tildes, Ñ y caracteres extendidos)
bool esLetra(char c) {
    unsigned char uc = (unsigned char)c;
    // Excluir caracteres extendidos (donde están tildes, Ñ, diéresis,  etc.)
    if (uc >= 0xC0) return false; //0xC0 en hexadecimal = 192 en decimal => Todos los caracteres desde 192 hasta 255 son EXCLUIDOS
    // Permitir solo A-Z y a-z básicos
    return (uc >= 'A' && uc <= 'Z') || (uc >= 'a' && uc <= 'z');
}

// Función adicional para validar caracteres permitidos (letras válidas y dígitos)
bool esCaracterPermitido(char c) {
    return esLetra(c) || esDigito(c);
}

// --- Rotación a la derecha de un byte ---
// Realizar una rotación de bits a la derecha en un byte
unsigned char rotarDerecha(unsigned char valor, int n) {
    return (valor >> n) | (valor << (8 - n));
}

// --- Función para leer archivos ---
bool leerArchivo(const char* nombreArchivo, unsigned char*& buffer, size_t& tamano) {
    ifstream archivo(nombreArchivo, ios::in | ios::binary | ios::ate);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << nombreArchivo << endl;
        buffer = nullptr;
        tamano = 0;
        return false;
    }

    tamano = archivo.tellg();
    archivo.seekg(0, ios::beg);

    buffer = new unsigned char[tamano];
    if (!buffer) {
        cerr << "Error al asignar memoria para el archivo." << endl;
        archivo.close();
        tamano = 0;
        return false;
    }

    archivo.read((char*)buffer, tamano);
    archivo.close();
    return true;
}

// --- Detectar si es RLE ---
bool esRLE(const unsigned char* mensaje, size_t tamano) {
    for (size_t i = 0; i < tamano; i++) {
        if (esDigito(mensaje[i])) {
            // Avanzar sobre todos los dígitos
            while (i < tamano && esDigito(mensaje[i])) {
                i++;
            }
            // Después de los dígitos debe haber una letra VÁLIDA
            if (i >= tamano || !esLetra(mensaje[i])) {
                return false;
            }
        } else if (!esLetra(mensaje[i])) {
            return false; // Carácter no válido (incluye tildes, Ñ, etc.)
        }
    }
    return true;
}

// --- Descomprimir RLE (CORREGIDA) ---
bool descomprimirRLE(const unsigned char* comprimido, size_t tamComprimido,
                     unsigned char*& descomprimido, size_t& tamDescomprimido) {
    // Primera pasada: calcular tamaño necesario
    tamDescomprimido = 0;

    for (size_t i = 0; i < tamComprimido; i++) {
        if (esDigito(comprimido[i])) {
            // Leer número completo (puede tener múltiples dígitos)
            int count = 0;
            while (i < tamComprimido && esDigito(comprimido[i])) {
                count = count * 10 + (comprimido[i] - '0');
                i++;
            }
            // Verificar que hay una letra VÁLIDA después del número
            if (i < tamComprimido && esLetra(comprimido[i])) {
                tamDescomprimido += count;
            } else {
                return false; // Formato inválido
            }
        } else if (esLetra(comprimido[i])) {
            tamDescomprimido++; // Letra válida sola = count 1
        } else {
            return false; // Carácter inválido (tildes, Ñ, etc.)
        }
    }

    // Segunda pasada: descomprimir
    descomprimido = new unsigned char[tamDescomprimido + 1];
    size_t pos = 0;

    for (size_t i = 0; i < tamComprimido; i++) {
        if (esDigito(comprimido[i])) {
            // Leer número completo
            int count = 0;
            while (i < tamComprimido && esDigito(comprimido[i])) {
                count = count * 10 + (comprimido[i] - '0');
                i++;
            }
            // Repetir la letra 'count' veces
            if (i < tamComprimido && esLetra(comprimido[i])) {
                for (int j = 0; j < count; j++) {
                    descomprimido[pos++] = comprimido[i];
                }
            }
        } else if (esLetra(comprimido[i])) {
            descomprimido[pos++] = comprimido[i]; // Letra válida sola
        }
    }
    descomprimido[tamDescomprimido] = '\0';
    return true;
}

// --- Procesar archivo encriptado ---
void procesarEncriptado(const unsigned char* buffer, size_t tamBuffer,
                        unsigned char*& resultado, size_t& tamResultado) {
    // Parámetros de desencriptación
    const unsigned char K = 0x5A;  // Clave XOR
    const int n = 3;               // Bits a rotar

    // Extraer bloques después de "ZR"
    unsigned char* extraido = new unsigned char[tamBuffer];
    size_t tamExtraido = 0;

    for (size_t i = 0; i < tamBuffer - 1; i++) {
        if (buffer[i] == 'Z' && buffer[i + 1] == 'R') {
            if (i + 2 < tamBuffer) {
                extraido[tamExtraido++] = buffer[i + 2];
            }
        }
    }

    // Desencriptar (XOR + Rotación derecha)
    resultado = new unsigned char[tamExtraido + 1];
    tamResultado = tamExtraido;

    for (size_t i = 0; i < tamExtraido; i++) {
        unsigned char x = extraido[i] ^ K;
        resultado[i] = rotarDerecha(x, n);
    }
    resultado[tamExtraido] = '\0';

    delete[] extraido;
}

// Función para validar texto final (opcional - para debugging)
bool validarTextoFinal(const unsigned char* texto, size_t tamano) {
    for (size_t i = 0; i < tamano; i++) {
        if (!esCaracterPermitido(texto[i]) && texto[i] != ' ') {
            cout << "Carácter no permitido detectado: '" << texto[i]
                 << "' (ASCII: " << (int)texto[i] << ")" << endl;
            return false;
        }
    }
    return true;
}

// --- Función principal ---
int main() {
    int n;
    cout << "Ingrese la cantidad de archivos a evaluar: ";
    cin >> n;

    for (int i = 1; i <= n; ++i) {
        char nombreEncriptado[20];
        char nombrePista[20];

        snprintf(nombreEncriptado, sizeof(nombreEncriptado), "Encriptado%d.txt", i);
        snprintf(nombrePista, sizeof(nombrePista), "pista%d.txt", i);

        // Leer archivos
        unsigned char* mensajeEncriptado = nullptr;
        size_t tamanoEncriptado = 0;
        unsigned char* pista = nullptr;
        size_t tamanoPista = 0;

        if (!leerArchivo(nombreEncriptado, mensajeEncriptado, tamanoEncriptado)) {
            continue;
        }

        if (!leerArchivo(nombrePista, pista, tamanoPista)) {
            delete[] mensajeEncriptado;
            continue;
        }

        cout << "\n=== Procesando Archivo " << i << " ===" << endl;

        // Procesar encriptado
        unsigned char* desencriptado = nullptr;
        size_t tamDesencriptado = 0;
        procesarEncriptado(mensajeEncriptado, tamanoEncriptado, desencriptado, tamDesencriptado);

        cout << "Texto desencriptado (" << tamDesencriptado << " bytes):" << endl;
        // Mostrar solo los primeros 100 caracteres para debug
        for (size_t j = 0; j < 100 && j < tamDesencriptado; ++j) {
            cout << desencriptado[j];
        }
        cout << endl;

        // Detectar método de compresión y descomprimir
        unsigned char* descomprimido = nullptr;
        size_t tamDescomprimido = 0;

        if (esRLE(desencriptado, tamDesencriptado)) {
            cout << "Metodo detectado: RLE" << endl;
            if (descomprimirRLE(desencriptado, tamDesencriptado, descomprimido, tamDescomprimido)) {
                // Validar texto descomprimido (opcional)
                if (validarTextoFinal(descomprimido, tamDescomprimido)) {
                    cout << "Texto validado correctamente - sin tildes ni caracteres no permitidos" << endl;
                }

                cout << "Texto descomprimido (" << tamDescomprimido << " bytes):" << endl;
                // Mostrar resultado completo
                for (size_t j = 0; j < tamDescomprimido; ++j) {
                    cout << descomprimido[j];
                }
                cout << endl;
            } else {
                cout << "Error en descompresión RLE" << endl;
            }
        } else {
            cout << "Metodo no reconocido o sin compresión" << endl;
            // Validar texto desencriptado (opcional)
            if (validarTextoFinal(desencriptado, tamDesencriptado)) {
                cout << "Texto validado correctamente - sin tildes ni caracteres no permitidos" << endl;
            }

            cout << "Resultado final:" << endl;
            for (size_t j = 0; j < tamDesencriptado; ++j) {
                cout << desencriptado[j];
            }
            cout << endl;
        }

        // Mostrar pista
        cout << "Pista: ";
        for (size_t j = 0; j < tamanoPista; ++j) {
            cout << pista[j];
        }
        cout << "\n==========================" << endl;

        // Liberar memoria
        delete[] mensajeEncriptado;
        delete[] pista;
        delete[] desencriptado;
        if (descomprimido) delete[] descomprimido;
    }

    return 0;
}
