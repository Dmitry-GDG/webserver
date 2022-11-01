NAME =		$(shell grep 'define NAME' srcs/main.hpp	\
			| sed -e 's/^.*"\(.*\)".*/\1/')
WEBSERV_NAME =		$(shell grep 'define WEBSERV_NAME' srcs/main.hpp	\
			| sed -e 's/^.*"\(.*\)".*/\1/')
VER =		$(shell grep 'define WEBSERV_VERSION' srcs/main.hpp	\
			| sed -e 's/^.*"\(.*\)".*/\1/')
AUTHORS =	$(shell grep 'define WEBSERV_AUTHORS' srcs/main.hpp	\
			| sed -e 's/^.*"\(.*\)".*/\1/')
HEAD =		$(addprefix -I $(HEAD_DIR)/, $(sort $(dir $(SRC))))
HEAD_DIR =	$(SRC_DIR)
HEADS =		$(HEAD_DIR)/$(sort $(dir $(SRC)))/*.hpp

CC =		c++
CCFLAGS =	-Wall -Wextra -Werror 
CCFLAGS +=	-std=c++98
CCFLAGS +=	-pedantic-errors
CCFLAGS +=	-DDEBUGMODE
RM =		rm -Rf

MAIN =		main ServerRouter utilities parserServerConfigFile Server ServerRouter_parseInputData \
			printDebugmode ServerRouter_GET ServerRouter_POST ServerRouter_DELETE ServerRouter_addStatusCodePage \
			ServerRouter_createAutoindex

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
NC 		= \033[0m

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJ_BUILD) Makefile $(HEADS)
	@$(CC) $(CCFLAGS) $(HEAD) $(DEFINES) -o $(NAME) $(OBJ_BUILD)
	@echo "\r    $(GRNS)$(NAME) $(IRCSERV_NAME) v.$(VER) by $(AUTHORS)\n    $(YELS)$(TICK)$(GRNS) was compiled!$(END)\n\
	\t$(BLUS)Usage: ./$(NAME) [config_file_name] $(NC)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp  $(HEADS)  Makefile
	@printf "$(VIOLETS)Generating $(NAME)'s objects... %-38.38s\r" $@
	@$(CC) $(CCFLAGS) $(HEAD) $(DEFINES) -o $@ -c $<

$(OBJ_DIR):
	@mkdir -p $(OBJ_SUBDIR)

clean:
	@echo "$(REDS)clean ...$(END)"
	@rm -rf $(OBJ_DIR)
	@echo "\t$(GRNS) $(TICK) $(NAME) was cleaned!$(END) $(NC)"

fclean: clean
	@echo "$(REDS)fclean ...$(END)"
	@rm -f $(NAME)
	@echo "\t$(GRNS) $(TICK) $(NAME) was fcleaned!$(END) $(NC)"

re: fclean all

.PHONY			:	all clean fclean re
