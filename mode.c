#include "mode.h"

/* Creates a new mode to run the simulation in. Handles any errors
 * with a useful message to stderr, and terminated the program.
 * args: command line arguments to parse into the mode
 * returns: a mode that signifies such things as:
 *          RR, quantum, verbose, detailed */
Mode newMode(char** argv, int argc) {
    Mode new;
    
    new.detailed   = false;
    new.verbose    = false;
    new.roundRobin = false;
    new.quantum    = 0;
    
    for(int i=1; i<argc; i++) {
        if(strcmp(argv[i], "-r") == 0) {
           new.roundRobin = true;
           if(i < argc-1) {
               if(isInt(argv[i+1])) {
                   new.quantum = atol(argv[i+1]);
                   
                   if(new.quantum < 1) {
                       fprintf(stderr, "This won't accomplish anything.\n");
                       exit(EXIT_FAILURE);
                   }
                   i++;
               } else {
                   fprintf(stderr, "Quantum value must be an integer.\n");
                   exit(EXIT_FAILURE);
               }
           } else {
               fprintf(stderr, "Round robin flag must be followed by a quantum value.\n");
               exit(EXIT_FAILURE);
           }
        } else if(strcmp(argv[i], "-d") == 0) {
            new.detailed = true;
        } else if(strcmp(argv[i], "-v") == 0) {
            new.verbose = true;
        } else {
            fprintf(stderr, "Usage: %s [-d] [-v] [-r quantum]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    return new;
}

/* Checks to see if a string can be converted to an integer.
 * args: the string to check
 * returns: true if string is an int, false otherwise */
bool isInt(char* str) {
	int len = 0;

	if(!str) {
		return false;
	} else if((len = strlen(str)) < 1) {
		return false;
	}
    
	for(int i=0; i<len; i++) {
		if(!isdigit(str[i])) {
			if(i == 0 && str[i] == '-') {
				continue;
			}
			return false;
		}
	}
	return true;
}

/* Checks if a string is empty or not
 * args: the string to check
 * returns: true if all characters are whitespace, false otherwise */
bool isEmpty(char* str) {
    int len = 0;
    
    if(!str) {
		return false;
	} else if((len = strlen(str)) < 1) {
		return false;
	}
    
    for(int i=0; i<len; i++) {
		if(!isspace(str[i])) {
			return false;
		}
	}
	return true;
}
