all: gcc
clear: clr

COMP   := gcc
SERVER := server
CLIENT := client
EARG   := lpthread
STD    := gnu99

gcc:
	@$(COMP) $(SERVER).c -o $(SERVER) -$(EARG) -std=$(STD)
	@$(COMP) $(CLIENT).c -o $(CLIENT)
	@echo -e '\nCompilado! "server" e "client" criados!'
	@echo 'Use "./client" em outras abas'
	@echo 'Inicinado servidor!'
	@./$(SERVER)

clr:
	@echo 'Limpando'
	rm server client
	@echo 'Executaveis removidos!'
