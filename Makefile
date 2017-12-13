
PROG_CXX=	midnightbsd-space-game
CXXFLAGS+=	-I/usr/local/include -I/usr/local/include/SDL -L/usr/local/lib  -L/usr/local/lib/SDL/

SRCS= Allenroids.cpp		ConfigureScreen.cpp		StandardElements.cpp	TitleScreen.cpp		sound.cpp \
	Asteroid.cpp	BadGuy.cpp	Crystal.cpp	GameObject.cpp	Kerblooie.cpp	Player.cpp	Shot.cpp	Spaceship.cpp	Starfield.cpp 

LDADD= -lglut -lGL -lGLU -lSDL

NO_MAN=

.include <bsd.prog.mk>
