#pragma once
#include <string>
#include <SDL_net.h>


#include "console.h"

typedef struct {
	char header[4];
	int data_size;
	uint8_t* data;
}net_message;

class UDPnet {
	bool exit;
	bool started;
	bool conn;
	UDPsocket local_socket;
	IPaddress remote_ip;
	UDPpacket *incoming_packet;
	UDPpacket *outgoing_packet;
	console *cons;
public:
	UDPnet();
	~UDPnet();
	bool quit();
	bool initialized();
	bool connected();
	bool open_port(uint16_t local_port);
	bool init_local(uint16_t local_port, console *con);
	bool connect(const std::string ip, uint16_t port);
	bool send_packet(std::string msg);
	std::string listen_packet();
};


void start_receiving(int id, UDPnet &udpnet, console &console);