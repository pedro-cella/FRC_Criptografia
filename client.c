#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"  // Replace this with the server's IP address
#define SERVER_PORT 12345      // Replace this with the server's port

// Function to read the .bmp file and send it over the TCP connection
int send_bmp_over_tcp(const char* filename) {
    FILE* bmp_file = fopen(filename, "rb");
    if (!bmp_file) {
        perror("Error opening BMP file");
        return -1;
    }

    fseek(bmp_file, 0, SEEK_END);
    long file_size = ftell(bmp_file);
    rewind(bmp_file);

    char* buffer = (char*)malloc(file_size);
    if (!buffer) {
        perror("Memory allocation error");
        fclose(bmp_file);
        return -1;
    }

    if (fread(buffer, 1, file_size, bmp_file) != file_size) {
        perror("Error reading BMP file");
        free(buffer);
        fclose(bmp_file);
        return -1;
    }

    fclose(bmp_file);

    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        free(buffer);
        return -1;
    }

    // Set up the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid server IP address");
        free(buffer);
        close(sockfd);
        return -1;
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        free(buffer);
        close(sockfd);
        return -1;
    }

    // Send the BMP data
    if (write(sockfd, buffer, file_size) != file_size) {
        perror("Error sending BMP data");
        free(buffer);
        close(sockfd);
        return -1;
    }

    free(buffer);
    close(sockfd);
    return 0;
}

int main(int argc, char *argv[]) {
    const char* bmp_filename = argv[1]; // Replace this with the actual .bmp filename
    if (send_bmp_over_tcp(bmp_filename) == 0) {
        printf("BMP file sent successfully.\n");
    } else {
        printf("Failed to send BMP file.\n");
    }

    return 0;
}
