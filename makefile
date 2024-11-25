parametrosCompilacao=-lpthread
arquivos=multi_partition.o 
all: multi_partition

multi_partition: multi_partition.o
	gcc -o multi_partition multi_partition.o $(parametrosCompilacao)

.o: .c
	gcc -c *.c $(parametrosCompilacao)

clean:
	rm -f *.o *.gch multi_partition

purge: clean
	-rm -f multi_partition