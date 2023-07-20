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

void decrypt_file(FILE *input_file, FILE *output_file, int d, int n) {
    int byte;
    while ((byte = fgetc(input_file)) != EOF) {
        int decrypted_byte = mod_pow(byte, d, n);
        fputc(decrypted_byte, output_file);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s arquivo_encriptado\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1], "rb");
    if (input_file == NULL) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        return 1;
    }

    FILE *output_file = fopen("arquivo_desencriptado", "wb");
    if (output_file == NULL) {
        printf("Erro ao abrir o arquivo de saída.\n");
        fclose(input_file);
        return 1;
    }

    FILE *private_key_file = fopen("chave.priv", "r");
    if (private_key_file == NULL) {
        printf("Erro ao abrir o arquivo da chave privada.\n");
        fclose(input_file);
        fclose(output_file);
        return 1;
    }

    int n, d;
    fscanf(private_key_file, "%d#%d", &n, &d);
    fclose(private_key_file);

    decrypt_file(input_file, output_file, d, n);

    fclose(input_file);
    fclose(output_file);

    printf("Arquivo desencriptado com sucesso.\n");

    return 0;
}
