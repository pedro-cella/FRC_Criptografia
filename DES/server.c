#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <openssl/des.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "des.h"

// Declare action parameters
#define ACTION_GENERATE_KEY "-g"
#define ACTION_ENCRYPT "-e"
#define ACTION_DECRYPT "-d"

// DES key is 8 bytes long
#define DES_KEY_SIZE 8

#define SERVER_PORT 25569  // Mude esse define para definir a porta do servidor.

int decrypt(char* des_key, char* filename) {
  FILE* encrypted_file = fopen(filename, "rb");
  printf("[+] Readed file: %s\n", filename);

  fseek(encrypted_file, 0, SEEK_END);
  long file_size = ftell(encrypted_file);
  rewind(encrypted_file);

  printf("[+] Encrypted file size: %ld\n", file_size);

  clock_t start, finish;
  double time_taken;
  unsigned short int padding;
  static FILE* output_file;
  // Open output file
  output_file = fopen("julia_decripted.bmp", "wb");
  if (!output_file) {
    printf("Could not open output file to write data.");
    exit(-1);
  }

  printf("[+] Opned decripted.\n");

  // Generate DES key set
  short int bytes_written, process_mode;
  unsigned long block_count = 0, number_of_blocks;
  unsigned char* data_block = (unsigned char*)malloc(8 * sizeof(char));
  unsigned char* processed_block = (unsigned char*)malloc(8 * sizeof(char));
  key_set* key_sets = (key_set*)malloc(17 * sizeof(key_set));

  start = clock();
  generate_sub_keys(des_key, key_sets);
  finish = clock();
  time_taken = (double)(finish - start) / (double)CLOCKS_PER_SEC;

  printf("[+] Decrypting..\n");

  number_of_blocks = file_size / 8 + ((file_size % 8) ? 1 : 0);
  start = clock();

  // Start reading input file, process and write to output file
  while (fread(data_block, 1, 8, encrypted_file)) {
    block_count++;
    if (block_count == number_of_blocks) {
      process_message(data_block, processed_block, key_sets, DECRYPTION_MODE);
      padding = processed_block[7];

      if (padding < 8) {
        bytes_written = fwrite(processed_block, 1, 8 - padding, output_file);
      }
    } else {
      process_message(data_block, processed_block, key_sets, DECRYPTION_MODE);
      bytes_written = fwrite(processed_block, 1, 8, output_file);
    }
    memset(data_block, 0, 8);
  }

  finish = clock();

  // Free up memory
  free(data_block);
  free(processed_block);
  fclose(encrypted_file);
  fclose(output_file);

  // Provide feedback
  time_taken = (double)(finish - start) / (double)CLOCKS_PER_SEC;
  printf("[+] Finished processing. Time taken: %lf seconds.\n", time_taken);
  return 0;
}

void recive_key(int client_fd, char* key) {
  ssize_t bytes_received;
  bytes_received = recv(client_fd, key, sizeof(key), 0);

  printf("[+] Bytes recived: %ld\n", bytes_received);

  if (bytes_received < 8) {
    printf("[-] Key recived was invalid. key: %s\n", key);
    printf("[-] Errno:  %d\n", errno);
    exit(-1);
  }
}

// Function to receive BMP data over the TCP connection and save it to a file
int receive_bmp_over_tcp(int client_fd) {
  // Create a file to save the received BMP data
  FILE* encrypted_file = fopen("encrypted_file.ecry", "wb");
  if (!encrypted_file) {
    printf("Error creating file");
    close(client_fd);
    return -1;
  }

  char key[8];
  bzero(key, 8);
  recive_key(client_fd, key);
  printf("[+] Key recived: %s\n", key);
  printf("[+] Created file.\n");
  // Receive the BMP data and save it to the file
  char buffer[4096];
  ssize_t bytes_received;
  while ((bytes_received = read(client_fd, buffer, sizeof(buffer))) > 0) {
    if (fwrite(buffer, 1, bytes_received, encrypted_file) != bytes_received) {
      printf("Error writing to file");
      fclose(encrypted_file);
      close(client_fd);
      return -1;
    }
  }

  fclose(encrypted_file);

  decrypt(key, "encrypted_file.ecry");

  return 0;
}

int main(int argc, char* argv[]) {
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
  int client_fd =
      accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
  if (client_fd < 0) {
    printf("Acceptance failed\n");
    close(server_fd);
    return -1;
  }

  receive_bmp_over_tcp(client_fd);

  close(client_fd);
  close(server_fd);

  return 0;
}
