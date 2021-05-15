#include <stdio.h>	//printf
#include <stdlib.h>	// atoi
#include <string.h>	// strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>

#if defined(__linux) || defined(__linux__)
#define limparTela() system("clear")
#else
#define limparTela() system("cls")
#endif

#define TAM 10000



void options(){
	printf(
	 "\t ______________MENU______________\n"
	 "\t| 1 Visualizar lista de filmes\t|\n" 
	 "\t| 2 Pesquisar filme por ID\t|\n"
	 "\t| 3 Inserir novo filme\t\t|\n"
	 "\t| 4 Limpar Tela\t\t\t|\n"
	 "\t| 5 Informações da implementação|\n"
	 "\t| 6 Sair\t\t\t|\n"
	 "\t|_______________________________|\n"
	 );
}

void infos(){
	printf(
	 "\t _____________INFOS_____________\n"
	 "\t|   Utilizado para comunicação \t|\n" 
	 "\t| 	  entre os processo\t|\n"
	 "\t|  \t\t\t\t|\n"
	 "\t| 1 -> Pipe  \t\t\t|\n"
	 "\t| 2 -> Fifo  \t\t\t|\n"
	 "\t| 3 -> Memoria Compartilhada\t|\n"
	 "\t|  \t\t\t\t|\n"
	 "\t|    Para mais informações\t|\n"
	 "\t|________acesse o README________|\n\n\n"
	 );
}

void optionsPost(char *message){
	char title[50], abstract[200], gender[50], cast[100], stat[]="Disponivel";
	printf(
	 "\t_Preenchimento de dados do Filme_\n\n"
	 "Titulo do filme\n--> ");
	scanf(" %99[^\n]", title);
	printf("\nResumo do filme\n--> ");
	scanf(" %99[^\n]", abstract);
	printf("\nGenero(s) do filme\n--> ");
	scanf(" %99[^\n]", gender);
	printf("\nElenco do filme\n--> ");
	scanf(" %99[^\n]", cast);
	sprintf(message, "POST /3 /%s /%s/%s /%s /%s", title, abstract, gender, cast, stat);
}

void optionSearch(char *id){
	
	printf(
	 "\n________________Pesquisa por ID_________________\n\n"
	 "Digite o ID do filme => ");
	scanf(" %99[^\n]", id);
}

int main(int argc , char *argv[])
{
	int sock;
	struct sockaddr_in server;
	char message[TAM] , server_reply[TAM], aux[10];
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		close(sock);
		return 1;
	}
	
	printf("\t _______________________________\n");
	printf("\t|\t\t\t\t|\n");
	printf("\t|\tSTEAMING MOVIES\t\t|\n");
	printf("\t|_______________________________|\n\n");
	//keep communicating with server
	while(1){ 
		init:
		options();
		printf("\nOpção => ");
		scanf(" %99[^\n]", message);
		int opt = atoi(message);
		
		printf("\nOP: %d\n", opt);
		memset(message, 0, sizeof(message));
		
		switch (opt){
		case 1:
			sprintf(message, "GET /%d", opt);
			break;

		case 2:
			limparTela();
			optionSearch(aux);
			sprintf(message, "GET /%d /%s", opt, aux);
			break;
		
		case 3:
			limparTela();
			optionsPost(message);
			break;
		
		case 4:
			limparTela();
			goto init;

		case 5:
			limparTela();
			infos();
			goto init;
		
		case 6:
			goto saida;
		
		default:
			puts("Opção inválida!");
			goto init;
		}

		if( send(sock , message , strlen(message) , 0) < 0){
			puts("Falha ao comunicar com servidor");
			return 1;
		}

		memset(server_reply, 0, TAM);
		//Receive a reply from the server
		if( recv(sock , server_reply , TAM , 0) < 0){
			puts("Falha ao receber do servidor");
			break;
		}
		limparTela();
		puts("Info: ");
		puts(server_reply);
		puts("________________________________________________________________\n");

	}
	
	saida:
		puts("Fechando!");
		close(sock);
	
	return 0;
}