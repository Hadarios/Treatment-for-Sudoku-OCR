CC=gcc -fsanitize=address

CPPFLAGS= `pkg-config --cflags sdl` -MMD
CFLAGS= -Wall -Wextra -Werror -std=c99 -O3 -g3
LDFLAGS= -lm
LDLIBS= `pkg-config --libs sdl SDL_image`

SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)
EXEC= image

all: $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(CPPFLAGS) $(LDLIBS) -c -o $@ $<

$(EXEC) : $(OBJ)
	$(CC) -o $@ $^ -lm $(LDLIBS)
	

clean:
	@${RM} *.o
	@${RM} *.d
	@${RM} image
	@${RM} *.bmp

clear:
	@${RM} *.o
	@${RM} *.d
	@${RM} image
	@${RM} *.bmp
	@clear

# END
