#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 2048

void write_file(int sockfd, int size) {
  int n;
  FILE *fp;
  char *filename = "recived_file.bmp";

  printf("Reading Picture Byte Array\n");
  char p_array[size];
  read(sockfd, p_array, size);

  //Convert it Back into Picture
  printf("Converting Byte Array to Picture\n");
  FILE *image;
  image = fopen("c1.bmp", "w");
  fwrite(p_array, 1, sizeof(p_array), image);
  fclose(image);
  return;
}

int main() {
  char *ip = "127.0.0.1";
  int port = 255666;
  int e;

  int sockfd, new_sock;
  struct sockaddr_in server_addr, new_addr;
  socklen_t addr_size;
  char buffer[SIZE];


  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (e < 0) {
    perror("[-]Error in bind");
    exit(1);
  }
  printf("[+]Binding successfull.\n");

  if (listen(sockfd, 10) == 0) {
    printf("[+]Listening....\n");
  } else {
    perror("[-]Error in listening");
    exit(1);
  }

  addr_size = sizeof(new_addr);
  new_sock = accept(sockfd, (struct sockaddr *)&new_addr, &addr_size);

  printf("Reading Picture Size\n");
  int size;
  read(new_sock, &size, sizeof(int));
  printf("[+]Reading file of size: %d\n", size);
  write_file(new_sock, size);
  printf("[+]Data written in the file successfully.\n");

  return 0;
}