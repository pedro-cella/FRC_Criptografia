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
void decrypt_file(FILE *input_file, FILE *output_file, unsigned long long d, unsigned long long n) {
    unsigned long long byte;
    while ((byte = fgetc(input_file)) != EOF) {
        unsigned long long decrypted_byte = mod_pow(byte, d, n);
        fputc(decrypted_byte, output_file);
    }
}

unsigned long long main(unsigned long long argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s arquivo_encriptado\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1], "rb");
    if (input_file == NULL) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        return 1;
    }

    FILE *output_file = fopen("arquivo_desencriptado.bmp", "wb");
    if (output_file == NULL) {
        printf("Erro ao abrir o arquivo de saída.\n");
        fclose(input_file);
        return 1;
    }

    FILE *public_key_file = fopen("generated_key.priv", "r");
    if (public_key_file == NULL) {
        printf("Erro ao abrir o arquivo da chave privada.\n");
        fclose(input_file);
        fclose(output_file);
        return 1;
    }

    unsigned long long n, d;
    fscanf(public_key_file, "%lld@%lld", &n, &d);
    fclose(public_key_file);

    printf("[+] N = %lld\n", n);
    printf("[+] D = %lld\n", d);

    decrypt_file(input_file, output_file, d, n);

    fclose(input_file);
    fclose(output_file);

    printf("Arquivo desencriptado com sucesso.\n");

    return 0;
}
