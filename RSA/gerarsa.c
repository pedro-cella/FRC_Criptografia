#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <gmp.h>

// Declare action parameters
#define ACTION_GEN_PRIMES "-p"
#define ACTION_GEN_KEYS "-k"

#define MIN_DIGITS 10000  // Mínimo de cinco dígitos
#define MAX_DIGITS 999999 // Máximo de seis dígitos

#define E_MAX 2023

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

bool is_prime(int num) {
    if (num <= 1)
        return false;
    
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0)
            return false;
    }

    return true;
}

void generate_prime(mpz_t *prime, int min, int max) {
    int num;
    do {
        num = rand() % (max - min + 1) + min;
    } while (!is_prime(num));
    printf("OLA\n");
    mpz_set_ui(prime, num);
    printf("OLA\n");

}

void set_primes(mpz_t p, mpz_t q)
{

    generate_prime(&p, MIN_DIGITS, MAX_DIGITS);
    do {
        srand(time(NULL));
       generate_prime(q, MIN_DIGITS, MAX_DIGITS);
    } while(mpz_cmp(p, q) == 0);

}


void set_n(mpz_t n, mpz_t  p, mpz_t  q){
    mpz_mul(n, p, q);
}


void set_z(mpz_t z, mpz_t p, mpz_t  q, mpz_t  n){
    
    mpz_t tmp;
    mpz_init(tmp);
    mpz_set_ui(tmp, 1);

    mpz_sub(tmp, n, p);
    mpz_sub(z, tmp, q);



    // Distributiva de (p - 1)(q - 1) = pq - p - q + 1 // p * q == n;
}


// mpz_t  get_e(mpz_t  z) {
//     mpz_t  e;
//     do {
//         srand(time(NULL));
//         e = rand() % min(E_MAX, z); // Garante q min < Z
//     } while (gcd(e, z) != 1);

//     return e;
// }


void set_d(mpz_t d, mpz_t e, mpz_t z) {
    mpz_t k, tmp, one, zero;
    mpz_init(k);
    mpz_set_ui(k,1);

    mpz_init(d);
    mpz_set_ui(d,0);

    mpz_init(tmp);
    mpz_set_ui(tmp,0);

    mpz_init(one);
    mpz_set_ui(one,1);
    mpz_init(zero);
    mpz_set_ui(zero,0);

    do {
        mpz_mul(tmp, k, z);
        mpz_add(tmp, tmp, one);
        mpz_mod(tmp, tmp, e);

        mpz_add(k, k, one);

    } while (mpz_cmp(tmp, zero) != 0);
    

//    while (((k * z)+1) % e != 0) {
//         k++;
//    }

    mpz_mul(tmp, k, z);
    mpz_add(tmp, tmp, one);
    mpz_div(d, tmp, e);

}

int  main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("[-]ERROR: É preciso passar uma das opções de execução:\n[-] -k: Gera novas chaves .pub e .priv\n[-] -p: Gera novos numeros primos.\n");
        exit(-1);
    }

    srand(time(NULL));

    clock_t start, finish;
    double time_taken;

    if (strcmp(argv[1], ACTION_GEN_PRIMES) == 0) {


        start = clock();

        mpz_t p, q;
        set_primes(p, q);
        // mpz_t  p = generate_prime(MIN_DIGITS, MAX_DIGITS);
        // mpz_t  q = generate_prime(MIN_DIGITS, MAX_DIGITS);
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

        mpz_t p;
        mpz_t q;

        FILE* primes = fopen(argv[2], "r");

        int f_return = fscanf(primes, "%lld#%lld", &p, &q);

        fclose(primes);

        printf("[+] Primos lidos:\n");
        gmp_printf("[+] p = %Zd\n", p);
        gmp_printf("[+] q = %Zd\n", q);

        printf("[+] Computando N...\n");
        mpz_t  n;
        mpz_init(n);
        set_n(n, p, q);
        gmp_printf("[+] N = %Zd\n\n", n);

        printf("[+] Computando Z...\n");
        mpz_t z;
        mpz_init(z);
        set_z(z, p, q, n);

        gmp_printf("[+] Z = %Zd\n\n", z);

        // printf("[+] Computando E...\n");
        /*
        The value e = 65537 comes from a cost-effectiveness compromise.
        A value of e that is too large increases the calculation times. A value of e that is too small increases the possibilities of attack.
            65357 is a Fermat which allows a simplification in the generation of prime numbers.
        This value has become a standard, it is not recommended to change it in the context of secure exchanges.
        */
        mpz_t  e;
        mpz_init(e);
        mpz_set_ui(e, 65537);

        gmp_printf("[+] E = %Zd\n\n", e);


        printf("[+] Computando D...\n");
        start = clock();
        mpz_t d;
        mpz_init(d);
        set_d(d, e, z);
        finish = clock();
        time_taken = (double)(finish - start) / (double)CLOCKS_PER_SEC;
        gmp_printf("[+] D = %Zd\n", d);
        printf("[+] D computado em %.2lfs.\n\n", time_taken);


        char* pub_filename = "generated_key.pub";
        char* priv_filename = "generated_key.priv";

        FILE *public_key_file = fopen(pub_filename, "w");
        if (public_key_file == NULL) {
            printf("[-] Erro ao abrir o arquivo da chave pública.\n");
            return 1;
        }

        fprintf(public_key_file, "%Zd@%Zd", n, e);
        fclose(public_key_file);

        printf("[+] Gerado o arquivo '%s'.\n", pub_filename);

        FILE *private_key_file = fopen(priv_filename, "w");
        if (private_key_file == NULL) {
            printf("[-] Erro ao abrir o arquivo da chave privada.\n");
            return 1;
        }

        fprintf(private_key_file, "%Zd@%Zd", n, d);
        fclose(private_key_file);

        printf("[+] Gerado o arquivo '%s'.\n", priv_filename);
        printf("[+] Chaves geradas com sucesso\n");
    }

    return 0;
}
