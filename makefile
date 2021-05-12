#!/bin/bash

gcc server.c -o server -lpthread
gcc client.c -o client
echo "Para executar os client em outro terminal => ./client"
echo "Iniciando servidor!"
./server
