# Copied from https://stackoverflow.com/questions/19928965/mingw-makefile-with-or-without-msys-del-vs-rm
# for windows/linux cross compatibility
ifeq ($(OS),Windows_NT) 
RM = del /Q /F
CP = copy /Y
ifdef ComSpec
SHELL := $(ComSpec)
endif
ifdef COMSPEC
SHELL := $(COMSPEC)
endif
else
RM = rm -rf
CP = cp -f
endif
# End copied code

OPTS = -Wall -Wextra -g -pedantic

.PHONY:
	sq% irr%

sq%: voronoi1
	$(eval data = full)
	cat data/square_$*split.txt | ./voronoi1 data/dataset_$(data).csv data/polygon_square.txt output.txt | /mnt/c/Windows/py.exe visualisation.py

irr%: voronoi1
	$(eval data = full)
	 cat data/poly_$*split.txt | ./voronoi1 data/dataset_$(data).csv data/polygon_irregular.txt output.txt | /mnt/c/Windows/py.exe visualisation.py

voronoi1: main.o utils.o shape.o tower.o
	gcc $(OPTS) -o voronoi1 main.o utils.o shape.o tower.o

main.o: main.c utils.h shape.h tower.h
	gcc $(OPTS) -c -o main.o main.c

tower.o: tower.c tower.h shape.h utils.h
	gcc $(OPTS) -c -o tower.o tower.c

shape.o: shape.c shape.h utils.h
	gcc $(OPTS) -c -o shape.o shape.c

utils.o: utils.c utils.h
	gcc $(OPTS) -c -o utils.o utils.c

clean:
	-$(RM) voronoi1.exe
	-$(RM) voronoi1
	-$(RM) *.o
