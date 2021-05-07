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
	 "\t| 4 Alugar filme\t\t|\n"
	 "\t| 5 Sair\t\t\t|\n"
	 //"\t-v or --verbose\t\t\tgive verbose output\n"
	 );
}
void optionsPost(char *message){
	char title[50], abstract[200], gender[50], cast[100], stat[]="Disponivel";
	printf(
	 "\tt_Preenchimento de dados do Filme_  --\n"
	 "\tTitulo do filme\n--> ");
	scanf(" %99[^\n]", title);
	puts("\n\tResumo do filme\n--> ");
	scanf(" %99[^\n]", abstract);
	puts("\n\tGenero(s) do filme\n--> ");
	scanf(" %99[^\n]", gender);
	puts("\n\tElenco do filme\n--> ");
	scanf(" %99[^\n]", cast);
	sprintf(message, "POST /3 / %s/ %s/ %s/ %s/ %s", title, abstract, gender, cast, stat);
}

void optionSearch(char *id){
	
	printf(
	 "\n________________Pesquisa por ID_________________\n\n"
	 "Digite o ID do filme => ");
	scanf("%s", id);
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
	puts("Socket created");
	
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
	
	puts("Connected\n");
	
	//keep communicating with server
	while(1){ 
		init:
		options();
		printf("\nOpção => ");
		scanf("%s" , message);
		int opt = atoi(message);
		
		printf("\nOP: %d\n", opt);
		memset(message, 0, sizeof(message));
		
		switch (opt){
		case 1:
			sprintf(message, "GET /%d /aloou", opt);
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
			puts("Ainda no implementado!\n");
			// PUT /...
			break;
		
		case 5:
			goto saida;
		
		default:
			puts("Opção inválida!");
			goto init;
		}

		printf("\nmessage: %s\n\n", message);
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
		puts("Server reply: ");
		puts(server_reply);
		puts("________________________________________________________________\n");

	}
	
	saida:
		puts("Fechando!");
		close(sock);
	
	return 0;
}