FLAGS=-lm

CC=mpicc

SOURCE=tsp.c

RM=rm -f

EXEC=tsp

all: $(EXEC)

$(EXEC): $(SOURCE)
	$(CC) -O3 $(EXEC).c -c -o $(EXEC).o $(FLAGS)
	$(CC) -O3 $(EXEC).o -o $(EXEC) $(FLAGS)

run:
	mpirun --hostfile hosts.txt -np 2 ./$(EXEC) < entrada15

clean:
	$(RM) $(EXEC).o $(EXEC)
