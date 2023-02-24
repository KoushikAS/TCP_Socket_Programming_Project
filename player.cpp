#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

#include "potato.hpp"
/**
citations:
    1) TCP example by Brian Rogers, updated by Rabih Younes. Duke University.
*/

class playerClass {
 public:
  char hostName[512];
  char port[512];
  int playerNo;
};

int setUpSocketToConnect(char * host_name, char * port) {
  struct addrinfo hints;
  struct addrinfo * hosts;

  memset(&hints, 0, sizeof(hints));

  hints.ai_flags = AF_INET;         // To return address family from both IPV4 and IPV6.
  hints.ai_socktype = SOCK_STREAM;  // Connection based protocol (i.e. TCP).

  int status = getaddrinfo(host_name, port, &hints, &hosts);
  if (status != 0) {
    std::cerr << "Error cannot get the addresses" << std::endl;
    exit(EXIT_FAILURE);
  }

  int socket_fd = socket(hosts->ai_family, hosts->ai_socktype, hosts->ai_protocol);
  if (socket_fd == -1) {
    std::cerr << "Error cannot create socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (connect(socket_fd, hosts->ai_addr, hosts->ai_addrlen) == -1) {
    std::cerr << "Error Cannot connect to socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(hosts);
  return socket_fd;
}

int setUpSocketToListen() {
  int socket_fd = socket(PF_INET, SOCK_STREAM, 0);

  if (socket_fd == -1) {
    std::cerr << "Error  cannot create socket" << std::endl;
    exit(EXIT_FAILURE);
  }

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

void threadToListen(int main_socketFd,
                    playerClass leftPlayer,
                    playerClass rightPlayer,
                    playerClass ringMaster,
                    int currPlayerNo) {
  while (true) {
    int socketFd = accept(main_socketFd, NULL, NULL);

    if (socketFd == -1) {
      std::cerr << "Error cannot accept connection on socket" << std::endl;
      exit(EXIT_FAILURE);
    }

    potato p[512];
    recv(socketFd, p, 512, 0);

    //ShutDown
    if (p->hops_left < 0) {
      break;
    }

    p->hops_left--;

    std::string tmp;
    if (p->hops_left != 0) {
      tmp = std::to_string(currPlayerNo) + ",";
    }
    else {
      tmp = std::to_string(currPlayerNo);
    }
    std::strcat(p->trace, tmp.c_str());

    int socketToSend;

    if (p->hops_left > 0) {
      if (rand() % 2 == 0) {
        //left
        std::cout << "Sending potato to " << leftPlayer.playerNo << std::endl;
        socketToSend = setUpSocketToConnect(leftPlayer.hostName, leftPlayer.port);
      }
      else {
        //right
        std::cout << "Sending potato to " << rightPlayer.playerNo << std::endl;
        socketToSend = setUpSocketToConnect(rightPlayer.hostName, rightPlayer.port);
      }
    }
    else {
      std::cout << "I’m it" << std::endl;
      socketToSend = setUpSocketToConnect(ringMaster.hostName, ringMaster.port);
    }

    send(socketToSend, p, 512, 0);

    close(socketToSend);
    close(socketFd);
  }

  return;
}

int main(int argc, char * argv[]) {
  if (argc < 3) {
    std::cerr << "Required no of arguments is not provided for this function"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  char * hostname = argv[1];
  char * port = argv[2];

  int player_socket = setUpSocketToListen();
  int ringmaster_socket = setUpSocketToConnect(hostname, port);

  //Send player's hostname to ringmaster
  char player_hostname[512];
  if (gethostname(player_hostname, 512) != 0) {
    std::cerr << "Error could not get hostname" << std::endl;
    exit(EXIT_FAILURE);
  }
  send(ringmaster_socket, player_hostname, 512, 0);

  // Send player's portto the the ring master.
  struct sockaddr_in playeraddr;
  memset(&playeraddr, 0, sizeof(playeraddr));
  socklen_t len = sizeof(playeraddr);
  if (getsockname(player_socket, (struct sockaddr *)&playeraddr, &len) == -1) {
    std::cerr << "Error cannot get the portname" << std::endl;
    exit(EXIT_FAILURE);
  }
  const char * player_port = std::to_string(ntohs(playeraddr.sin_port)).c_str();

  send(ringmaster_socket, player_port, 512, 0);

  //Receving Left Player
  playerClass leftPlayer;

  recv(ringmaster_socket, leftPlayer.hostName, 512, 0);
  recv(ringmaster_socket, leftPlayer.port, 512, 0);

  //Receving Right Player
  playerClass rightPlayer;

  recv(ringmaster_socket, rightPlayer.hostName, 512, 0);
  recv(ringmaster_socket, rightPlayer.port, 512, 0);

  //Receving Player Name Info
  char playerNo[512];
  recv(ringmaster_socket, playerNo, 512, 0);

  //Receving Total No of Players Info
  char no_players[512];
  recv(ringmaster_socket, no_players, 512, 0);

  int currPlayerNo = atoi(playerNo);
  int totalplayers = atoi(no_players);
  leftPlayer.playerNo = (totalplayers + (currPlayerNo - 1) % totalplayers) % totalplayers;
  rightPlayer.playerNo = (currPlayerNo + 1) % totalplayers;

  playerClass ringmaster;
  std::strcpy(ringmaster.hostName, hostname);
  std::strcpy(ringmaster.port, port);

  // create a seperate thread to listen for potatoes
  std::thread t1(
      threadToListen, player_socket, leftPlayer, rightPlayer, ringmaster, currPlayerNo);

  std::cout << "Connected as player " << playerNo << " out of " << no_players
            << " total players" << std::endl;

  const char * msg = "OK";
  send(ringmaster_socket, msg, 512, 0);

  t1.join();
  close(ringmaster_socket);
  close(player_socket);
  return EXIT_SUCCESS;
}
