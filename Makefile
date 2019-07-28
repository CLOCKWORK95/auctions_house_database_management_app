all:
	gcc -g *.c -o program `mysql_config --cflags --include --libs`
clean:
	-rm program
