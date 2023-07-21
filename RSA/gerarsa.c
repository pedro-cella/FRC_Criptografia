#include <stdio.h>
#include <stdlib.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <stdbool.h>

// Declare action parameters
#define ACTION_GEN_PRIMES "-p"
#define ACTION_GEN_KEYS "-k"

#define MIN_DIGITS 10000  // Mínimo de cinco dígitos
#define MAX_DIGITS 999999 // Máximo de seis dígitos

#define RSA_KEY_SIZE 4096


bool is_prime(unsigned long long  num) {
    if (num <= 1)
        return false;
    
    for (unsigned long long  i = 2; i * i <= num; i++) {
        if (num % i == 0)
            return false;
    }

    return true;
}

unsigned long long  generate_prime(unsigned long long  min, unsigned long long  max) {
    unsigned long long  num;
    do {
        num = rand() % (max - min + 1) + min;
    } while (!is_prime(num));
    return num;
}

void set_primes(unsigned long long  *p, unsigned long long  *q)
{

    *p = generate_prime(MIN_DIGITS, MAX_DIGITS);
    do {
        srand(time(NULL));
        *q = generate_prime(MIN_DIGITS, MAX_DIGITS);
    } while(*p == *q);


}


int main(int argc, char* argv[]) {

 if (argc < 2) {
        printf("[-]ERROR: É preciso passar uma das opções de execução:\n[-] -k: Gera novas chaves .pub e .priv\n[-] -p: Gera novos numeros primos.\n");
        exit(-1);
    }

    srand(time(NULL));

    clock_t start, finish;
    double time_taken;

    if (strcmp(argv[1], ACTION_GEN_PRIMES) == 0) {

        start = clock();

        unsigned long long  p, q;
        set_primes(&p, &q);
        // unsigned long long  p = generate_prime(MIN_DIGITS, MAX_DIGITS);
        // unsigned long long  q = generate_prime(MIN_DIGITS, MAX_DIGITS);
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

        FILE* primes = fopen(argv[2], "r");

        unsigned long long Tp, Tq;

        int f_return = fscanf(primes, "%lld#%lld", &Tp, &Tq);

        fclose(primes);

        printf("[+] Primos lidos:\n");
        printf("[+] p = %lld\n", Tp);
        printf("[+] q = %lld\n", Tq);

        // Initialize OpenSSL
        ERR_load_crypto_strings();
        OpenSSL_add_all_algorithms();
        OPENSSL_config(NULL);

        RSA *rsa_keypair = NULL;
        FILE *private_key_file = NULL;
        FILE *public_key_file = NULL;
        BIGNUM *e = NULL;
        BIGNUM *p = NULL;
        BIGNUM *q = NULL;
        int success = 0;

        /*
            The value e = 65537 comes from a cost-effectiveness compromise.
            A value of e that is too large increases the calculation times. A value of e that is too small increases the possibilities of attack.
                65357 is a Fermat which allows a simplification in the generation of prime numbers.
            This value has become a standard, it is not recommended to change it in the context of secure exchanges.
        */
        const char *base_primes_e = "65537";
        char *base_primes_p = (char*)malloc(8 * sizeof(char));
        char *base_primes_q = (char*)malloc(8 * sizeof(char));

        sprintf(base_primes_p, "%lld", Tp);
        sprintf(base_primes_q, "%lld", Tq);

        // Initialize BIGNUMs
        e = BN_new();
        p = BN_new();
        q = BN_new();

        if (e == NULL || p == NULL || q == NULL) {
            fprintf(stderr, "Error initializing BIGNUMs.\n");
            return 1;
        }

        // Set values for e, p, and q
        if (!BN_dec2bn(&e, base_primes_p) || !BN_dec2bn(&p, base_primes_p) || !BN_dec2bn(&q, base_primes_q)) {
            fprintf(stderr, "Error setting base primes p and q.\n");
            return 1;
        }

        // Generate RSA keypair with provided primes p and q
        rsa_keypair = RSA_new();
        if (rsa_keypair == NULL) {
            fprintf(stderr, "Error creating RSA structure.\n");
            return 1;
        }

        if (RSA_generate_key_ex(rsa_keypair, RSA_KEY_SIZE, e, NULL) != 1) {
            fprintf(stderr, "Error generating RSA keypair.\n");
            ERR_print_errors_fp(stderr);
            RSA_free(rsa_keypair);
            return 1;
        }

        // Save private key to file
        private_key_file = fopen("chave.priv", "wb");
        if (private_key_file == NULL) {
            perror("Error creating private_key.pem");
            RSA_free(rsa_keypair);
            return 1;
        }
        if (PEM_write_RSAPrivateKey(private_key_file, rsa_keypair, NULL, NULL, 0, NULL, NULL) != 1) {
            perror("Error writing private key to file");
            fclose(private_key_file);
            RSA_free(rsa_keypair);
            return 1;
        }
        printf("[+] Criado chave privada e salva em 'chave.priv'\n");
        fclose(private_key_file);

        // Save public key to file
        public_key_file = fopen("chave.pub", "wb");
        if (public_key_file == NULL) {
            perror("Error creating public_key.pem");
            RSA_free(rsa_keypair);
            return 1;
        }
        if (PEM_write_RSA_PUBKEY(public_key_file, rsa_keypair) != 1) {
            perror("Error writing public key to file");
            fclose(public_key_file);
            RSA_free(rsa_keypair);
            return 1;
        }
        printf("[+] Criado chave publica e salva em 'chave.pub'\n");
        fclose(public_key_file);

        // Cleanup and free resources
        RSA_free(rsa_keypair);
        BN_free(e);
        BN_free(p);
        BN_free(q);

        // Clean up OpenSSL
        EVP_cleanup();
        ERR_free_strings();

    }

    return 0;
}
