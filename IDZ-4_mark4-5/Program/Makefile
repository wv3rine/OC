all: UDPEchoClientLoop UDPEchoServer-ForkN
UDPEchoClientLoop: UDPEchoClientLoop.c DieWithError.c
	gcc UDPEchoClientLoop.c DieWithError.c -o UDPEchoClientLoop
UDPEchoServer-ForkN: UDPEchoServer-ForkN.c DieWithError.c
	gcc UDPEchoServer.h UDPEchoServer-ForkN.c DieWithError.c \
	CreateUDPServerSocket.c \
	-pthread -o UDPEchoServer-ForkN
