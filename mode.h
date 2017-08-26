#ifndef __MODE_H_
#define __MODE_H_

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Mode {
    bool detailed;
    bool verbose;
    bool roundRobin;
    int quantum;
} Mode;

/* Creates a new mode to run the simulation in. Handles any errors
 * with a useful message to stderr, and terminated the program.
 * args: command line arguments to parse into the mode
 * returns: a mode that signifies such things as:
 *          RR, quantum, verbose, detailed */
Mode newMode(char** argv, int argc);

/* Checks to see if a string can be converted to an integer.
 * args: the string to check
 * returns: true if string is an int, false otherwise */
bool isInt(char* str);

/* Checks if a string is empty or not
 * args: the string to check
 * returns: true if all characters are whitespace, false otherwise */
bool isEmpty(char* str);

#endif
