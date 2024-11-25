all:
	@	gcc ./src/main.c -o ./bin/main -O1 -Wall -I /usr/local/include -L/usr/local/lib -lraylib
	@	./bin/main
