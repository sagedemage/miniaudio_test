CC=g++
CFLAGS = -ldl -lpthread -lm -msse2 -mavx2 -I include

out:
	$(CC) $(CFLAGS) src/main.cpp -o bin/out

run:
	./bin/out

clean:
	rm -f bin/out
