NAME =		$(shell grep 'define NAME' srcs/main.hpp	\
			| sed -e 's/^.*"\(.*\)".*/\1/')
WEBSERV_NAME =		$(shell grep 'define WEBSERV_NAME' srcs/main.hpp	\
			| sed -e 's/^.*"\(.*\)".*/\1/')
VER =		$(shell grep 'define WEBSERV_VERSION' srcs/main.hpp	\
			| sed -e 's/^.*"\(.*\)".*/\1/')
AUTHORS =	$(shell grep 'define WEBSERV_AUTHORS' srcs/main.hpp	\
			| sed -e 's/^.*"\(.*\)".*/\1/')
# HEAD =		-I include $(addprefix -I $(SRC_DIR)/, $(sort $(dir $(SRC))))
HEAD =		$(addprefix -I $(HEAD_DIR)/, $(sort $(dir $(SRC))))
HEAD_DIR =	$(SRC_DIR)
# HEADS =		$(HEAD_DIR)/*.hpp
HEADS =		$(HEAD_DIR)/$(sort $(dir $(SRC)))/*.hpp

CC =		c++
CCFLAGS =	-Wall -Wextra -Werror 
CCFLAGS +=	-std=c++98
CCFLAGS +=	-pedantic-errors
CCFLAGS +=	-DDEBUGMODE
# CCFLAGS +=	-g -fsanitize=address
RM =		rm -Rf

# FILES
MAIN =		main ServerRouter utilites parserServerConfigFile Server Connection
# WEBSERVCONFIG =	lexer parser
# AUTOINDEX =	AutoIndexGenerator
# CGI =		CgiHandler
# CONFIG =	Config ConfigHelperFunctions ConfigReader ConfigServer RequestConfig
# REQUEST =	Request RequestMembers
# RESPONSE =	Response ResponseHeader
# SERVER =	Server Cluster
# TOOLS =		list_sort ntoh string pathIsFile

SRC = 	$(addsuffix .cpp, $(MAIN))
# SRC +=	$(addprefix webservconfig/, $(addsuffix .cpp, $(WEBSERVCONFIG)))
# $(addprefix autoindex/, $(addsuffix .cpp, $(AUTOINDEX))) \
# $(addprefix cgi/, $(addsuffix .cpp, $(CGI))) \
# $(addprefix config/, $(addsuffix .cpp, $(CONFIG))) \
# $(addprefix request/, $(addsuffix .cpp, $(REQUEST))) \
# $(addprefix response/, $(addsuffix .cpp, $(RESPONSE))) \
# $(addprefix server/, $(addsuffix .cpp, $(SERVER))) \
# $(addprefix utils/, $(addsuffix .cpp, $(TOOLS))) \

SRC_DIR = srcs

OBJ = $(addprefix $(OBJ_DIR)/, $(sort $(dir $(SRC))))

# OBJ = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(MAIN))) \
# 	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(AUTOINDEX))) \
# 	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(CGI))) \
# 	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(CONFIG))) \
# 	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(REQUEST))) \
# 	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(RESPONSE))) \
# 	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SERVER))) \
# 	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(TOOLS))) \

OBJ_DIR = objs

OBJ_SUBDIR =  $(addprefix $(OBJ_DIR)/,$(sort $(dir $(SRC))))

# OBJ_SUBDIR = $(addprefix $(OBJ_DIR)/, autoindex cgi config request response server utils)

OBJ_BUILD = $(addprefix $(OBJ_DIR)/, $(SRC:cpp=o))

# BONUS_SRC_DIR = ./bonus
# BONUS_MAIN = main Bot utils CommandParced commands
# BONUS_NAME =		$(shell grep 'define NAME' bonus/srcs/main.hpp	\
# 			| sed -e 's/^.*"\(.*\)".*/\1/')
# BONUS_BOT_NAME =		$(shell grep 'define BOT_NAME' bonus/srcs/main.hpp	\
# 			| sed -e 's/^.*"\(.*\)".*/\1/')
# BONUS_VER =		$(shell grep 'define BOT_VERSION' bonus/srcs/main.hpp	\
# 			| sed -e 's/^.*"\(.*\)".*/\1/')
# BONUS_AUTHORS =	$(shell grep 'define BOT_AUTHORS' bonus/srcs/main.hpp	\
# 			| sed -e 's/^.*"\(.*\)".*/\1/')

