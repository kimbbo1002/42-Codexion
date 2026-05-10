NAME = codexion
CC = cc
CFLAGS = -Wall -Werror -Wextra -pthread
INC_DIR = inc/
SRC_DIR = src/
OBJ_DIR = obj/

SRC_FILES =	coder_compile.c\
			coder.c\
			dongle.c\
			hub.c\
			init.c\
			main.c\
			monitor.c\
			parsing.c\
			scheduler.c\
			utils.c
SRCS = $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJS = $(addprefix $(OBJ_DIR), $(SRC_FILES:.c=.o))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c $(INC_DIR)codexion.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re