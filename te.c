#include <stdio.h>
#include <string.h>

int main(){
	const char t[] = "/";

	char *ai[6];
	char frase[] = "meu / vix eu / to / 4/ final";
	int i=0;
	char *token = strtok(frase, t);

	while(token != NULL){
		ai[i] = token;
		//printf("-> %s\n", token);
		token = strtok(NULL, t);
		i++;
	}

	printf("1 - %s\n", ai[0]);
	printf("3 - %s\n", ai[2]);

	return 0;
}
