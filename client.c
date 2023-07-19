#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 2048

void send_file(FILE *fp, int sockfd, int size) {
  int n;
  char send_buffer[size];
  while(!feof(fp)) {
      fread(send_buffer, 1, sizeof(send_buffer), fp);
      write(sockfd, send_buffer, sizeof(send_buffer));
      bzero(send_buffer, sizeof(send_buffer));
  }
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    fprintf(stderr, "ERROR: Coloque o nome do arquivo a ser enviado! \n");
    return -1;
  }

  char *ip = "127.0.0.1";
  int port = 255666;
  int e;

  int sockfd;
  struct sockaddr_in server_addr;
  FILE *fp;
  char *filename = argv[1];

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (e == -1) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Connected to Server.\n");

  fp = fopen(filename, "rb");
  if (fp == NULL) {
    perror("[-]Error in reading file.");
    exit(1);
  }
  printf("[+]Opened file.\n");

  //Get Picture Size
  printf("[+]Getting Picture Size\n");
  int size;
  fseek(fp, 0, SEEK_END);
  printf("[+]FSEEK END\n");
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  //Send Picture Size
  printf("[+]Sending Picture Size\n");
  write(sockfd, &size, sizeof(size));

  send_file(fp, sockfd, size);
  printf("[+]File data sent successfully.\n");

  printf("[+]Closing the connection.\n");
  close(sockfd);

  return 0;
}
