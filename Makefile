all:
	g++ -g *.cpp -o hw4 -std=c++11 -Wall -lGLEW -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -ldl -ljpeg

clean:
	rm -rf hw4