# BLUE 	= \033[0;34m
# BLUB 	= \033[1;34m
BLUS 	= \033[2;34m
# GRN		= \033[0;32m
GRNS	= \033[2;32m
# RED		= \033[0;31m
REDS	= \033[2;31m
# VIOLET	= \033[0;35m
VIOLETS	= \033[2;35m
# YEL		= \033[0;33m
# YELB	= \033[1;33m
YELS	= \033[2;33m
# END		= \033[0m
TICK	= \xE2\x9C\x94
NC 		= \033[0m

# MAKING
all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJ_BUILD) Makefile $(HEADS)
#	@echo "\n"
#	@echo "$(GRN)Compiling webserv...\n"
	@$(CC) $(CCFLAGS) $(HEAD) $(DEFINES) -o $(NAME) $(OBJ_BUILD)
	@echo "\r    $(GRNS)$(NAME) $(IRCSERV_NAME) v.$(VER) by $(AUTHORS)\n    $(YELS)$(TICK)$(GRNS) was compiled!$(END)\n\
	\t$(BLUS)Usage: ./$(NAME) [config_file_name] $(NC)"
#	@echo "\n\033[0mDone !"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp  $(HEADS)  Makefile
	@printf "$(VIOLETS)Generating $(NAME)'s objects... %-38.38s\r" $@
	@$(CC) $(CCFLAGS) $(HEAD) $(DEFINES) -o $@ -c $<

$(OBJ_DIR):
#	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_SUBDIR)

# CLEANING
clean:
#	@echo "\n\033[0;31mDeleting objects..."
	@echo "$(REDS)clean ...$(END)"
	@rm -rf $(OBJ_DIR)
#	@rm -rf test_us/root
#	@rm -rf client
#	@rm -rf YoupiBanane/put_here
#	@echo "\033[0m"
	@echo "\t$(GRNS) $(TICK) $(NAME) was cleaned!$(END)] $(NC)"
#	@make -C bonus clean

fclean: clean
#	@echo "\033[0;31mDeleting executable..."
	@echo "$(REDS)fclean ...$(END)"
	@rm -f $(NAME)
#	@echo "\033[0m"
#	@echo "\033[0mDone !"
	@echo "\t$(GRNS) $(TICK) $(NAME) was fcleaned!$(END)] $(NC)"
#	@make -C bonus ffclean

re: fclean all
#	@make -C bonus fre


.PHONY			:	all clean fclean re





# NAME =		webserv
# VER =		$(shell grep 'define WEBSERV_VERSION' srcs/main.hpp	\
# 			| sed -e 's/^.*"\(.*\)".*/\1/')
# AUTHORS =	$(shell grep 'define WEBSERV_AUTHORS' srcs/main.hpp	\
# 			| sed -e 's/^.*"\(.*\)".*/\1/')
# # HEAD =		-I include $(addprefix -I $(SRC_DIR)/, $(sort $(dir $(SRC))))
# HEAD =		$(addprefix -I $(SRC_DIR)/, $(sort $(dir $(SRC))))

# CC =		c++
# CCFLAGS =	-Wall -Wextra -Werror -std=c++98
# # CCFLAGS +=	-pedantic-errors
# # CCFLAGS +=	-DDEBUG
# # CCFLAGS +=	-g -fsanitize=address
# RM =		rm -Rf

# # FILES
# MAIN =			main
# WEBSERVCONFIG =	lexer parser
# # AUTOINDEX =	AutoIndexGenerator
# # CGI =		CgiHandler
# # CONFIG =	Config ConfigHelperFunctions ConfigReader ConfigServer RequestConfig
# # REQUEST =	Request RequestMembers
# # RESPONSE =	Response ResponseHeader
# # SERVER =	Server Cluster
# # TOOLS =		list_sort ntoh string pathIsFile

# SRC = 	$(addsuffix .cpp, $(MAIN))
# SRC +=	$(addprefix webservconfig/, $(addsuffix .cpp, $(WEBSERVCONFIG)))
# # $(addprefix autoindex/, $(addsuffix .cpp, $(AUTOINDEX))) \
# # $(addprefix cgi/, $(addsuffix .cpp, $(CGI))) \
# # $(addprefix config/, $(addsuffix .cpp, $(CONFIG))) \
# # $(addprefix request/, $(addsuffix .cpp, $(REQUEST))) \
# # $(addprefix response/, $(addsuffix .cpp, $(RESPONSE))) \
# # $(addprefix server/, $(addsuffix .cpp, $(SERVER))) \
# # $(addprefix utils/, $(addsuffix .cpp, $(TOOLS))) \

# SRC_DIR = srcs

# OBJ = $(addprefix $(OBJ_DIR)/, $(sort $(dir $(SRC))))

# # OBJ = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(MAIN))) \
# # 	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(AUTOINDEX))) \
# # 	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(CGI))) \
# # 	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(CONFIG))) \
# # 	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(REQUEST))) \
# # 	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(RESPONSE))) \
# # 	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SERVER))) \
# # 	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(TOOLS))) \

# OBJ_DIR = objs

# OBJ_SUBDIR =  $(addprefix $(OBJ_DIR)/,$(sort $(dir $(SRC))))

# # OBJ_SUBDIR = $(addprefix $(OBJ_DIR)/, autoindex cgi config request response server utils)

# OBJ_BUILD = $(addprefix $(OBJ_DIR)/, $(SRC:cpp=o))

# GRN		= \033[0;32m
# RED		= \033[0;31m
# YEL		= \033[0;33m
# YELB	= \033[1;33m
# END		= \033[0m
# TICK	= \xE2\x9C\x94

# # MAKING
# all: $(NAME)

# $(NAME): $(LIBFT) $(OBJ_DIR) $(OBJ_BUILD) Makefile
# #	@echo "\n"
# #	@echo "$(GRN)Compiling webserv...\n"
# 	@$(CC) $(CCFLAGS) $(HEAD) $(DEFINES) -o $(NAME) $(OBJ_BUILD)
# 	@echo "\n\t$(GRN)$(NAME) v.$(VER) \n\tby $(AUTHORS)\n\t$(TICK) was compiled!$(END)\n\t$(YELB) try it: ./$(NAME) test_mac/mac.conf$(END)"
# #	@echo "\n\033[0mDone !"

# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
# 	@printf "$(YEL)Generating webserv objects... %-38.38s\r" $@
# 	@$(CC) $(CCFLAGS) $(HEAD) $(DEFINES) -o $@ -c $<

# $(OBJ_DIR):
# #	@mkdir -p $(OBJ_DIR)
# 	@mkdir -p $(OBJ_SUBDIR)

# # CLEANING
# clean:
# #	@echo "\n\033[0;31mDeleting objects..."
# 	@echo "$(RED)clean ...$(END)"
# 	@rm -rf $(OBJ_DIR)
# 	@rm -rf test_us/root
# 	@rm -rf client
# 	@rm -rf YoupiBanane/put_here
# #	@echo "\033[0m"
# 	@echo "\t$(GRN) $(TICK) $(NAME) was cleaned!$(END)"

# fclean: clean
# #	@echo "\033[0;31mDeleting executable..."
# 	@echo "$(RED)fclean ...$(END)"
# 	@rm -f $(NAME)
# #	@echo "\033[0m"
# #	@echo "\033[0mDone !"
# 	@echo "\t$(GRN) $(TICK) $(NAME) was fcleaned!$(END)"

# re: fclean all


# .PHONY			:	all clean fclean re



# # SRC =  

# # SRCS_DIR = srcs/

# # SRCS = $(addprefix $(SRCS_DIR),$(SRC))

# # # CGISRC = srcs/mainCGI.cpp
# # # CGI    = cgi/my_cgi.test

# # OBJS_DIR = objs/

# # OBJS = $(addprefix $(OBJS_DIR),$(SRCS:.cpp=.o))

# # # DEPS = $(OBJS:%.o=%.d)

# # # sort - это сортировка и удаление повторяющихся слов
# # # $(dir $(OBJS)) - Из каждого имени файла, перечисленного в именах, выделяет имя каталога, где этот файл расположен. 
# # # Именем каталога считается часть имени до последнего встреченного символа `/' (включая и этот символ). 
# # # Если имя файла не содержит символов `/', его именем каталога считается строка `./'.
# # DIRLIST = $(sort $(dir $(OBJS)))

# # all				:	$(NAME)

# # $(NAME)			:	$(DIRLIST) $(OBJS)
# # 					@$(CC) $(CGISRC) -I$(HEAD) -o $(CGI)
# # 					@$(CC) $(CCFLAGS) -I$(HEAD) -o $(NAME) $(OBJS)
# # 					@printf "\e[37;5;44m             Webserver is ready 🤗              \e[0m\n"

# # $(DIRLIST)		:	$(OBJS_DIR)
# # 					@mkdir -p $@

# # $(OBJS_DIR)		:
# # 					@mkdir -p $(OBJS_DIR)

# # $(OBJS_DIR)/%.o	:	%.cpp Makefile
# # 					@$(CC) $(CCFLAGS) -I$(HEAD) -MMD -o $@ -c $<

# # # -include $(DEPS)

# # clean			:
# # 					@$(RM) $(OBJS_DIR) $(CGI)
# # 					@printf "\e[30;5;47m                 Clean done 🧹                  \e[0m\n"
	
# # fclean			:	clean
# # 					@$(RM) $(NAME)
# # 					@printf "\e[30;5;42m                Fclean done 🧹                  \e[0m\n"

# # re				:	fclean all
