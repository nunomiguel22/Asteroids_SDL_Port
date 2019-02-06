#pragma once
#include <string>
#include <SDL_net.h>
#include <time.h>
#include "ship.h"


typedef struct {
	std::string header;
	unsigned int timer_tick;
	std::string ip;
	uint16_t port;
	player playern;

}net_message;

class UDPnet {
	bool server_start;

	bool exit;
	bool connected;

	UDPsocket local_socket;
	uint16_t local_port;
	std::string public_address;
	IPaddress remote_address;
	
	UDPpacket *incoming_packet;
	UDPpacket *outgoing_packet;

public:
	UDPnet();
	~UDPnet();

	bool server_is_open() const;
	bool port_is_open() const;
	bool is_connected() const;
	void close();

	std::string get_public_ip() const;
	uint16_t get_port() const;

	bool port_open(uint16_t local_port);
	void port_close();

	bool fecth_public_ip();
	void set_localhost();

	void server_open();
	void server_close();

	bool resolve_ip(const std::string ip, uint16_t port);

	bool send_packet(net_message &msg);
	void listen_packet(net_message &msg);



	bool quit();
	
	
	

};

