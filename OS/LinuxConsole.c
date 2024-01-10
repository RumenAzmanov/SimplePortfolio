#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const int commandLineLen = 128;
const int commandOptionsCount = 16;
const char exitCommand[] = "X";

int executeLine(char *commandOptArg[], int comOptI, int startIn);

// The character is an empty symbol
bool isEmpty(const char c) {
  return (c == ' ' || c == '\t' || c == '\0' || c == '\n');
}

// Execute a simple command using EXECVP
int executeCommand(char *commandOptArg[], int comOptI, int startInd) {
  if (execvp(commandOptArg[startInd], &commandOptArg[startInd]) < 0) {
    write(1, "Unable to execute command!: ", 29);
    write(1, commandOptArg[startInd], strlen(commandOptArg[startInd]));
    write(1, "\n", 1);
    exit(1);
  }
  exit(0);
}

// Create pipe between two commands
int executePipe(char *commandOptArg[], int comOptI, int startInd, int fdI) {
  int fd[2];
  if (pipe(fd) < 0) {
    write(1, "Unable to create a pipe!\n", 23);
    exit(1);
  }
  if (fork()) {  // Parent
    close(0);
    dup(fd[0]);
    close(fd[0]);
    close(fd[1]);
    wait();
    executeLine(commandOptArg, comOptI, fdI + 1);
  } else {  // Child
    close(1);
    dup(fd[1]);
    close(fd[0]);
    close(fd[1]);
    executeCommand(commandOptArg, comOptI, startInd);
  }
  exit(0);
}

// Open a file for input/output
int executeFile(char *commandOptArg[], int comOptI, int startInd, int fdI, char type) {
  if (fork()) {
    wait();
  } else {
    int filefd;
    if (type == '>') {
      filefd = open(commandOptArg[fdI + 1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
      if (filefd < 0) {
        write(2, "Unable to open the file for writing!\n", 26);
        exit(1);
      }
      int stdOut;
      dup2(1, stdOut);
      dup2(filefd, 1);
      commandOptArg[fdI + 1] = NULL;
      if (fork()) {
        wait();
        dup2(stdOut, 1);
        if (comOptI > fdI + 3) {
          executeLine(commandOptArg, comOptI, fdI + 3);
        }
      } else {
        executeCommand(commandOptArg, comOptI, startInd);
      }
    }
    if (type == '<') {
      filefd = open(commandOptArg[fdI + 1], O_RDONLY);
      if (filefd < 0) {
        write(2, "Unable to open the file for reading!\n", 35);
        exit(1);
      }
      close(0);
      dup(filefd);
      commandOptArg[fdI + 1] = NULL;
      executeLine(commandOptArg, comOptI, startInd);
    }
    close(filefd);
  }
  exit(0);
}

// Execute a line of operators
int executeLine(char *commandOptArg[], int comOptI, int startInd) {
  int fdI = 0;
  int flagOperation = 0;
  char type;
  for (fdI = startInd; fdI < comOptI - 1; ++fdI) {
    if (commandOptArg[fdI] != NULL) {
      type = commandOptArg[fdI][0];
      if (type == '|' || type == '>' || type == '<') {
        flagOperation = 1;
        commandOptArg[fdI] = NULL;
        if (type == '|') {
          executePipe(commandOptArg, comOptI, startInd, fdI);
          break;
        }
        if (type == '>' || type == '<') {
          executeFile(commandOptArg, comOptI, startInd, fdI, type);
          break;
        }
      }
    }
  }

  if (!flagOperation) executeCommand(commandOptArg, comOptI, startInd);
  exit(0);
}

int main() {
  char commandLine[commandLineLen];
  char *commandOptArg[commandOptionsCount];
  int readCnt = 0;
  int comOptI = 0;

  while (true) {
    // Display interface
    int i = 0;
    for (i = 0; i < commandLineLen; ++i) commandLine[i] = '\0';
    for (i = 0; i < commandOptionsCount; ++i) commandOptArg[i] = NULL;
    write(1, "~>", 2);
    readCnt = read(0, commandLine, commandLineLen);
    if (readCnt < 0) {
      write(2, "Empty command!\n", 15);
      continue;
    }

    // Remove spaces and tabulations
    int readI = 0, writeI = 0;
    while (isEmpty(commandLine[readI])) ++readI;
    for (; readI < readCnt; ++readI) {
      if (isEmpty(commandLine[readI])) {
        if (!isEmpty(commandLine[readI - 1])) commandLine[writeI++] = '\0';
        continue;
      }
      commandLine[writeI++] = commandLine[readI];
    }
    readCnt = writeI - 1;

    // Format the string as parameters
    comOptI = 0;
    commandOptArg[comOptI++] = &commandLine[0];
    for (readI = 0; readI < readCnt; ++readI) {
      if (isEmpty(commandLine[readI]) && readI + 1 < readCnt)
        commandOptArg[comOptI++] = &commandLine[readI + 1];
    }
    commandOptArg[comOptI] = NULL;

    // Exit command
    if (strcmp(commandOptArg[0], exitCommand) == 0) break;

    // Execute command line
    int PID;
    if ((PID = fork()) < 0) {
      write(2, "Unable to create a new process!\n", 32);
      exit(1);
    } else {
      if (PID != 0) {
        if (commandOptArg[comOptI - 1][0] == '&') {
          write(1, "[", 1);
          write(1, &PID, sizeof(PID));
          write(1, "]\n", 2);
        } else {
          wait();
        }
      } else {
        if (commandOptArg[comOptI - 1][0] == '&') {
          commandOptArg[comOptI - 1] = NULL;
        }
        executeLine(commandOptArg, comOptI, 0);
        // exit(0);
      }
    }

    // write(1, "End!\n", 5);
    // break;
  }

  write(1, "Goodbye!\n", 9);
  exit(0);
  return 0;
}