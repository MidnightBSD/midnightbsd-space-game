
PROG_CXX=	midnightbsd-space-game
CXXFLAGS+=	-std=c++20 -I/usr/local/include -L/usr/local/lib

SRCS= Allenroids.cpp		ConfigureScreen.cpp		StandardElements.cpp	TitleScreen.cpp		sound.cpp \
	Asteroid.cpp	BadGuy.cpp	Crystal.cpp	GameObject.cpp	Kerblooie.cpp	Player.cpp	Platform.cpp	Renderer.cpp	Shot.cpp	Spaceship.cpp	Starfield.cpp	UiText.cpp

LDADD= -lGL -lSDL2 -lSDL2_ttf

MK_MAN=	no

.include <bsd.prog.mk>
