# TCP Socket Programming Project
## ECE 650 – Spring 2023

### Overview
This project implements a multiplayer game of hot potato using TCP socket programming, Developed as part of ECE-650 Systems Program & Engineering, this individual project. The game consists of a ringmaster process and multiple player processes communicating in a ring topology. The goal is to pass a "potato" object among players until a randomly determined endpoint, at which the game concludes, and the player holding the potato is marked as "it".

### Features
- **Ringmaster Process:** Initiates the game, sends the potato to a random player, and concludes the game.
- **Player Processes:** Form a ring network where each player passes the potato to either neighbor.
- **TCP Sockets:** Used for communication between the ringmaster and players, and among players themselves.
- **Dynamic Player Interaction:** Players listen to multiple channels and make decisions based on game rules.
- **Game Trace Output:** Ringmaster prints a trace of the game showing the path of the potato.

### How to Run
1. **Ringmaster Program:** 
   ```bash
   ./ringmaster <port_num> <num_players> <num_hops>
   ```
   - Example: ./ringmaster 1234 3 100

2. **Player Program:** 
   ```bash
   ./player <machine_name> <port_num>
   ```
   - Example: ./player vcm-xxxx.vm.duke.edu 1234
   - machine_name is where the ringmaster process is running.

### Responsibilities
- **Ringmaster:**
  - Establishes socket connections with players.
  - Initiates and ends the game, reporting results.

- **Players:**
  - Establish socket connections for communication.
  - Continuously listen to channels for the potato or commands.
  - Handle game logic based on the number of hops remaining.

### Communication Mechanism
- TCP sockets facilitate communication between processes.
- Players are assigned IDs to track the potato's path.
- Ringmaster sends essential game information to each player.

### Output Specifications
- **Ringmaster's Output:**
  - Initial game setup details.
  - Connection acknowledgments from players.
  - Game start announcement and potato's path trace.

- **Player's Output:**
  - Confirmation of connection and player ID.
  - Notifications of sending the potato.
  - Announcement of being "it" if holding the potato at game end.



## Contributions

This project was completed as part of an academic assignment with requirments provided requirments.pdf. Contributions were made solely by Koushik Annareddy Sreenath, adhering to the project guidelines and requirements set by the course ECE-650 Systems Program & Engineering

## License

This project is an academic assignment and is subject to university guidelines on academic integrity and software use.

## Acknowledgments

- Thanks to Rabih Younes and the course staff for providing guidance and support throughout the project.
