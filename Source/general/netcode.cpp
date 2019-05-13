#include <stdio.h>
#include <sstream>
#include <fstream>
#include <windows.h>

#include "netcode.h"
#include "macros.h"

#include <iostream> //DEBUG
//TODO Rework, no need to alloc and free every operation

UDPnet::UDPnet() { exit = false; connected = false; server_start = false; local_socket = nullptr; }

UDPnet::~UDPnet() { 
	if (local_socket != nullptr)
		SDLNet_UDP_Close(local_socket);
	server_close();
	close();
}

bool UDPnet::server_is_open() const { return server_start; }
bool UDPnet::port_is_open() const {
	if (local_socket == nullptr)
		return false;
	return true;
}

std::string UDPnet::get_public_ip() const { return public_address; }
uint16_t UDPnet::get_port() const { return local_port; }




bool UDPnet::quit() { return exit; }

bool UDPnet::is_connected() const { return connected; }
void UDPnet::close() { connected = false; }

bool UDPnet::port_open(uint16_t local_port) {
	local_socket = SDLNet_UDP_Open(local_port);

	if (local_socket == nullptr)
		return false;

	this->local_port = local_port;
	incoming_packet = SDLNet_AllocPacket(512);
	outgoing_packet = SDLNet_AllocPacket(512);

	return true;
}
void UDPnet::port_close() { 
	SDLNet_UDP_Close(local_socket); 
	SDLNet_FreePacket(incoming_packet);
	SDLNet_FreePacket(outgoing_packet);
}

void UDPnet::server_open() { server_start = true; }
void UDPnet::server_close() { server_start = false; }

bool UDPnet::fecth_public_ip() {

	STARTUPINFO info = { sizeof(info) };
	PROCESS_INFORMATION processInfo;
	CreateProcess("gamefiles\\iptool.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);
	WaitForSingleObject(processInfo.hProcess, INFINITE);
	CloseHandle(processInfo.hProcess);

	std::ifstream file;

	file.open("jsonLog.txt");

	if (!file.is_open()) 	
		return false;

	file.ignore(9);
	getline(file, public_address, '\"');

	file.close();
	return true;
}

void UDPnet::set_localhost() { public_address = "127.0.0.1"; }

bool UDPnet::resolve_ip(const std::string ip, uint16_t port) {
	
	std::stringstream str;
	str <<  ip << ":" << port;

	if (SDLNet_ResolveHost(&remote_address, ip.c_str(), port)) 
		return false;
	
	connected = true;
	return true;
}

bool UDPnet::send_packet(net_message &msg) {

	
	outgoing_packet->address.host = remote_address.host;
	outgoing_packet->address.port = remote_address.port;

	int size = sizeof(msg);
	memcpy(outgoing_packet->data, &msg, size);
	outgoing_packet->len = size;

	if (!SDLNet_UDP_Send(local_socket, -1, outgoing_packet)) { //SINGLE CHANNEL??
		//cons->write("Packet lost", C_ERROR);
		return false;
	}
	exit = true;
	return true;
}

void UDPnet::listen_packet(net_message &msg) {

	if (SDLNet_UDP_Recv(local_socket, incoming_packet)) 
		memcpy(&msg, incoming_packet->data, incoming_packet->len);
	else msg.header = "NULL";
	
}
