.PHONY:all
all:client server

client:cal_client.cc 
	g++ -o $@ $^ -std=c++11
server:cal_server.cc 	
	g++ -o $@ $^ -std=c++11 -lpthread

.PHONY:clean
clean:
	rm -f server client