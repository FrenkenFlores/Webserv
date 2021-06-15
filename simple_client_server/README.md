# Instructions
compile server and client:
```shell
clang client.c -g client
clang server.c -g server
```
launch the server from one terminal and the client from another:
```shell
./server
Server is waiting for connection ...
```
```shell
./client localhost
connecting to ::1
client received: Hello from Frenken Flores!
```
