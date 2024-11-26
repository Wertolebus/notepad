all:
	@	gcc ./src/main.c -o ./pad -O1 -Wall -I /usr/local/include -L/usr/local/lib -lraylib
	@	./pad 
