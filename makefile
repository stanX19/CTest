SRCDIR		= srcs
SRCS		= $(addsuffix .cpp, $(addprefix $(SRCDIR)/, TemporaryFile UnitTest))

HEADER_DIR	= headers
HEADERS		= $(addprefix $(HEADER_DIR)/, UnitTest.hpp TemporaryFile.hpp)
OBJDIR		= objs
OBJDIRS		= $(sort $(dir $(OBJS)))
OBJS		= $(subst $(SRCDIR),$(OBJDIR),$(subst .cpp,.o,$(SRCS)))

AR			= ar rc
RM			= rm -rf
NAME		= unittest.a
IFLAGS		= -I. -I$(HEADER_DIR)

CC			= g++
CFLAGS		= -Wall -Wextra -Werror -fsanitize=address -g3

UP			= \033[1A
FLUSH		= \033[2K

run: $(NAME)
	$(CC) $(CFLAGS) $(IFLAGS) main.cpp unittest.a -o a.out && ./a.out

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