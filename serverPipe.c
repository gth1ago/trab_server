#include<stdio.h>
#include<string.h>	// atoi
#include<stdlib.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<pthread.h> //for threading , link with lpthread
#include <semaphore.h>
#include "functionsXML.c"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#define MSGSIZE 16

void *connection_handler(void *socket_desc);
void requiredOption();
const char *searchPipe(char name[], char *preenche);

sem_t sem; 		// semafaro
int shared;
int value;

int main(int argc , char *argv[])
{
	shared = 0;  // se 0 = nao pode compartilhar entre processo; se != 0 pode
	value = 1;
	sem_init(&sem, shared, value);

	int socket_desc , client_sock , c , *new_sock;
	struct sockaddr_in server , client;

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");

	//Listen
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	
	
	//Accept and incoming connection
	//puts("Waiting for incoming connections...");
	//c = sizeof(struct sockaddr_in);
	while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		puts("Connection accepted");
		
		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = client_sock;
		
		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		{
			perror("could not create thread");
			return 1;
		}
		
		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( sniffer_thread , NULL);
		puts("Handler assigned");
	}
	
	if (client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}
	
	return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    char inbuf[MSGSIZE];

	//Get the socket descriptor
	int sock = *(int*)socket_desc, read_size;
	char message[TAM], client_message[TAM], textXml[TAM], name[] = "locadora.xml";

	//Receive a message from client
	while( (read_size = recv(sock , client_message , TAM , 0)) > 0 )
	{ 
		int fd[2];
		int iPid;
		if(pipe(fd)<0) {
			perror("pipe") ;
			exit(1) ;
		}

		strcpy(message, " ");
		puts("Aguardando\n");
		printf("%s\n", client_message);
		const char del[]= "/";
		
		int typeT, i = 0, tamanho = strlen(client_message), n;
		char *token, *preenche[10];

		token = strtok(client_message, del);

		while(token != NULL){
			preenche[i] = token;
			token = strtok(NULL, del);
			i++;
		}

		switch (atoi(preenche[1])){
		case 1: // uso do pipe
			sem_wait(&sem);
            iPid = fork();
        	char line[TAM];

            if (iPid < 0){
                puts("Error fork iPid!");
                exit(1);
            }
			// filho
            if (iPid == 0){
				printf("filho: %d\n", getppid());
				char buf[TAM];
        		char *hello;
				readFile(name, buf);
				hello = buf;
				// Operação obrigatória: fechar o descritor
				close(fd[0]) ;

				/* Escreve a mensagem no pipe. */
				write(fd[1], hello, strlen(hello)+1) ;
				sem_post(&sem);
				close(fd[1]);
				sleep(1);
				kill(getpid(), SIGKILL);
            }
            else{
                //wait(NULL);
				printf("pai: %d\n", getppid());
                close(fd[1]); // fecha o lado emissor do pipe
                read(fd[0], message, sizeof message);
                close(fd[0]);
            }
			puts("Listado");
            break;
		case 2:	
			//write(sock , "Ok" , strlen("Ok"));
			findFile(name, preenche[2], message);
			puts("Pesquisado\n");
			break;

		case 3:
		    toInsertFile(name, preenche[2], preenche[3], preenche[4], preenche[5], preenche[6], textXml);
			printf("--------------------------------------------\n");
			insertEndFile(name, textXml);
			break;
		
		default:
			break;
		}

		//Send the message back to client
		write(sock , message , strlen(message));
		memset(message, 0, TAM);
		memset(client_message, 0, TAM);
		memset(preenche, 0, sizeof(preenche));
	}
	
	if(read_size == 0)
	{	
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
	
	//Free the socket pointer
	free(socket_desc);
	
	return 0;
}

const char *searchPipe(char name[], char pesquisa[]){
    char msg[TAM] ;
    int fd[2] ;
    int pid ;

    /* Cria um pipe. */
    if(pipe(fd)<0) {
        perror("pipe") ;
       exit(1) ;
    }

    /* Cria processo filho. */
    pid = fork() ;

    if(pid == -1) {
        perror("fork") ;
        exit(1) ;
    }

    /* Processo filho. */
    if(pid) {
		char buf[TAM];
        char *hello;
		findFile(name, pesquisa, buf);
		hello = buf;
        /* Operação obrigatória: fechar o descritor
         * desnecessário. */
        close(fd[0]) ;

        /* Escreve a mensagem no pipe. */
        write(fd[1], hello, strlen(hello)+1) ;

        close(fd[1]) ;
		sleep(2);
    } else { /* Processo pai. */
        /* Operação obrigatória: fechar o descritor
         * desnecessário. */
        close(fd[1]) ;

        /* Lê a mensagem do pipe. */
        read(fd[0], msg, sizeof msg) ;

        printf("Mensagem recebida: %s\n", msg) ;

        close(fd[0]) ;
    }
	return "oie";
}