
PROG_CXX=	midnightbsd-space-game
CXXFLAGS+=	-std=c++20 -I/usr/local/include -L/usr/local/lib

SRCS= Allenroids.cpp		ConfigureScreen.cpp		StandardElements.cpp	TitleScreen.cpp		sound.cpp \
	Asteroid.cpp	BadGuy.cpp	Crystal.cpp	GameObject.cpp	Kerblooie.cpp	Player.cpp	Shot.cpp	Spaceship.cpp	Starfield.cpp 

LDADD= -lglut -lGL -lGLU -lSDL2

NO_MAN=

.include <bsd.prog.mk>
