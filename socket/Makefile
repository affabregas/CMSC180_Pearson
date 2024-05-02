port = 53770
n = 25000

ser:
	gcc pearson4.c -pthread -lm
	./a.out $(n) $(port) 0

cli:
	gcc pearson4.c -pthread -lm
	./a.out $(n) $(port) 1