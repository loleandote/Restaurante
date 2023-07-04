DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall -ggdb
LDFLAGS := -lpthread -lrt
CC := gcc

#all : dirs manager barbero cliente
all : dirs manager receptor cocinero empaquetador cajero

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

manager: $(DIROBJ)manager.o $(DIROBJ)semaforoI.o $(DIROBJ)memoriaI.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)

receptor: $(DIROBJ)receptor.o $(DIROBJ)semaforoI.o $(DIROBJ)memoriaI.o
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)

cocinero: $(DIROBJ)cocinero.o $(DIROBJ)semaforoI.o $(DIROBJ)memoriaI.o
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)

empaquetador: $(DIROBJ)empaquetador.o $(DIROBJ)semaforoI.o $(DIROBJ)memoriaI.o
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)

cajero: $(DIROBJ)cajero.o $(DIROBJ)semaforoI.o $(DIROBJ)memoriaI.o
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)


$(DIROBJ)%.o: $(DIRSRC)%.c
	$(CC) $(CFLAGS) $^ -o $@

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~
