# SO - Tracking and Monitoring Program Execution

## Table of Contents

1. [Overview](#overview)
2. [Key Features](#key-features)
3. [Dependencies](#dependencies)
4. [Cloning the Repository](#cloning-the-repository)
5. [Compiling and Running](#compiling-and-running)
6. [Cleaning Up](#cleaning-up)
7. [Group's Report](#groups-report)
8. [Conclusion](#conclusion)
9. [Developed by](#developed-by)

## Overview

This project, developed during the second semester of the second year of the Software Engineering degree at the University of Minho as part of the `Sistemas Operativos` subject, focuses on implementing a program execution monitoring service. It involves developing both a client (tracer) and a server (monitor) to enable users to execute programs and retrieve their execution time, as well as allowing system administrators to monitor running programs and access statistics on completed programs.

For further information about this project, please read the [project report](docs/project.pdf).

## Key Features

- `Client-Server Architecture:` The project implements a client-server model where the client (tracer) communicates with the server (monitor) to execute programs and retrieve relevant information.

- `Execution Monitoring:` Users can execute programs via the client and obtain their execution time. The server is responsible for storing execution information and providing it for user queries.

- `Real-Time Monitoring:` System administrators can monitor running programs in real-time and access statistics on completed programs.

- `Concurrency Support:` The server supports concurrent processing of requests to prevent blocking interactions with clients.

## Dependencies

Since this project was developed in C, its dependencies primarily revolve around the GNU Compiler Collection (gcc).
To install `gcc`, you can use various package managers depending on your Linux distribution. Here are some examples:

### Debian/Ubuntu-based distributions (using apt):

```
$ sudo apt update
$ sudo apt install gcc
```

### Fedora-based distributions (using dnf):

```
$ sudo dnf install gcc
```

### Arch-based distributions (using pacman):

```
$ sudo pacman -S gcc
```

### Manjaro Linux (using pamac):

```
$ sudo pamac install gcc
```

Feel free to choose the appropriate command based on your Linux distribution and package manager.

## Cloning the Repository

To clone the repository, run the following command in your terminal:

```
$ git clone https://github.com/JoaoCoelho2003/SO.git
```

Once cloned, navigate to the repository directory using the cd command:

```
$ cd SO
```

## Compiling and Running

To compile the project, navigate to the root directory of the project where the Makefile is located and execute the following command:

```
$ make
```

This command will compile the project and generate the executable files. The Makefile includes rules for both the server and client programs.

To run the compiled server program, use the following command:

```
$ ./bin/monitor
```

There are various ways of running the tracer program, each with its own set of command-line options and arguments. One example is:

```
$ ./bin/tracer -u program_name arg1 arg2 ...
```

However, for a detailed explanation of all available options and usage scenarios, we strongly recommend referring to the project report provided earlier.

## Cleaning Up

To clean up the compiled files and reset the project to its initial state, you can use the following command:

```
$ make clean
```

This command will remove all compiled object files, executable files, and any folders created during the compilation process.

## Group's Report

The group has prepared a comprehensive report that provides a detailed explanation of the developed project, including all the decisions made during the design and implementation phases. This report delves into various aspects of the project, such as system architecture, communication mechanisms, and implementation details. It serves as a valuable resource for understanding the project's goals, methodologies, and outcomes.

[Final Report](reports/SO.pdf)

## Conclusion

We trust that engaging with the project on program execution monitoring has been both enlightening and fulfilling for you. If you have any inquiries, recommendations, or feedback, feel free to get in touch. Keep exploring and enhancing your skills! Happy coding!

## Developed by

**A100596** João Coelho

**A100692** José Rodrigues

**A100750** Duarte Araújo