gcc server.c -o server -lpthread
gcc client.c -o client
echo "Para executar os client => ./client"
echo "Iniciando servidor!"
./server
