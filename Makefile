all: libprocesshider.so

libprocesshider.so: processhider.c
	echo $(NAMES)
	echo $(INDEXS)

	gcc -D 'NAMES=$(NAMES)' -D 'INDEXS=$(INDEXS)' -Wall -fPIC  -shared -o libprocesshider.so config.h processhider.c hide_dlsym.c

.PHONY clean :
	rm -f libprocesshider.so