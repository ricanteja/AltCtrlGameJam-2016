SFML_INC = -I C:/Users/Ricky/MinGW-Libs/SFML/include
SFML_LIB = -L C:/Users/Ricky/MinGW-Libs/SFML/lib -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system

all: $(SOURCE)
	g++ GreenThumb.cpp $(SFML_INC) $(SFML_LIB) -static-libgcc -lpthread -O2 -o ../bin/GreenThumb.exe