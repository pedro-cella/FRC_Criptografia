#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/des.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/*
 * des.h provides the following functions and constants:
 *
 * generate_key, generate_sub_keys, process_message, ENCRYPTION_MODE,
 * DECRYPTION_MODE
 *
 */
#include "des.h"

// Declare action parameters
#define ACTION_GENERATE_KEY "-g"
#define ACTION_ENCRYPT "-e"
#define ACTION_DECRYPT "-d"

// DES key is 8 bytes long
#define DES_KEY_SIZE 8

#define SERVER_IP "127.0.0.1"  // Replace this with the server's IP address
#define SERVER_PORT 25569      // Replace this with the server's port

char* gen_key() {
  unsigned int iseed = (unsigned int)time(NULL);
  srand(iseed);

  short int bytes_written;
  unsigned char* des_key = (unsigned char*)malloc(8 * sizeof(char));

  generate_key(des_key);

  printf("[+] Generated key: %s\n", des_key);

  return des_key;
}

char* encrypt(int client_fd, FILE* bmp_file) {
  fseek(bmp_file, 0, SEEK_END);
  long file_size = ftell(bmp_file);
  rewind(bmp_file);

  char* filename = "julia_encrypted";

  clock_t start, finish;
  double time_taken;
  unsigned short int padding;
  static FILE* output_file;
  // Open output file
  output_file = fopen(filename, "wb");
  if (!output_file) {
    printf("Could not open output file to write data.");
    return 1;
  }

  // Generate DES key set
  short int bytes_written, process_mode;
  unsigned long block_count = 0, number_of_blocks;
  unsigned char* data_block = (unsigned char*)malloc(8 * sizeof(char));
  unsigned char* processed_block = (unsigned char*)malloc(8 * sizeof(char));
  key_set* key_sets = (key_set*)malloc(17 * sizeof(key_set));

  char* des_key = gen_key();

  if (send(client_fd, des_key, strlen(des_key), 0) != 8) {
    printf("Error while sending the key.\n");
    exit(-1);
  }

  printf("[+] Sended key: %s\n", des_key);
  printf("[+] Strlen key: %d\n", strlen(des_key));

  start = clock();
  generate_sub_keys(des_key, key_sets);
  finish = clock();
  time_taken = (double)(finish - start) / (double)CLOCKS_PER_SEC;

  printf("Encrypting..\n");

  number_of_blocks = file_size / 8 + ((file_size % 8) ? 1 : 0);
  start = clock();

  //! TODO: Pular o header do bmp. talvez com fread e file seek.
  // Start reading input file, process and write to output file
  while (fread(data_block, 1, 8, bmp_file)) {
    block_count++;
    if (block_count == number_of_blocks) {
      padding = 8 - file_size % 8;
      if (padding < 8) {  // Fill empty data block bytes with padding
        memset((data_block + 8 - padding), (unsigned char)padding, padding);
      }

      process_message(data_block, processed_block, key_sets, ENCRYPTION_MODE);
      bytes_written = fwrite(processed_block, 1, 8, output_file);

      if (padding == 8) {  // Write an extra block for padding
        memset(data_block, (unsigned char)padding, 8);
        process_message(data_block, processed_block, key_sets, ENCRYPTION_MODE);
        bytes_written = fwrite(processed_block, 1, 8, output_file);
      }
    } else {
      process_message(data_block, processed_block, key_sets, ENCRYPTION_MODE);
      bytes_written = fwrite(processed_block, 1, 8, output_file);
    }
    memset(data_block, 0, 8);
  }

  finish = clock();

  // Free up memory
  free(des_key);
  free(data_block);
  free(processed_block);
  fclose(bmp_file);
  fclose(output_file);

  // Provide feedback
  time_taken = (double)(finish - start) / (double)CLOCKS_PER_SEC;
  printf("Finished processing. Time taken: %lf seconds.\n", time_taken);

  return filename;
}

// Function to read the .bmp file and send it over the TCP connection
int send_over_tcp_encrypted_bmp(int sockfd, const char* filename) {
  FILE* bmp_file = fopen(filename, "rb");
  if (!bmp_file) {
    printf("Error opening BMP file\n");
    return -1;
  }

  FILE* encrypted_file;
  encrypted_file = fopen(encrypt(sockfd, bmp_file), "rb");

  fseek(encrypted_file, 0, SEEK_END);
  long encrypted_file_size = ftell(encrypted_file);
  rewind(encrypted_file);

  char* encrypted_file_buffer = (char*)malloc(encrypted_file_size);
  if (!encrypted_file_buffer) {
    printf("Memory allocation error\n");
    fclose(encrypted_file_buffer);
    return -1;
  }

  if (fread(encrypted_file_buffer, 1, encrypted_file_size, encrypted_file) !=
      encrypted_file_size) {
    printf("Error reading Encrypted BMP file\n");
    free(encrypted_file_buffer);
    fclose(encrypted_file);
    return -1;
  }

  printf("[+] Encrypted mensage size: %ld\n", encrypted_file_size);

  // Send the BMP data
  if (write(sockfd, encrypted_file_buffer, encrypted_file_size) !=
      encrypted_file_size) {
    printf("Error sending Encrypted BMP data\n");
    free(encrypted_file_buffer);
    close(sockfd);
    return -1;
  }

  free(encrypted_file_buffer);
  close(sockfd);
  return 0;
}

int main(int argc, char* argv[]) {
  // Pre-shared key (PSK) for both client and server (must be 8 bytes long for
  // DES)

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

  send_over_tcp_encrypted_bmp(sockfd, argv[1]);
  close(client_socket);

  return 0;
}
