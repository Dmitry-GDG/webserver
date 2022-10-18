NAME =		$(shell grep 'define NAME' srcs/main.hpp	\
			| sed -e 's/^.*"\(.*\)".*/\1/')
BOT_NAME =		$(shell grep 'define CLIENT_NAME' srcs/main.hpp	\
			| sed -e 's/^.*"\(.*\)".*/\1/')
VER =		$(shell grep 'define CLIENT_VERSION' srcs/main.hpp	\
			| sed -e 's/^.*"\(.*\)".*/\1/')
AUTHORS =	$(shell grep 'define CLIENT_AUTHORS' srcs/main.hpp	\
			| sed -e 's/^.*"\(.*\)".*/\1/')
HEAD =		$(addprefix -I $(HEAD_DIR)/, $(sort $(dir $(SRC))))
HEAD_DIR =	$(SRC_DIR)
HEADS =		$(HEAD_DIR)/$(sort $(dir $(SRC)))/*.hpp

CC =		c++
CCFLAGS =	-Wall -Wextra -Werror 
CCFLAGS +=	-std=c++98
CCFLAGS +=	-pedantic-errors
CCFLAGS +=	-DDEBUGMODE
# CCFLAGS +=	-g -fsanitize=address

RM =		rm -Rf

MAIN =		main Client utils commands printDebugmode

SRC = 	$(addsuffix .cpp, $(MAIN))

SRC_DIR = srcs

OBJ = $(addprefix $(OBJ_DIR)/, $(sort $(dir $(SRC))))

OBJ_DIR = objs

OBJ_SUBDIR =  $(addprefix $(OBJ_DIR)/,$(sort $(dir $(SRC))))

OBJ_BUILD = $(addprefix $(OBJ_DIR)/, $(SRC:cpp=o))

BLUS 	= \033[2;34m
GRNS	= \033[2;32m
REDS	= \033[2;31m
VIOLETS	= \033[2;35m
YELS	= \033[2;33m
TICK	= \xE2\x9C\x94

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJ_BUILD) Makefile $(HEADS)
	@$(CC) $(CCFLAGS) $(HEAD) $(DEFINES) -o $(NAME) $(OBJ_BUILD)
	@echo "\r    $(GRNS)$(NAME) $(BOT_NAME) v.$(VER) by $(AUTHORS)\n    $(YELS)$(TICK)$(GRNS) was compiled!$(END)\n\
	\t$(BLUS)Usage: ./$(NAME) <host> <port>"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp  $(HEADS)
	@printf "$(VIOLETS)Generating $(NAME)'s objects... %-38.38s\r" $@
	@$(CC) $(CCFLAGS) $(HEAD) $(DEFINES) -o $@ -c $<

$(OBJ_DIR):
	@mkdir -p $(OBJ_SUBDIR)

clean:
	@echo "$(REDS)clean ...$(END)"
	@rm -rf $(OBJ_DIR)
	@echo "\t$(GRNS) $(TICK) $(NAME) was cleaned!$(END)"

fclean: clean
	@echo "$(REDS)fclean ...$(END)"
	@rm -f $(NAME)
	@echo "\t$(GRNS) $(TICK) $(NAME) was fcleaned!$(END)"

ffclean: 
	@echo "$(REDS)fclean ...$(END)"
	@rm -f $(NAME)
	@echo "\t$(GRNS) $(TICK) $(NAME) was fcleaned!$(END)"

re: fclean all

.PHONY			:	all clean fclean re
