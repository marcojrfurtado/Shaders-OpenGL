//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol

#ifndef MY_UTILITY_STUFF
#define MY_UTILITY_STUFF

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* convertFileToCString(char *fn) {


	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) {
		fp = fopen(fn,"rt");
		//errno_t result = fopen_s(&fp, fn,"rt");

		//if( result == 0 && fp != NULL) {
		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

#endif
