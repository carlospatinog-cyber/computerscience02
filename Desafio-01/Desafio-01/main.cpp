#include <iostream> // Se usa para incluir archivos de cabecera iostrem
#include <fstream> //Esta biblioteca proporciona las clases necesarias para realizar operaciones de entrada y salida (lectura y escritura) de archivos en el código

using namespace std;

// --- Rotación a la derecha de un byte ---
unsigned char rotarDerecha(unsigned char valor, int n) {
    return (valor >> n) | (valor << (8 - n));
}

int main() {
    const char* archivoEnc = "Encriptado1.txt";
    ifstream in(archivoEnc, ios::binary);
    if (!in) {
        cout << "Error abriendo archivo: " << archivoEnc << endl;
        return 1;
    }

    // --- Parametros de desencriptación ---
    const unsigned char K = 0x5A;  // Clave XOR (cambiar para probar)
    const int n = 3;               // Número de bits a rotar a la derecha (cambiar para probar)

    // --- Leer archivo completo a memoria ---
    in.seekg(0, ios::end);
    long tamArchivo = in.tellg();
    in.seekg(0, ios::beg);

    unsigned char* buffer = new unsigned char[tamArchivo];
    in.read((char*)buffer, tamArchivo);
    in.close();

    // --- Extraer bloques después de "ZR" ---
    unsigned char* extraido = new unsigned char[tamArchivo]; // tamaño máx posible
    long tamExtraido = 0;

    for (long i = 0; i < tamArchivo - 1; i++) {
        if (buffer[i] == 'Z' && buffer[i + 1] == 'R') {
            if (i + 2 < tamArchivo) {
                extraido[tamExtraido++] = buffer[i + 2];
            }
        }
    }

    cout << "[DEBUG] Bloques extraidos: " << tamExtraido << endl;