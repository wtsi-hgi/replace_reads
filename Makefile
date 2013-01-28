CC = c++
CFLAGS = -I../htslib -std=gnu++0x -DPOSIX_SOURCE -g
OBJ = main.o
LDFLAGS = -L../htslib/htslib -lz -lhts

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

replace_reads: $(OBJ)
	$(CC) -o replace_reads $(LDFLAGS) $(OBJ)

all: replace_reads

