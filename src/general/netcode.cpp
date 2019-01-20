#include <stdio.h>

#include "netcode.h"
#include "macros.h"
#include <sstream>



UDPnet::UDPnet() { exit = false; started = false; }

UDPnet::~UDPnet() { SDLNet_UDP_Close(local_socket); SDLNet_Quit(); }

bool UDPnet::quit() { return exit; }
bool UDPnet::initialized() { return started; }

bool UDPnet::open_port(uint16_t local_port) {
	local_socket = SDLNet_UDP_Open(local_port);
	if (local_socket != NULL)
		return false;
	return true;
}

bool UDPnet::init_local(uint16_t local_port, console *con) {
	cons = con;
	
	if (SDLNet_Init()) {
		cons->write_message("Cannot start network system", C_ERROR);
		return false;
	}
	else cons->write_message("Network initialized sucessfully", C_NORMAL);
	
	if (open_port(local_port)) {
		cons->write_message("Cannot open port", C_ERROR);
		return false;
	}
	else cons->write_message("Port open, waiting for connection", C_NORMAL);	

	started = true;
}


bool UDPnet::connect(const std::string ip, uint16_t port) {
	
	std::stringstream str;
	str <<  ip << ":" << port;

	if (SDLNet_ResolveHost(&remote_ip, ip.c_str(), port)) {
		cons->write_message("Failed to resolve address: " + str.str(), C_ERROR);
		return false;
	}
	else cons->write_message("Resolved address: " + str.str(), C_NORMAL);
	
	return true;
}

bool UDPnet::send_packet(std::string msg) {

	outgoing_packet = SDLNet_AllocPacket(512);
	outgoing_packet->address.host = remote_ip.host;
	outgoing_packet->address.port = remote_ip.port;

	memcpy(outgoing_packet->data, &msg, msg.size());
	outgoing_packet->len = msg.size();

	if (!SDLNet_UDP_Send(local_socket, -1, outgoing_packet)) {
		cons->write_message("Packet lost", C_ERROR);
		return false;
	}

	SDLNet_FreePacket(outgoing_packet);

	exit = true;
	return true;
}

std::string UDPnet::listen_packet() {
	std::string msg = "NULL";
	incoming_packet = SDLNet_AllocPacket(512);

	if (SDLNet_UDP_Recv(local_socket, incoming_packet)) {
		memcpy(&msg, incoming_packet->data, incoming_packet->len);
	}

	SDLNet_FreePacket(incoming_packet);

	return msg;
}
