CC	 			= gcc
CFLAGS			= -Wall -Werror -pedantic -std=c99

OBJS			= wavinfo.o wav.o wavvol.o wavnorm.o io.o wavrev.o wavecho.o wavwide.o wavcatmix.o
LIBS_OBJ 		= wav.o io.o
EXEC			= wavinfo wavvol wavnorm wavrev wavecho wavwide wavcatmix
DEPENDENCIES 	= wav.h io.h
LINKS			= wavcat wavmix


wavinfo_OBJS	= wavinfo.o $(LIBS_OBJ) 
wavvol_OBJS		= wavvol.o $(LIBS_OBJ)
wavnorm_OBJS	= wavnorm.o $(LIBS_OBJ)
wavrev_OBJS		= wavrev.o $(LIBS_OBJ)
wavecho_OBJS	= wavecho.o $(LIBS_OBJ)
wavwide_OBJS	= wavwide.o $(LIBS_OBJ)
wavcatmix_OBJS	= wavcatmix.o $(LIBS_OBJ)

all: $(EXEC) $(LINKS)

wavinfo: $(wavinfo_OBJS)
wavvol: $(wavvol_OBJS)
wavnorm: $(wavnorm_OBJS)
wavrev: $(wavrev_OBJS)
wavecho: $(wavecho_OBJS)
wavwide: $(wavwide_OBJS)

wavcatmix: $(wavcatmix_OBJS)

wavcat:
	ln -s -f ./wavcatmix ./wavcat
wavmix:
	ln -s -f ./wavcatmix ./wavmix

%.o: %.c $(DEPENDENCIES)
	$(CC) -c $(CFLAGS) -o $@ $< 

clean:
	rm -f $(OBJS)

purge:
	rm -f $(OBJS) $(EXEC) $(LINKS)