#include <stdio.h>
#include <stdlib.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/des.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define RSA_KEY_SIZE 4096
#define CHUNK_SIZE 256

#define SERVER_PORT 25569  // Replace this with the server's port

char* decrypt(char* chave_priv_filename){
    
    // Initialize OpenSSL
    ERR_load_crypto_strings();
    OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CONFIG, NULL);
    OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_CIPHERS, NULL);

    // Load the private key from the .pem file
    FILE *private_key_file = fopen(chave_priv_filename, "rb");
    if (private_key_file == NULL) {
        perror("Error opening private_key.pem");
        return 1;
    }

    RSA *rsa_keypair = RSA_new();
    if (rsa_keypair == NULL) {
        perror("Error creating RSA structure");
        fclose(private_key_file);
        return 1;
    }

    rsa_keypair = PEM_read_RSAPrivateKey(private_key_file, NULL, NULL, NULL);
    fclose(private_key_file);

    if (rsa_keypair == NULL) {
        perror("Error reading private key from file");
        return 1;
    }

    // Read the encrypted file
    FILE *encrypted_file = fopen("encrypted_output.dat", "rb");
    if (encrypted_file == NULL) {
        perror("Error opening encrypted_output.dat");
        RSA_free(rsa_keypair);
        return 1;
    }

    // Create the output file for the decrypted data (e.g., .bmp)
    FILE *decrypted_file = fopen("decrypted_output.bmp", "wb");
    if (decrypted_file == NULL) {
        perror("Error creating decrypted_output.bmp");
        fclose(encrypted_file);
        RSA_free(rsa_keypair);
        return 1;
    }

    unsigned char ciphertext[RSA_size(rsa_keypair)];
    unsigned char decrypted[CHUNK_SIZE];

    int bytes_read, decrypted_len;
    while ((bytes_read = fread(ciphertext, sizeof(unsigned char), RSA_size(rsa_keypair), encrypted_file)) > 0) {
        decrypted_len = RSA_private_decrypt(bytes_read, ciphertext, decrypted, rsa_keypair, RSA_PKCS1_PADDING);
        if (decrypted_len == -1) {
            perror("Error decrypting data");
            fclose(encrypted_file);
            fclose(decrypted_file);
            RSA_free(rsa_keypair);
            return 1;
        }
        fwrite(decrypted, sizeof(unsigned char), decrypted_len, decrypted_file);
    }

    // Close the files
    fclose(encrypted_file);
    fclose(decrypted_file);

    RSA_free(rsa_keypair);

    // Clean up OpenSSL
    OPENSSL_cleanup();
    ERR_free_strings();

    return "decrypted_output.bmp";

}


int send_key(int client_fd, char* chave_pub_filename){

    ssize_t bytes_received;

    FILE* chave_pub_file = fopen(chave_pub_filename, "rb");
    printf("[+] Readed file: %s\n", chave_pub_filename);

    fseek(chave_pub_file, 0, SEEK_END);
    long file_size = ftell(chave_pub_file);
    rewind(chave_pub_file);

    char* key_len = (char*)malloc(sizeof(char) * 5);
    bzero(key_len, sizeof(char) * 5);

    snprintf(key_len, 5, "%lld",file_size);

    printf("[+] Key len: %s\n", key_len);

    printf("%d\n", strlen(key_len));

    send(client_fd, key_len, strlen(key_len), 0);
    char* recived = (char*)malloc(sizeof(char) * 5);;
    bzero(recived, 5);
    bytes_received = read(client_fd, recived, 5);

    printf("%s\n", recived);


    char* pub_file_buffer = (char*)malloc(sizeof(char) * file_size);
    fread(pub_file_buffer, sizeof(char), file_size, chave_pub_file);

    send(client_fd, pub_file_buffer, sizeof(char) * file_size, 0);

}

int main() {

      // Pre-shared key (PSK) for both client and server (must be 8 bytes long for
  // DES) Create a TCP socket
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    printf("Error creating socket\n");
    return -1;
  }

  // Set up the server address
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(SERVER_PORT);

  // Bind the socket to the specified port
  if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
      0) {
    printf("Binding failed\n");
    close(server_fd);
    return -1;
  }

  // Listen for incoming connections
  if (listen(server_fd, 1) < 0) {
    printf("Listening failed\n");
    close(server_fd);
    return -1;
  }

  // Accept the incoming connection
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
  if (client_fd < 0) {
    printf("Acceptance failed\n");
    close(server_fd);
    return -1;
  }

    send_key(client_fd, "chave.pub");




    return 0;
}
// Enviar tamanho do arquivo dps o arquivo.