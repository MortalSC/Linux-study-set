.PHONY:all
all:Cal_client Cal_server

Cal_client:cal_client.cc 
	g++ -o $@ $^ -std=c++11 -ljsoncpp
Cal_server:cal_server.cc 	
	g++ -o $@ $^ -std=c++11 -ljsoncpp -lpthread

.PHONY:clean
clean:
	rm -f Cal_client Cal_server