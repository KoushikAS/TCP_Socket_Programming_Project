#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <vector>

/**
citations:
    1) TCP example by Brian Rogers, updated by Rabih Younes. Duke University.
*/

int setUpSocketToConnect(char * host_name, struct addrinfo ** hosts, char * port) {
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));

  hints.ai_flags = AF_INET;         // To return address family from both IPV4 and IPV6.
  hints.ai_socktype = SOCK_STREAM;  // Connection based protocol (i.e. TCP).

  int status = getaddrinfo(host_name, port, &hints, hosts);
  if (status != 0) {
    std::cerr << "Error cannot get the addresses" << std::endl;
    exit(EXIT_FAILURE);
  }

  int socket_fd =
      socket((*hosts)->ai_family, (*hosts)->ai_socktype, (*hosts)->ai_protocol);
  if (socket_fd == -1) {
    std::cerr << "Error cannot create socket" << std::endl;
    exit(EXIT_FAILURE);
  }
  return socket_fd;
}

int setUpSocketToListen() {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  int yes = 1;
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  struct sockaddr_in my_addr;
  memset(&my_addr, 0, sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = 0;
  socklen_t len = sizeof(my_addr);

  int status = bind(socket_fd, (struct sockaddr *)&my_addr, len);
  if (status == -1) {
    std::cerr << "Error cannot bind socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  status = listen(socket_fd, 100);
  if (status == -1) {
    std::cerr << "Error cannot listen on socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  return socket_fd;
}

int main(int argc, char * argv[]) {
  if (argc < 3) {
    std::cerr << "Required no of arguments is not provided for this function"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  char * hostname = argv[1];
  char * port = argv[2];
  struct addrinfo * hosts;

  int ringmaster_socket = setUpSocketToConnect(hostname, &hosts, port);
  int player_socket = setUpSocketToListen();

  std::cout << "Connecting" << std::endl;

  if (connect(ringmaster_socket, hosts->ai_addr, hosts->ai_addrlen) == -1) {
    std::cerr << "Error Cannot connect to socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  //Send player's hostname to ringmaster
  char player_hostname[512];
  if (gethostname(player_hostname, 512) != 0) {
    std::cerr << "Error could not get hostname" << std::endl;
  }
  send(ringmaster_socket, player_hostname, 512, 0);

  // Send player's portto the the ring master.
  struct sockaddr_in playeraddr;
  memset(&playeraddr, 0, sizeof(playeraddr));
  socklen_t len = sizeof(playeraddr);
  getsockname(player_socket, (struct sockaddr *)&playeraddr, &len);
  const char * player_port = std::to_string(playeraddr.sin_port).c_str();
  send(ringmaster_socket, player_port, 512, 0);

  char buffer[512];
  recv(ringmaster_socket, buffer, 512, 0);
  std::cout << "Received  " << buffer << std::endl;

  freeaddrinfo(hosts);
  close(ringmaster_socket);
  close(player_socket);
  return EXIT_SUCCESS;
}
