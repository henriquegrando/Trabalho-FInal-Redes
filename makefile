all:
	@ g++ chat.cpp -o chat -std=c++11 -pthread
clean:
	@ rm chat