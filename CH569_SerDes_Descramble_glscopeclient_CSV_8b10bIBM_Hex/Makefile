
RM=rm -rf

CC=gcc
CFLAGS=-w -Wall -ansi -std=c99 -O3
LDFLAGS=
EXEC=CH569_SerDes_Descramble_glscopeclient_CSV_8b10bIBM_Hex
STRIP_EXE=strip

OBJ=CH569_SerDes_Descramble_glscopeclient_CSV_8b10bIBM_Hex.o csv.o crc32_SATA.o

all: $(EXEC)

CH569_SerDes_Descramble_glscopeclient_CSV_8b10bIBM_Hex: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Dependencies

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	$(RM) *.o
