## Projeto 1 - S.O.

### Universidade Estadual De Maringá

### Disciplina:
  * Sistemas Operacionais - Trab 1 - implementação
  
### Discentes:
  * RA: 107774 - Gabriel T. H. Santos 
  * RA: 103069 - Thiago I. Yasunaka 

### Informações do projeto:
Projeto possui o arquivo de servidor (server.c) e o do cliente (client.c), no qual realizam cadastro, listagem e busca de filmes.
Para a streaming de videos fora usada um arquivo XML, declarado como streming.xml, para percorrer de maneira mais direta se utilizou do parser ezxml para C, e com este então é criado funções para executar o acesso, alterações e complementos, functionsXML.c.

### Implementação
No servidor, para cada conexão aceita é declarada uma nova thread, e então nesta nova é executada as funções exigidas no trabalho.
Sendo separado em:
* Para a listagem completa dos filmes se utilizou do pipe para se comunir entre os processos do pai e do filho, passando para o filho executar tal serviço e então se completo passar os dados para o pai.
* Para a busca especifica é utilizada de fifo para a comunição entre pai e filho.
* Para um novo cadastro de filme, é usado do compartilhamento de memórias, passando para o filho executar e escrever na memória para se informar para o pai se fora inserido corretamente ou não, no pai então acessa esta memoria e dependendo do resultado informa se ocorreu tudo certo.

Sendo consequentemente em todos o uso do fork para se criar o filho e então a comunicação ocorrer.

## Execução
    
    $ make
    ------- another tab 
    $ ./client
    
Para executar, pode se usar do make para compilar tanto server.c quanto o client.c e consequentemente já executar o servidor. 
Para novas conexões use do .client em outras abas do terminal para iniciar diversas conexões.
