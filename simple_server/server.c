#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

void *connection_handler(void *);
char *get_name_by_current_time();

int main(int argc, char *argv[]) {
  int socket_desc, client_sock, c;
  struct sockaddr_in server, client;

  // Create socket
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1) {
    printf("[error]\tCould not create socket\n");
  }

  // Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(12345);

  // Bind
  if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
    printf("[error]\tBind failed\n");
    return 1;
  }

  // Listen
  listen(socket_desc, 128);  // backlog - size of queue

  // Accept and incoming connection
  printf("Waiting for incoming connections...\n");
  c = sizeof(struct sockaddr_in);
  pthread_t thread;

  while ((client_sock = accept(socket_desc, (struct sockaddr *)&client,
                               (socklen_t *)&c))) {
    printf("Connection accepted\n");
    printf("IP address is: %s\n", inet_ntoa(client.sin_addr));

    if (pthread_create(&thread, NULL, connection_handler,
                       (void *)&client_sock) < 0) {
      perror("[error]\tCould not create thread\n");
      return 1;
    }

    printf("Handler assigned\n");
  }

  if (client_sock < 0) {
    printf("[error]\tAccept failed");
    return 1;
  }

  return 0;
}

void *connection_handler(void *socket_desc) {
  // Get the socket descriptor
  int sock = *(int *)socket_desc;
  int read_size;
  int message_size = 16;
  char *message, client_message[message_size], *filename;

  filename = get_name_by_current_time();
  FILE *f = fopen(filename, "wb");
  free(filename);

  while ((read_size = recv(sock, client_message, message_size, 0)) > 0) {
    fprintf(f, client_message);
    memset(client_message, 0, message_size);
  }
  fclose(f);

  if (read_size == 0) {
    printf("Client disconnected\n");
    fflush(stdout);
  } else if (read_size == -1) {
    printf("[error]\tRecv failed\n");
  }

  return 0;
}

char *get_name_by_current_time() {
  int buffer_size = 100;
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  char *buff = (char *)malloc(sizeof(char) * buffer_size);
  memset(buff, 0, buffer_size);
  snprintf(buff, buffer_size, "%d-%d-%d-%d:%d:%d.tar.gz", tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

  return buff;
}