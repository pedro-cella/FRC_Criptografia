#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define MIN_DIGITS 10000  // Mínimo de cinco dígitos
#define MAX_DIGITS 999999 // Máximo de seis dígitos

bool is_prime(int num) {
    if (num <= 1)
        return false;
    
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0)
            return false;
    }

    return true;
}

int generate_prime(int min, int max) {
    int num;
    do {
        num = rand() % (max - min + 1) + min;
    } while (!is_prime(num));

    return num;
}

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int compute_n(int p, int q) {
    return p * q;
}

int compute_z(int p, int q) {
    return (p - 1) * (q - 1);
}

int choose_e(int z) {
    int e;
    do {
        e = rand() % (z - 2) + 2; // Começa em 2 para garantir que e < z
    } while (gcd(e, z) != 1);

    return e;
}

int choose_d(int e, int z) {
    int d = 1;
    while ((d * e) % z != 1)
        d++;

    return d;
}

int main() {
    srand(time(NULL));

    int p = generate_prime(MIN_DIGITS, MAX_DIGITS);
    int q = generate_prime(MIN_DIGITS, MAX_DIGITS);

    printf("Números primos gerados:\n");
    printf("p = %d\n", p);
    printf("q = %d\n", q);

    FILE *primes_file = fopen("primos.txt", "w");
    if (primes_file == NULL) {
        printf("Erro ao abrir o arquivo 'primos.txt'.\n");
        return 1;
    }

    fprintf(primes_file, "p = %d\n", p);
    fprintf(primes_file, "q = %d\n", q);
    fclose(primes_file);

    int n = compute_n(p, q);
    int z = compute_z(p, q);

    int e = choose_e(z);
    int d = choose_d(e, z);

    FILE *public_key_file = fopen("chave.pub", "w");
    if (public_key_file == NULL) {
        printf("Erro ao abrir o arquivo da chave pública.\n");
        return 1;
    }

    fprintf(public_key_file, "%d#%d", n, e);
    fclose(public_key_file);

    FILE *private_key_file = fopen("chave.priv", "w");
    if (private_key_file == NULL) {
        printf("Erro ao abrir o arquivo da chave privada.\n");
        return 1;
    }

    fprintf(private_key_file, "%d#%d", n, d);
    fclose(private_key_file);

    printf("Chaves pública e privada geradas com sucesso e salvas nos arquivos 'chave.pub', 'chave.priv' e 'primos.txt'.\n");

    return 0;
}
