parametrosCompilacao=-lpthread
arquivos=multi_partition.o verifica_particoes.o
all: multi_partition verifica_particoes

multi_partition: multi_partition.o verifica_particoes.o
	gcc -o multi_partition multi_partition.o verifica_particoes.o $(parametrosCompilacao)

verifica_particoes.o: verifica_particoes.h verifica_particoes.c
	gcc -c verifica_particoes.c $(parametrosCompilacao)

.o: .c
	gcc -c *.c $(parametrosCompilacao)

clean:
	rm -f *.o *.gch multi_partition verifica_particoes

purge: clean
	-rm -f multi_partition verifica_particoes