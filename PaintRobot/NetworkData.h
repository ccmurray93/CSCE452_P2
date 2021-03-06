
#ifndef _packet_types_
#define _packet_types_

#define MAX_PACKET_SIZE 1000000

enum PacketTypes {

	PACKET_ERROR = 0,

	INIT_CONNECTION = 1,

	ACTION_EVENT = 2,

};

struct Packet {

	unsigned int packet_type;

	//these two are just here to see what works
	char contents;
	char *command;
	double many_vals[30];

	//TODO when this is a part of the main code, have packets contain PaintArm objects
	//This will allow all relevant info to transfer in one go

	void serialize(char * data) {
		memcpy(data, this, sizeof(Packet));
	}

	void deserialize(char * data) {
		memcpy(this, data, sizeof(Packet));
	}
};

#endif

