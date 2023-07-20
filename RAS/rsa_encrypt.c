#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

int mod_pow(int base, int exponent, int modulus) {
    int result = 1;
    base %= modulus;
    while (exponent > 0) {
        if (exponent & 1)
            result = (result * base) % modulus;
        exponent >>= 1;
        base = (base * base) % modulus;
    }
    return result;
}

void encrypt_file(FILE *input_file, FILE *output_file, int e, int n) {
    int byte;
    while ((byte = fgetc(input_file)) != EOF) {
        int encrypted_byte = mod_pow(byte, e, n);
        fputc(encrypted_byte, output_file);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s arquivo_a_encriptar\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1], "rb");
    if (input_file == NULL) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        return 1;
    }

    FILE *output_file = fopen("arquivo_encriptado", "wb");
    if (output_file == NULL) {
        printf("Erro ao abrir o arquivo de saída.\n");
        fclose(input_file);
        return 1;
    }

    FILE *public_key_file = fopen("chave.pub", "r");
    if (public_key_file == NULL) {
        printf("Erro ao abrir o arquivo da chave pública.\n");
        fclose(input_file);
        fclose(output_file);
        return 1;
    }

    int n, e;
    fscanf(public_key_file, "%d#%d", &n, &e);
    fclose(public_key_file);

    encrypt_file(input_file, output_file, e, n);

    fclose(input_file);
    fclose(output_file);

    printf("Arquivo encriptado com sucesso.\n");

    return 0;
}
