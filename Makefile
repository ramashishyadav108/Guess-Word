CC = g++
CFLAGS = -std=c++23 -Wall -I/usr/include/freetype2 -I/usr/include/mysql -I/usr/include/mysql 

LIBS = -lGL -lGLU -lglut -lfreetype -lmysqlclient -lmysqlcppconn
SRCS = main.cpp common.cpp hindi_game.cpp manipuri_game.cpp database_manager.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = game

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(EXEC)
	

