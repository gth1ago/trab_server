#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ezxml/ezxml.c"
#include "ezxml/ezxml.h"
#define TAM 100000

int insertEndFile(char name[], char new[]){
    int verf = 0;
    FILE * fd= fopen(name, "r+b");
        
    if(fd == NULL){
        printf("Erro ao abrir o arquivo!\n");
        return 1;
    }

    fseek(fd, -13, SEEK_END);
    verf = fprintf(fd, "\t%s\n</streamings>", new);
    fclose(fd); 
    
    return verf;
}

int readFile(char *name, char *message){
    ezxml_t l1 = ezxml_parse_file(name), filme, titulo, resumo, genero, elenco, status;
    const char *filmename;
    memset(message, 0, TAM);
    for (filme = ezxml_child(l1, "filme"); filme; filme = filme->next) {
        filmename = ezxml_attr(filme, "id");

        sprintf(message, "%sID: %s\nTitulo: %s\nResumo: %s\nGenero: %s\nElenco: %s\nStatus: %s\n\n", 
            message, filmename, ezxml_child(filme, "titulo")->txt,
            ezxml_child(filme, "resumo")->txt, ezxml_child(filme, "genero")->txt,
            ezxml_child(filme, "elenco")->txt, ezxml_child(filme, "status")->txt
            );
    }
    
    ezxml_free(l1); 
    return 1;    
}

int findFile(char *name, char *ID, char *message){
    ezxml_t l1 = ezxml_parse_file(name), filme, titulo, resumo, genero, elenco, status;
    
    const char *filmeId;
    memset(message, 0, TAM);
    sprintf(message, "Busca por ID: %s!\n", ID);
    
    for (filme = ezxml_child(l1, "filme"); filme; filme = filme->next) {
        filmeId = ezxml_attr(filme, "id");

        if (!strcmp(filmeId, ID)){ 
            sprintf(message, "%s\nID: %s\nTitulo: %s\nResumo: %s\nGenero: %s\nElenco: %s\nStatus: %s\n", 
                message, filmeId, ezxml_child(filme, "titulo")->txt,
                ezxml_child(filme, "resumo")->txt, ezxml_child(filme, "genero")->txt,
                ezxml_child(filme, "elenco")->txt, ezxml_child(filme, "status")->txt);

            goto encontrado;
        }
    }
    sprintf(message, "%s\nNão encontrado!\n", message);

    encontrado:
        sprintf(message, "%s\nBusca finalizada!\n\n", message);

    ezxml_free(l1);
    return 1;     
}

int toInsertFile(char name[], char title[], char abstract[], 
                char gender[], char cast[], char stat[], char texto[]){
    char *Id;
    char aux[3];

    ezxml_t new, child1, child2, child3, child4, child5; 

    ezxml_t l1 = ezxml_parse_file(name), filme, titulo, resumo, genero, elenco, status;
    const char *filmename;
    
    for (filme = ezxml_child(l1, "filme"); filme; filme = filme->next) {
        filmename = ezxml_attr(filme, "id");

        if (filme->next == NULL){
            
            /*   Create the root node 'new'  */
	        new = ezxml_new("filme");

            /*      Add first child to root node 'new'        */
            child1 = ezxml_add_child(new, "titulo", 0);
            child2 = ezxml_add_child(new, "resumo", 0);
            child3 = ezxml_add_child(new, "genero", 0);
            child4 = ezxml_add_child(new, "elenco", 0);
            child5 = ezxml_add_child(new, "status", 0);

            /*      Set value to node      */
            child1 = ezxml_set_txt(child1, title);
            child2 = ezxml_set_txt(child2, abstract);
            child3 = ezxml_set_txt(child3, gender);
            child4 = ezxml_set_txt(child4, cast);
            child5 = ezxml_set_txt(child5, stat);

            /*      Set attribute name to node d    */
            sprintf(aux, "%i", atoi(filmename) + 1); 
            Id = aux; 

            // para o id para int pra somar e coloca string aux e é apontado pro Id
            new = ezxml_set_attr(new, "id", Id);

            //printf("%s\n", ezxml_toxml(new));
        }
    }
    
    strcpy(texto, ezxml_toxml(new));
    ezxml_free(l1); 
    ezxml_free(new); 

    return 1;
}

/* int main(){
    char name[]= "streamings.xml";
    char *ID = "id";
    char texto[500];

    char title[]="Flash";
    char abstract[]="Filme americano com base no personagem homônimo da DC Comics";
    char elenco[]="Graant Gustin; Candice Patton";
    char gender[]="Aventura";
    char status[]="Disponivel";


    //readFile(name);
    printf("--------------------------------------------\n");
    findFile(name, "22");
    printf("--------------------------------------------\n");
    toInsertFile(name, title, abstract, gender, elenco, status, texto);

    readFile(name);
    printf("--------------------------------------------\n");
    insertEndFile(name, texto);
    return 0;
} */