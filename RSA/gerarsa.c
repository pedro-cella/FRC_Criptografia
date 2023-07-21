#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Declare action parameters
#define ACTION_GEN_PRIMES "-p"
#define ACTION_GEN_KEYS "-k"

#define MIN_DIGITS 10000  // Mínimo de cinco dígitos
#define MAX_DIGITS 99999 // Máximo de cinco dígitos

bool is_prime(unsigned long long num) {
    if (num <= 1)
        return false;
    
    for (unsigned long long i = 2; i * i <= num; i++) {
        if (num % i == 0)
            return false;
    }

    return true;
}

unsigned long long generate_prime(unsigned long long min, unsigned long long max) {
    unsigned long long num;
    do {
        num = rand() % (max - min + 1) + min;
    } while (!is_prime(num));
    return num;
}

unsigned long long gcd(unsigned long long a, unsigned long long b) {
    while (b != 0) {
        unsigned long long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

unsigned long long compute_n(unsigned long long p, unsigned long long q) {
    return p * q;
}

unsigned long long compute_z(unsigned long long p, unsigned long long q) {
    return (p - 1) * (q - 1);
}

unsigned long long choose_e(unsigned long long z) {
    unsigned long long e;
    do {
        e = rand() % (z - 2) + 2; // Começa em 2 para garantir que e < z
    } while (gcd(e, z) != 1);

    return e;
}


unsigned long long choose_d(unsigned long long e, unsigned long long z) {
   unsigned long long d = 1;

   while ((d * e) % z != 1) {
    d++;
   }

   return d;
   

}

unsigned long long main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("[-]ERROR: É preciso passar uma das opções de execução:\n[-] -k: Gera novas chaves .pub e .priv\n[-] -p: Gera novos numeros primos.\n");
        exit(-1);
    }

    srand(time(NULL));

    clock_t start, finish;
    double time_taken;

    if (strcmp(argv[1], ACTION_GEN_PRIMES) == 0) {


        start = clock();
        unsigned long long p = generate_prime(MIN_DIGITS, MAX_DIGITS);
        unsigned long long q = generate_prime(MIN_DIGITS, MAX_DIGITS);
        finish = clock();
        time_taken = (double)(finish - start) / (double)CLOCKS_PER_SEC;

        printf("[+] Números primos gerados:\n");
        printf("[+] p = %lld\n", p);
        printf("[+] q = %lld\n", q);
        printf("[+] Primos gerados em %.10lfs.\n", time_taken);

        FILE *primes_file = fopen("primos.txt", "w");
        if (primes_file == NULL) {
            printf ("Erro ao abrir o arquivo 'primos.txt'.\n");
            return 1;
        }

        fprintf(primes_file, "%lld#%lld", p, q);
        fclose(primes_file);

    } else if (strcmp(argv[1], ACTION_GEN_KEYS) == 0) {
        if(argc > 3 || argc < 3){

            printf("[-] Opção -k precisa de 1 argumento: caminho do arquivo de numero primos.\n");
            exit(-1);

        }

        unsigned long long p;
        unsigned long long q;

        FILE* primes = fopen(argv[2], "r");

        int f_return = fscanf(primes, "%lld#%lld", &p, &q);

        fclose(primes);

        printf("[+] Primos lidos:\n");
        printf("[+] p = %lld\n", p);
        printf("[+] q = %lld\n", q);

        printf("[+] Computando N...\n");
        unsigned long long n = compute_n(p, q);
        printf("[+] N = %lld\n\n", n);

        printf("[+] Computando Z...\n");
        unsigned long long z = compute_z(p, q);
        printf("[+] Z = %lld\n\n", z);

        printf("[+] Computando E...\n");
        unsigned long long e = choose_e(z);
        printf("[+] E = %lld\n\n", e);


        printf("[+] Computando D...\n");
        start = clock();
        unsigned long long d = choose_d(e, z);
        finish = clock();
        time_taken = (double)(finish - start) / (double)CLOCKS_PER_SEC;
        printf("[+] D = %lld\n", d);
        printf("[+] D computado em %.2lfs.\n\n", time_taken);


        char* pub_filename = "generated_key.pub";
        char* priv_filename = "generated_key.priv";

        FILE *public_key_file = fopen(pub_filename, "w");
        if (public_key_file == NULL) {
            printf("[-] Erro ao abrir o arquivo da chave pública.\n");
            return 1;
        }

        fprintf(public_key_file, "%lld@%lld", n, e);
        fclose(public_key_file);

        printf("[+] Gerado o arquivo '%s'.\n", pub_filename);

        FILE *private_key_file = fopen(priv_filename, "w");
        if (private_key_file == NULL) {
            printf("[-] Erro ao abrir o arquivo da chave privada.\n");
            return 1;
        }

        fprintf(private_key_file, "%lld@%lld", n, d);
        fclose(private_key_file);

        printf("[+] Gerado o arquivo '%s'.\n", priv_filename);
        printf("[+] Chaves geradas com sucesso\n");
    }

    return 0;
}
