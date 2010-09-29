all: tinytile

tinytile: tinytile.c
	$(CC) -o $@ $< -lX11
