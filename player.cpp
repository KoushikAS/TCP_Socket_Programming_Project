#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <vector>

/**
citations:
    1) TCP example by Brian Rogers, updated by Rabih Younes. Duke University.
*/

int setUpSocket(char * host_name, struct addrinfo ** hosts, char * port) {
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));

  hints.ai_flags = AF_UNSPEC;       // To return address family from both IPV4 and IPV6.
  hints.ai_socktype = SOCK_STREAM;  // Connection based protocol (i.e. TCP).
  hints.ai_flags = AI_PASSIVE;      // Return Socket will be suitable for bind and accept.

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

  /**
  int yes = 1;
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  status = bind(socket_fd, (*hosts)->ai_addr, (*hosts)->ai_addrlen);
  if (status == -1) {
    std::cerr << "Error cannot bind socket" << std::endl;
    return EXIT_FAILURE;
  }

  status = listen(socket_fd, 100);
  if (status == -1) {
    std::cerr << "Error cannot listen on socket" << std::endl;
    return EXIT_FAILURE;
  }
  **/
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

  int ringmaster_socket = setUpSocket(hostname, &hosts, port);

  std::cout << "Connecting" << std::endl;

  int status = connect(ringmaster_socket, hosts->ai_addr, hosts->ai_addrlen);
  if (status == -1) {
    std::cerr << "Error Cannot connect to socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  const char * msg = "Server message";

  std::cout << msg << std::endl;
  send(ringmaster_socket, msg, strlen(msg), 0);

  freeaddrinfo(hosts);
  close(ringmaster_socket);

  return EXIT_SUCCESS;
}
