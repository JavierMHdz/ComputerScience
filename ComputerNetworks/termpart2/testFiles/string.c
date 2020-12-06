#include <stdio.h>
#include <string.h>

int main(){
	char str[] = "Geeks for Geeks";

	//Return first token
	char* token = strtok(str, " ");

	//Keep printing tokens while one of the
	//delimeters present in str[].
	while(token != NULL){
		printf("%s\n",token);
		token = strtok(NULL, " ");
	}

	char a[20] = "a";
	char b[20] = "b";
	char c[20] = "c";
	char d[20] = "d";

	char array[20] = "";
	strcat(array, a);
	strcat(array, b);
	strcat(array, c);
	strcat(array, d);

	printf("%s\n", array);

	return 0;
}