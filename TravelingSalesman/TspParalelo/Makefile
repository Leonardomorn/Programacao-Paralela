FLAGS=-lm -fopenmp

CC=gcc

SOURCE=tsp.c

RM=rm -f

EXEC=tsp

all: $(EXEC)

$(EXEC): $(SOURCE)
	$(CC) -O3 $(EXEC).c -c -o $(EXEC).o $(FLAGS)
	$(CC) -O3 $(EXEC).o -o $(EXEC) $(FLAGS)

run:
	./$(EXEC)

clean:
	$(RM) $(EXEC).o $(EXEC)
