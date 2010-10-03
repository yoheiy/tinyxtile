all: tinytile xnobar

%: %.c
	$(CC) -o $@ $< -lX11
