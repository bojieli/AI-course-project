default:
	gcc -O2 -Wall -g -o p1 p1.c p1_main.c
	gcc -O2 -Wall -g -o p2 p2.c p2_main.c
clean:
	rm p1 p2
