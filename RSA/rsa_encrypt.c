#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/des.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define RSA_KEY_SIZE 4096
#define CHUNK_SIZE 256

#define SERVER_IP "127.0.0.1"  // Replace this with the server's IP address
#define SERVER_PORT 25522      // Replace this with the server's port

char* encrypt(char* chave_pub_filename, char* input_filename) {
  // Initialize OpenSSL
  ERR_load_crypto_strings();
  OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CONFIG, NULL);
  OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_CIPHERS, NULL);

  // Load the public key from the .pem file
  FILE* public_key_file = fopen(chave_pub_filename, "rb");
  if (public_key_file == NULL) {
    perror("Error opening public_key.pem");
    exit(-1);
  }

  printf("[+] Reded file %s\n", chave_pub_filename);

  RSA* rsa_keypair = RSA_new();
  if (rsa_keypair == NULL) {
    perror("Error creating RSA structure");
    fclose(public_key_file);
    exit(-1);
  }

  rsa_keypair = PEM_read_RSA_PUBKEY(public_key_file, NULL, NULL, NULL);
  fclose(public_key_file);

  if (rsa_keypair == NULL) {
    perror("Error reading public key from file");
    exit(-1);
  }

  // Read the binary file (e.g., .bmp) to encrypt
  FILE* input_file = fopen(input_filename, "rb");
  if (input_file == NULL) {
    perror("Error opening input.bmp");
    RSA_free(rsa_keypair);
    exit(-1);
  }

  // Create the output file for the encrypted data
  FILE* output_file = fopen("encrypted_output.dat", "wb");
  if (output_file == NULL) {
    perror("Error creating encrypted_output.dat");
    fclose(input_file);
    RSA_free(rsa_keypair);
    exit(-1);
  }

  unsigned char plaintext[CHUNK_SIZE];
  unsigned char ciphertext[RSA_size(rsa_keypair)];

  int bytes_read, encrypted_len;
  while ((bytes_read = fread(plaintext, sizeof(unsigned char), CHUNK_SIZE,
                             input_file)) > 0) {
    encrypted_len = RSA_public_encrypt(bytes_read, plaintext, ciphertext,
                                       rsa_keypair, RSA_PKCS1_PADDING);
    if (encrypted_len == -1) {
      perror("Error encrypting data\n");
      fclose(input_file);
      fclose(output_file);
      RSA_free(rsa_keypair);
      exit(-1);
    }
    fwrite(ciphertext, sizeof(unsigned char), encrypted_len, output_file);
  }

  // Close the files
  fclose(input_file);
  fclose(output_file);

  RSA_free(rsa_keypair);

  // Clean up OpenSSL
  OPENSSL_cleanup();
  ERR_free_strings();

  return "encrypted_output.dat";
}

char* recive_key(int server_fd) {
  // Recive file size.
  ssize_t bytes_received;

  char* key_file = (char*)malloc(sizeof(char) * 4000);
  bytes_received = recv(server_fd, key_file, 4000, 0);

  if (bytes_received < 500) {
    printf("[-] Key recived was invalid. key: %s\n", key_file);
    printf("[-] Errno:  %d\n", errno);
    exit(-1);
  }

  FILE* pub_key_file = fopen("server.pub", "wb");
  fwrite(key_file, sizeof(char), bytes_received, pub_key_file);
  fclose(pub_key_file);

  return "server.pub";
}

void send_ecrypted_bmp(int sockect_fd, char* encrypted_filename) {
  ssize_t bytes_received;

  FILE* encrypted_file = fopen(encrypted_filename, "rb");
  printf("[+] Readed file: %s\n", encrypted_filename);

  fseek(encrypted_file, 0, SEEK_END);
  long file_size = ftell(encrypted_file);
  rewind(encrypted_file);

  char* file_size_buffer = (char*)malloc(sizeof(char) * 500);
  bzero(file_size_buffer, sizeof(char) * 500);

  snprintf(file_size_buffer, 500, "%lld", file_size);

  printf("File size: %s\n", file_size_buffer);

  send(sockect_fd, file_size_buffer, sizeof(char) * 500, 0);

  char* encrypted_file_buffer = (char*)malloc(sizeof(char) * file_size);
  fread(encrypted_file_buffer, sizeof(char), file_size, encrypted_file);
  // write(sockect_fd, encrypted_file_buffer, sizeof(char));
  if (write(sockect_fd, encrypted_file_buffer, file_size) != file_size) {
    printf("Error sending Encrypted BMP data\n");
    printf("[-] Errno:  %d\n", errno);
    fprintf( stderr, "%s\n", strerror( errno ));
    free(encrypted_file_buffer);
    close(sockect_fd);
    return -1;
  }
  // recv(sockect_fd, wait, 1, 0);

  char* wait = (char*)malloc(sizeof(char));
  recv(sockect_fd, wait, 1, 0);
}

int main(int argc, char* argv[]) {
  // Create a TCP socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("Error creating socket\n");
    return -1;
  }

  // Set up the server address
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);
  if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
    printf("Invalid server IP address\n");
    close(sockfd);
    return -1;
  }

  int client_socket =
      connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

  // Connect to the server
  if (client_socket < 0) {
    printf("Connection failed\n");
    close(sockfd);
    return -1;
  }

  char* encrypted_filename = encrypt(recive_key(sockfd), argv[1]);
  send_ecrypted_bmp(sockfd, encrypted_filename);

  return 0;
}
