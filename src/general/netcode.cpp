#include <stdio.h>

#include "netcode.h"
#include "macros.h"
#include <sstream>

//TODO Rework, no need to alloc and free every operation

UDPnet::UDPnet() { exit = false; started = false; conn = false; }

UDPnet::~UDPnet() { 
	if (started)
		SDLNet_UDP_Close(local_socket); 
}

bool UDPnet::quit() { return exit; }
bool UDPnet::initialized() { return started; }
bool UDPnet::connected() { return conn; }

bool UDPnet::open_port(uint16_t local_port) {
	local_socket = SDLNet_UDP_Open(local_port);
	if (local_socket == nullptr)
		return false;
	return true;
}

bool UDPnet::init_local(uint16_t local_port, console *con) {
	cons = con;
	
	if (!open_port(local_port)) {
		cons->write("Cannot open port", C_ERROR);
		return false;
	}
	else cons->write("Port open, waiting for connection", C_NORMAL);	

	started = true;
	return true;
}


bool UDPnet::connect(const std::string ip, uint16_t port) {
	
	std::stringstream str;
	str <<  ip << ":" << port;

	IPaddress cbind;
	cbind.port = port;
	SDLNet_ResolveHost(&cbind, NULL, port);
	SDLNet_UDP_Bind(local_socket, 0, &cbind);

	if (SDLNet_ResolveHost(&remote_ip, ip.c_str(), port)) {
		cons->write("Failed to resolve address: " + str.str(), C_ERROR);
		return false;
	}
	else cons->write("Resolved address: " + str.str(), C_NORMAL);
	
	conn = true;

	return true;
}

bool UDPnet::send_packet(std::string msg) {

	outgoing_packet = SDLNet_AllocPacket(512);
	outgoing_packet->address.host = remote_ip.host;
	outgoing_packet->address.port = remote_ip.port;


	memcpy(outgoing_packet->data, &msg, msg.size());
	outgoing_packet->len = msg.size();

	if (!SDLNet_UDP_Send(local_socket, -1, outgoing_packet)) {
		cons->write("Packet lost", C_ERROR);
		return false;
	}

	SDLNet_FreePacket(outgoing_packet);

	exit = true;
	return true;
}

std::string UDPnet::listen_packet() {
	char *msg = (char *) malloc (512);
	incoming_packet = SDLNet_AllocPacket(512);

	if (SDLNet_UDP_Recv(local_socket, incoming_packet)) 
		strncpy_s(msg, 512, (char *)incoming_packet->data, incoming_packet->len);
	else strncpy_s(msg, 512 ,"NULL", 4);
	
	std::string fin (msg);

	free(msg);
	SDLNet_FreePacket(incoming_packet);

	return fin;
}
