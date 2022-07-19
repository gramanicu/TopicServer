# Topic Server - [![Build Status](https://travis-ci.com/gramanicu/TopicServer.svg?token=bpyWbq9HzbqLxtGzRHpD&branch=master)](https://travis-ci.com/gramanicu/TopicServer) ![Unit-Tests](https://github.com/gramanicu/TopicServer/workflows/Unit-Tests/badge.svg) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/bd76c224f71142bc82f27c42864a6d91)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=gramanicu/TopicServer&amp;utm_campaign=Badge_Grade) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

This is a server/client application that uses tcp/udp connections to receive/send messages on different topics. A UDP client sends messages on a specific topic, and the server forwards the messages to the TCP clients subscribed to that specific topic. This is the 2nd Communication Protocols Course homework. The problem statement / application basic design is described [here (Romanian)](./docs/problem_statement.pdf).

## Project structure

- src/
  - Server - manages all conections and data
  - Subscriber - the client program
  - Filesystem - a backend utility, that creates the files used by the server to store data
  - Database - the system that manages all the data used by the server: users, messages, etc.
  - User - a class that stores different user-related data
  - Topic - a class that stores different topic-related data
  - Utils - this header is included in all other files, as it contains different macros, functions, data-types, and it includes most of the libraries that are used by the other files.
- data/ - in this folder, all the messages received by the server will be stored
- docs/ - in this folder are stored different documentation files
- .clang-format - my personal coding style ruleset. A variation of the google file
- Makefile - a lot of rules used to compile, run, test, etc. this application

There are some other files that were not included in the final homework submission, like unit-tests, the UDP client, CI files, etc.

## Application overview

After the server is started, it will be able to receive TCP and UDP messages. The UDP messages are stored and forwarded to connected clients (subscribed to the message topic). The TCP messages are processed, and based on their type, the server will send different responses (also TCP messages).

### UDP Messages

This type of message is described in the problem statement. This is the way it was implemented:

```cpp
struct udp_message {
    char topic[...];
    bint type;
    char payload[...];
    //...
}
```

Depending on the message type, the data stored in the payload will be parsed differently. When a UDP message is stored/forwarded, it is processed into a string and the source ip and port is prepended. In the database, every message starts with a number, that represents the message id (for the store-forward system).

### TCP Messages

The structure that I decided to use for the TCP messages is the following:

```cpp
struct tcp_message {
    bint type;
    char payload[...]
}
```

There are multiple message types defined:

- DATA
- SUBSCRIBE
- UNSUBSCRIBE
- TOPIC_ID
- CONNECT
- CONFIRM_U
- CONNECT_DUP

Some of these message types have a corresponding data structure, to have a way to parse the TCP message payload easier, some don't, like `CONNECT_DUP`, the message that signals to the "subscriber" the existance of another online user with the same ID.

To decrease the message sizes, instead of sending the topic of the message, `UNSUBSCRIBE` and `DATA` contain an id. When the server "creates" a new topic, it gives it an ID. This id is a 4 bytes unsigned int, smaller than the "up to 50 bytes" topic name. Especially in the case of the `UNSUBSCRIBE` command, this is a very big difference, as it decreases the payload size by 12.5 times.

### TCP Server-Subscriber protocol

As the server-udp client interaction is described in detail in the problem statement, I will focus on the server-subscriber interaction.

When the server is started, it initialises a "main" TCP socket, on which it will wait for new connections. When a client (subscriber) is started, it will try to initialise a connection with the server. The client will send a CONNECT message, containing his ID.

The server will search in the database for a user with the that ID. If none exists, it will add him to the list. If it does and that user is marked as "offline", it will update his status, port, ip and socket. After that, it will send any messages (`DATA`) that arrived on the server, on the topics that the client was subscribed to (and activated the `SF` option). Also (for the previously "offline" users), the server will send `TOPIC_ID` messages to tell the subscriber application its subscriptions.

If the user existed and it was also online, the server will respond with a `CONNECT_DUP` message, to tell that client the ID is unavailable.

When a UDP message arrives or a user subscribes to a topic, that topic is added to the server's database (if it didn't exist before). At this point, the topic is assigned an id. When a subscriber sends a `SUBSCRIBE` message, the server will reply with a `TOPIC_ID` message, that contains the topic name and id. When the subscriber program receives that type of message, if the topic was requested with a `SUBSCRIBE` message, it will print "Subscribed {topic name}". In the negative case, it means that the message was sent from the server when the "user reconnected". To a `UNSUBSCRIBE` message, the server will remove that subscription from the user and will reply with `CONFIRM_U`.

If the client disconnects, the server closes the connection and makes the respective user "offline". If the server closes, it will close all connected TCP clients.

### Server Database

The messages received by the server are stored in memory up to a limit (500/topic). When this limit is reached, a quarter of them are stored in files. If the name of a topic is "a/b/c/d/whatever", the path to the file that contains the data is "./data/a/b/c/d/whatever". There are safeguards implemented so that files outside the directory of the server program can't be accessed. When the server is closed, all the messages are moved into the files. However, the server won't load data from the files when it is started. They should be deleted before starting the server.

## Usage and Makefile

The simplest way to test this application is to run `make run_server` to start the server and `make run_subscriber` to run a client.

There are many Makefile commands included, some that are used to build the server and subscriber, some used for testing or during development. Some of them can't be run as the files they used are not included in the homework submission.

### Variables

IP - the ip of the server
PORT - the port on which the server listens (and the clients will connect to)
USERNAME - the username/id used by the subscriber

### Commands

- build - compiles both the server and subscriber
- build_server, build_subscriber - self-explanatory
- run_server - removes the ./data folder, executables and objectfiles, then it compiles and starts the server
- run_subscriber - compiles and starts the subscriber
- clean - removes the ./data folder, executables and objectfiles
- beauty - uses clang-format and the file included in this project to "beautify" the code (coding style)
- memory - runs valgrind on the server to check for errors and memory leaks
- memory-sub - runs valgrind on the subscriber to check for errors and memory leaks
- pack - creates the "homework submission" archive
- gitignore - creates the gitignore file (and adds rules)

The next commands are not usable (the files they use are not included):

- statistics - shows the total number of lines written for the project and each individual file (in /src and /test) - development command
- udp_client - starts the udp client and sends multiple messages, all at once
- udp_manual_client - starts the udp client in manual mode
- test - runs some unit-tests (this command is also used by the CI programs)

The application was developed on a Ubuntu 18.04 LTS machine. It was tested using `gcc 7.5.0`, `clang-format 6.0.0` and `valgrind-3.13.0`.

## Other information

- if any of the subscriber/server executables are run without administrator rights (`sudo`), neagle's algorithm can't be disabled. I looked into the problem, and it has something to do with "ports under 1024 need administrator rights to be changed". I couldn't find any solution to the problem. At least, it doesn't seem to impact the server/subscriber behaviour.
- there is an option that can be activated in "Utils.hpp", `ENABLE_LOGS`, which will print some extra messages and non-critical error messages (like the ones for the problem above).
- another problem that I encountered was the fact that, despite closing all sockets, I couldn't start the server again on the same port. I found out that this is a common problem, as TCP sockets will enter a TIME_WAIT state. Even though the problem was apparently solved by changing some socket options, I'm not sure it is completely solved, as those socket options don't solve the problem sometimes.
- some components were tested using a a simple unit-test "framework" (extremely simple), while others were tested by hand
- at the beginning of this documentation, there are some badges that show CI status and other stuff. Some may not be visible, as the github repository for this project is private.

© 2020 Grama Nicolae
