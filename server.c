#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345  // Replace this with the port you want the server to listen on

// Function to receive BMP data over the TCP connection and save it to a file
int receive_bmp_over_tcp() {
    // Create a TCP socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Error creating socket");
        return -1;
    }

    // Set up the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the specified port
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        close(server_fd);
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 1) < 0) {
        perror("Listening failed");
        close(server_fd);
        return -1;
    }

    // Accept the incoming connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("Acceptance failed");
        close(server_fd);
        return -1;
    }

    // Create a file to save the received BMP data
    FILE* bmp_file = fopen("received.bmp", "wb");
    if (!bmp_file) {
        perror("Error creating file");
        close(client_fd);
        close(server_fd);
        return -1;
    }

    // Receive the BMP data and save it to the file
    char buffer[4096];
    ssize_t bytes_received;
    while ((bytes_received = read(client_fd, buffer, sizeof(buffer))) > 0) {
        if (fwrite(buffer, 1, bytes_received, bmp_file) != bytes_received) {
            perror("Error writing to file");
            fclose(bmp_file);
            close(client_fd);
            close(server_fd);
            return -1;
        }
    }

    fclose(bmp_file);
    close(client_fd);
    close(server_fd);
    return 0;
}

int main() {
    if (receive_bmp_over_tcp() == 0) {
        printf("BMP file received and saved as received.bmp.\n");
    } else {
        printf("Failed to receive BMP file.\n");
    }

    return 0;
}
