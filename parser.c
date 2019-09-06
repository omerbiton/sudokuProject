#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MainAux.h"



/* This module is responsible of parsing the input string of the player
 * during the game into commands on the board.
 */


/* parse the input string of the player during the game into commands on the board */
void parseUserInput(int *command, char* path char input[]){
	/* define the separators of the user inputs */
	   const char s[5] = " \t\r\n";
	   char* token;
	   int i =0;
	   /* get the first token */
	   /* input[0] represent the command of the user set/hint/.. */
	   token = strtok(input, s);
	   if( token == NULL){
		   command[0] = 18;
	   }
	   else if(strcmp(token, "solve") == 0){
		   /* walk through other tokens */
		   command[0] = 1;
		   while(i < 1) {
			   token = strtok(NULL, s);
			   if(token != NULL){
				   path = token;
			   }
			   else{
				   command[1] = 1;
		          }
		      i++;
		   }
	   }
	   else if(strcmp(token, "edit") == 0){
		   /* walk through other tokens */
		   command[0] = 2;
		   while(i < 1) {
			   token = strtok(NULL, s);
			   if(token != NULL){
				   path = token;
			   }
			   else{
				   command[1] = 1;
		          }
		      i++;
		   }
	   }
	   else if(strcmp(token, "mark_errors") == 0){
		   /* walk through other tokens */
		   command[0] = 3;
		   while(i < 1) {
			   token = strtok(NULL, s);
			   if(token != NULL){
				   command[i+1] = atoi(token);
			   }
			   else{
				   command[0] = 19;
		          }
		      i++;
		   }
	   }
	   else if(strcmp(token, "print_board") == 0){
		   /* walk through other tokens */
		   command[0] = 4;
	   }
	   else if(strcmp(token, "set") == 0){
		   /* walk through other tokens */
		   command[0] = 5;
		   while(i < 3) {
			   token = strtok(NULL, s);
			   if(token != NULL){
				   command[i+1] = atoi(token);
			   }
			   else{
				   command[0] = 19;
		          }
		      i++;
		   }
	   }
	   else if(strcmp(token, "validate") == 0){
		   command[0] = 6;
	   }
	   else if(strcmp(token, "guess") == 0){
		   /* walk through other tokens */
		   command[0] = 7;
		   while(i < 1) {
			   token = strtok(NULL, s);
			   if(token != NULL){
				   command[i+1] = atoi(token);
			   }
			   else{
				   command[0] = 19;
		          }
		      i++;
		   }
	   }
	   else if(strcmp(token, "generate") == 0){
		   command[0] = 8;
		   /* walk through other tokens */
		   while(i < 2) {
			   token = strtok(NULL, s);
			   if(token != NULL){
				   command[i+1] = atoi(token);
			   }
			   else{
				   command[0] = 19;
		          }
		      i++;
		   }
	   }
	   else if(strcmp(token, "undo") == 0){
		   command[0] = 9;
	   }
	   else if(strcmp(token, "redo") == 0){
		   command[0] = 10;
	   }
	   else if(strcmp(token, "save") == 0){
		   /* walk through other tokens */
		   command[0] = 11;
		   while(i < 1) {
			   token = strtok(NULL, s);
			   if(token != NULL){
				   command[i+1] = atoi(token);
			   }
			   else{
				   command[0] = 19;
		          }
		      i++;
		   }
	   }
	   else if(strcmp(token, "hint") == 0){
		   command[0] = 12;
		   /* walk through other tokens */
		   while(i < 2) {
			   token = strtok(NULL, s);
			   if(token != NULL){
				   command[i+1] = atoi(token);
			   }
			   else{
				   command[0] = 19;
		          }
		      i++;
		   }
	   }
	   else if(strcmp(token, "guess_hint") == 0){
		   command[0] = 13;
		   /* walk through other tokens */
		   while(i < 2) {
			   token = strtok(NULL, s);
			   if(token != NULL){
				   command[i+1] = atoi(token);
			   }
			   else{
				   command[0] = 19;
		          }
		      i++;
		   }
	   }
	   else if(strcmp(token, "num_solutions") == 0){
		   command[0] = 14;
	   }
	   else if(strcmp(token, "autofill") == 0){
		   command[0] = 15;
	   }
	   else if(strcmp(token, "restart") == 0){
		   command[0] = 16;
	   }
	   else if(strcmp(token, "exit") == 0){
		   command[0] = 17;
	   }
	   else{
		   command[0] = 19;

	   }
	}

