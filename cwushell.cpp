#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <bits/stdc++.h>

// Author: Eric Engman
// SID: 40855107
// I pledge this is my code and I recieved no outside help

using namespace std;

int parse(char ** command, char * input);
void executeArgs(char ** command, int numberOfParams);
void executeDistro(char ** args, int numberOfParams);
void displayInfo(char ** args, int numberOfParams);
int determineArgs(char ** args, int numberOfParams);
void help(char ** args);
void executeCD(char ** args);


int main(){

  char * args[100];
  char input[100];
  char  *prompt = "cwushell";
  int numberOfParams = 0;
  int status = 1;


  while(status){
    cout << prompt << "> ";

    cin.getline(input, 100);
    numberOfParams = parse(args, input);

    // if change_prompt is executed without paremeters
    if ((strcmp(args[0], "change_prompt") == 0) && (numberOfParams == 1)){
      prompt = "cwushell";
      continue;
    }else if((strcmp(args[0], "change_prompt") == 0) && (args[1] != NULL)){
      // if -help or -h isnt a parameter
      if(!((strcmp(args[1], "-help") == 0) || (strcmp(args[1], "-h") == 0))){
        prompt = args[1];
      }
    }

    status = determineArgs(args, numberOfParams);

  }

  return 0;
}

// Function parses input and stores it in args array
// returns the number of parameters passed
int parse(char ** args, char * input){

  char * p = strtok(input, " ");

  int i = 0;
  while(p != NULL){
    args[i++] = p;
    p = strtok(NULL, " ");
  }

  args[i] = NULL;
  return i;

}

// Function executes the internal and external commands
void executeArgs(char ** args, int numberOfParams){

  pid_t pid = fork();
  if(pid == 0){

      if(execvp(args[0], args) == -1){
        perror("exec");
      }
  }
  if(pid > 0){
      // wait for child to execute
      wait(0);
  }

}

// Function checks to see what command function needs to be called or
// executes help function if needed
int determineArgs(char ** args, int numberOfParams){

  // quit
  if(strcmp(args[0], "quit") == 0){
    cout << "\nGoodbye" << endl;
    if(args[1] != NULL){
      cout << "Shell exiting (" << args[1] << ")" << endl;
    }
    return 0;
  }
  // distro
  if(strcmp(args[0], "distro") == 0){
    if(numberOfParams > 1){
      executeDistro(args, numberOfParams);  // distro command and flags
      return 1;
    }else{
      help(args);
      return 1;
    }
  }
  // info
  if(strcmp(args[0], "info") == 0){
    if(numberOfParams > 1){
      displayInfo(args, numberOfParams);    // display info command in flags
      return 1;
    }else{
      help(args);
      return 1;
    }
  }

  // change_prompt
  if(strcmp(args[0], "change_prompt") == 0){
    if((strcmp(args[1], "-help") == 0) || (strcmp(args[1], "-h") == 0)){
      help(args);
      return 1;
    }
    return 1;
  }

  if(strcmp(args[0], "cd") == 0){
    executeCD(args);
    return 1;
  }

  // execute other commands
  executeArgs(args, numberOfParams);  // other commands

  return 1;
}

// Function that prints on the screen different cpu related information based
// on the switch or invokes help function if needed
void executeDistro(char ** args, int numberOfParams){

  fstream file;
  string line;
  string info;
  string fileLines[4];
  int num;

  // Open file with distro information
  file.open("/etc/lsb-release", ios::in);
  if(file.is_open()){
    num = 0;
    while(getline(file, line)){
        fileLines[num++] = line;
    }
    file.close();
  }
  // print distros version
  if(strcmp(args[1], "-v") == 0){
    cout << fileLines[1] << endl;
  // print distros name
  }else if(strcmp(args[1], "-n") == 0){
    cout << fileLines[0] << endl;
  // print distros codename
  }else if(strcmp(args[1], "-c") == 0){
    cout << fileLines[2] << endl;
  }else{
    cout << "\nThis input is not recognized, here's some help" << endl;
    help(args);
  }

}

// Function that prints on screen different memory related information based
// on the switch
void displayInfo(char ** args, int numberOfParams){

  fstream file;
  string line;
  string info;

  // Page size
  if(strcmp(args[1], "-s") == 0){
    int size = getpagesize();
    int sizeKB = size/1000;
    cout << "\nCurrent memory page size: " << sizeKB << " KB" << endl;
    cout << endl;
  }else if(strcmp(args[1], "-n") == 0){
    // print out currently available page numbers
    printf("\nAvailable page numbers: %li", get_avphys_pages());
    cout << endl;
    cout << endl;
  }else if(strcmp(args[1], "-p") == 0){
    // print out number of available processors
    printf("\nAvailable Processors: %d", get_nprocs());
    cout << endl;
    cout << endl;
  }else{
    cout << "\nThis input is not recognized, here's some help" << endl;
    help(args);
  }

}

// Function to handle the cd problem with fork()
void executeCD(char ** args){

  if(args[1] != NULL){
    if(chdir(args[1]) != 0){
        perror("chdir() failed");
        cout << endl;
    }
  }else{
    cout << "Please provide a directory to change too" << endl;
  }

}

// Function that prints out specific help depending on switches or lack thereof
void help(char ** args){
  if(strcmp(args[0], "distro") == 0 ){
    cout << endl;
    cout << "distro [options] " << endl;
    cout << "Options: " << endl;
    cout << "-help or -h      Display this information" << endl;
    cout << "-v               Print the distros version" << endl;
    cout << "-n               Print the distros name" << endl;
    cout << "-c               Print the distros codename" << endl;
    cout << endl;
  }

  if(strcmp(args[0], "info") == 0){
    cout << endl;
    cout << "info [options] " << endl;
    cout << "Options: " << endl;
    cout << "-help or -h      Display this information" << endl;
    cout << "-s               Print current mem page size in KB" << endl;
    cout << "-n               Print current available page numbers" << endl;
    cout << "-p               Print the number of available processors" << endl;
    cout << endl;
  }

  if(strcmp(args[0], "change_prompt") == 0){
    cout << endl;
    cout << "change_prompt [options] " << endl;
    cout << "Options: " << endl;
    cout << "-help or - h         Display this information" << endl;
    cout << "NULL                 Restore default prompt" << endl;
    cout << "[new_prompt_name]    will change current shell prompt to new_prompt_name" << endl;
    cout << endl;
  }
}
