ALLEGRO_VERSION=5.0.10
MINGW_VERSION=4.7.0
FOLDER=C:

FOLDER_NAME=\allegro-$(ALLEGRO_VERSION)-mingw-$(MINGW_VERSION)
PATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)
LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
INCLUDE_ALLEGRO=\include

all: rtype.exe


rtype.exe: rtype.o
	gcc -o rtype.exe rtype.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

rtype.o: rtype.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c rtype.c		
	
	
clean:
	del rtype.o
	del rtype.exe
