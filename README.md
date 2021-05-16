## Projeto 1 - S.O.

### Universidade Estadual De Maringá

### Disciplina:
  * Sistemas Operacionais - Trab 1 - implementação
  
### Discentes:
  * RA: 107774 - Gabriel T. H. Santos 
  * RA: 103069 - Thiago I. Yasunaka 

### Informações do projeto:
Projeto possui o arquivo de servidor (server.c) e o do cliente (client.c), no qual realizam cadastro, listagem e busca de filmes.
Para a streaming de videos fora usada um arquivo XML, declarado como streaming.xml, para percorrer de maneira mais direta se utilizou do parser ezxml para C, e com este então é criado funções para executar o acesso, alterações e complementos, functionsXML.c.

### Implementação
No servidor, para cada conexão aceita é declarada uma nova thread, e então nesta nova é executada as funções exigidas no trabalho.
Sendo separado em:
* Para a listagem completa dos filmes, se utilizou do pipe para se comunicar entre o processos do pai e do filho, passando para o filho executar tal serviço e então, se completo, passado os dados para o pai.
* Para a busca especifica pelo ID é utilizada do fifo para a comunição entre pai e filho.
* Para um novo cadastro de filme, é usado do compartilhamento de memórias, passando para o filho executar e escrever na memória informando para o pai se foi inserido corretamente ou não, no pai então, se acessa esta memoria e dependendo do resultado informa se ocorreu tudo certo.

Sendo consequentemente em todos, o uso do fork para se criar o filho e então a comunicação ocorrer.

## Execução
    
    $ make
    ------- another tab 
    $ ./client
    
Para executar, pode se usar do make para compilar tanto server.c quanto o client.c e consequentemente já executar o servidor. 
Para novas conexões usasse do ./client em outras abas do terminal para iniciar diversas conexões.
