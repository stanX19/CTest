SRCDIR		= srcs
SRCS		= $(wildcard $(SRCDIR)/*.cpp)

HEADER_DIR	= headers
HEADERS		= $(wildcard $(HEADER_DIR)/*.hpp)
OBJDIR		= objs
OBJDIRS		= $(sort $(dir $(OBJS)))
OBJS		= $(subst $(SRCDIR),$(OBJDIR),$(subst .cpp,.o,$(SRCS)))

AR			= ar rc
RM			= rm -rf
NAME		= unittest.a
IFLAGS		= -I. -I$(HEADER_DIR)

CC			= g++
CFLAGS		= -Wall -Wextra -Werror -fsanitize=address -g3 -std=c++17

UP			= \033[1A
FLUSH		= \033[2K

run: $(NAME)
	$(CC) $(CFLAGS) $(IFLAGS) main.cpp $(NAME) -o a.out && ./a.out

all: $(NAME)

$(NAME): $(OBJDIRS) $(OBJS) $(HEADERS)
	$(AR) $(NAME) $(OBJS)
	@echo "$(UP)$(FLUSH)$(UP)"

$(OBJDIRS):
	mkdir -p $@
	@echo "$(UP)$(FLUSH)$(UP)"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS) | $(OBJDIRS)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@
	@echo "$(UP)$(FLUSH)$(UP)"

clean:
	@$(RM) $(OBJDIR)

fclean: clean
	@$(RM) $(NAME)
	@$(RM) ./a.out

re: fclean all

.PHONY: all clean fclean re
