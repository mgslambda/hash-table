ht_test: src/hashtable.c test/ht_test.c
	mkdir -p bin
	gcc -o bin/ht_test src/hashtable.c test/ht_test.c

