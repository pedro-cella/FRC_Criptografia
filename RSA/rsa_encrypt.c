#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>


unsigned long long ipow(unsigned long long base, unsigned long long exp)
{
    unsigned long long result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

unsigned long long mod_pow(unsigned long long base, unsigned long long exponent, unsigned long long modulus) {
    return ipow(base, exponent) % modulus;;
}

void encrypt_file(FILE *input_file, FILE *output_file, unsigned long long e, unsigned long long n) {
    unsigned long long byte;
    while ((byte = fgetc(input_file)) != EOF) {

        printf("ORIGINAL - CHAR: %c, ASCII: %d\n", byte, byte);
        unsigned long long encrypted_byte = mod_pow(byte, e, n);
        fputc(encrypted_byte, output_file);
        printf("ECNRYPTED - CHAR: %c, ASCII: %d\n\n", encrypted_byte, encrypted_byte);

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

    FILE *priv_key_file = fopen("generated_key.pub", "r");
    if (priv_key_file == NULL) {
        printf("Erro ao abrir o arquivo da chave pública.\n");
        fclose(input_file);
        fclose(output_file);
        return 1;
    }

    unsigned long long n, e;
    fscanf(priv_key_file, "%d@%d", &n, &e);
    fclose(priv_key_file);

    printf("[+] N = %d\n", n);
    printf("[+] E = %d\n", e);

    encrypt_file(input_file, output_file, e, n);

    fclose(input_file);
    fclose(output_file);

    printf("Arquivo encriptado com sucesso.\n");

    return 0;
}
