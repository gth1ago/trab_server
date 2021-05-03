#include <stdio.h>
#include <string.h>

void teste(char *aa){
	char oi[]= "fogo no parkin";
	int i=0;
	memset(aa, 0, 200);

	while (i<2){
		sprintf(aa, "%s%d teste logo\n%s\n", aa, 3, oi);
		i++;
	}
	printf("1\n");
}

void pa(char *buf){
	char Buf[200];

	teste(Buf);
	*buf = Buf;
	printf("2\n");
}

int main(){
	char *buf;

	pa(buf);
	printf("3\n%s\n", buf);

	return 0;
}
