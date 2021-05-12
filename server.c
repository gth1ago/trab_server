#include<stdio.h>
#include<stdlib.h>	//strlen
#include<string.h>	// atoi
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<pthread.h> //for threading , link with lpthread
#include <semaphore.h>
#include "functionsXML.c"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/mman.h>
#define MSGSIZE 16

void *connection_handler(void *socket_desc);
void* create_shared_memory(size_t size);
void requiredOption();

char name[] = "streaming.xml"; // xml com os dados

int main(int argc , char *argv[])
{
	int socket_desc , client_sock , c , *new_sock;
	struct sockaddr_in server , client;

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("\nLogs:\n\nSocket created");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");

	//Listen
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("\nWaiting for incoming connections...\n");
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
	char message[TAM], client_message[TAM], textXml[TAM], line[TAM];
	char *myfifo = "/tmp/myfifo";
	
	mkfifo(myfifo, 0666);

	//Receive a message from client
	while( (read_size = recv(sock , client_message , TAM , 0)) > 0 )
	{ 
		int fd[2];
		int iPid;
		void* shmem = create_shared_memory(128);

		if(pipe(fd)<0) {
			perror("pipe") ;
			exit(1) ;
		}

		strcpy(message, " ");
		puts("Aguardando\n");
		printf("%s\n", client_message);
		const char del[]= "/";
		
		int typeT, i = 0, n;
		char *token, *preenche[10];

		token = strtok(client_message, del);

		while(token != NULL){ 			// preenche entradas
			preenche[i] = token;
			token = strtok(NULL, del);
			i++;
		}

		switch (atoi(preenche[1])){
		case 1: // listagem - pipe
			//sem_wait(&sem);
            iPid = fork();
        	memset(line, 0, TAM);

            if (iPid < 0){
                puts("Error fork iPid!");
                exit(1);
            }

			// filho
            if (iPid == 0){
				printf("filho: %d\n", getppid());
				char buf[TAM];
        		char *text;
				readFile(name, buf);
				text = buf;
				// Operação obrigatória: fechar o descritor
				close(fd[0]) ;

				/* Escreve a mensagem no pipe. */
				write(fd[1], text, strlen(text)+1) ;
				close(fd[1]);
				kill(getpid(), SIGKILL);
            }
			//pai
            else{
                //wait(NULL);
				printf("pai: %d\n", getppid());
                close(fd[1]); // fecha o lado emissor do pipe
                read(fd[0], message, sizeof message);
                close(fd[0]);
            }
			puts("Listado\n");
            break;
		case 2:	// busca - fifo
			iPid = fork();

			if (iPid < 0){
                puts("Error fork iPid!");
                exit(1);
            }
			
			// filho
            if (iPid == 0){
				char buf[TAM];
				char *text;
				int Fd1;
				printf("filho: %d\n", getppid());
				findFile(name, preenche[2], buf);
				text = buf;
				puts(line);
				if((Fd1 = open(myfifo, O_WRONLY)) == -1){
					perror("open Fd fifo");
					exit(EXIT_FAILURE);
				}
				write(Fd1, text, strlen(text)+1);
				close(Fd1);
				kill(getpid(), SIGKILL);
			}
			
			else{
				int Fd2;
				printf("pai: %d\n", getppid());
				if((Fd2 = open(myfifo, O_RDONLY)) == -1){
					perror("open Fd fifo");
					exit(EXIT_FAILURE);
				}
				read(Fd2, message, sizeof message);
				close(Fd2);
			}
			puts("Pesquisado\n");
			break;

		case 3: // inserção - mc
			iPid = fork();

			if (iPid < 0){
                puts("Error fork iPid!");
                exit(1);
            }
			
			// filho
            if (iPid == 0){
				int val;
				printf("filho: %d\n", getppid());
		    	toInsertFile(name, preenche[2], preenche[3], preenche[4], preenche[5], preenche[6], textXml);
				if (insertEndFile(name, textXml) > 0)
					memcpy(shmem, "1", sizeof("1"));
				else
					memcpy(shmem, "0", sizeof("0"));

				kill(getpid(), SIGKILL);
			}
			else{
				printf("pai: %d\n", getppid());
				if (strcmp(shmem, "1"))
					strcpy(message, "Inserido\n");
				else
					strcpy(message, "Falha ao Inserir\n");
			}
			break;
		
		default:
			strcpy(message, "Opção invalida!\n");
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

void* create_shared_memory(size_t size) {
  // para comunicação de sucesso na inserção
  int protection = PROT_READ | PROT_WRITE;

  // visibilidade para apenas o responsavel e os filhos puder usa-los

  int visibility = MAP_SHARED | MAP_ANONYMOUS; // compilacao com gnu99 para o MAP_ANONYMOUS 

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, -1, 0);
}
