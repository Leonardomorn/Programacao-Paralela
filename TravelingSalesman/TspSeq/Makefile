FLAGS=-lm

CC=gcc

RM=rm -f

EXEC=tsp

all: $(EXEC)

$(EXEC):
	$(CC) -O3 $(EXEC).c -c -o $(EXEC).o $(FLAGS)
	$(CC) -O3 $(EXEC).o -o $(EXEC) $(FLAGS)

run:
	./$(EXEC)

clean:
	$(RM) $(EXEC).o $(EXEC)
