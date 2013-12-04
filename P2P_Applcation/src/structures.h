#ifndef P2P_structures
#define P2P_structures

#include "T-110_5150_reference_c_header.h"
#include "data.h"

//////////////////////////////////////////////////////////

struct P2P_pong {
    struct P2P_pong_front front;
    struct P2P_pong_entry entries[5];
};

struct P2P_query {
	char criteria[MAX_QUERY_LEN];
};

struct P2P_qhit_front {
	uint16_t size;
	uint16_t sbz;
};

struct P2P_qhit_entry {
	uint16_t id;
	uint16_t sbz;
	uint32_t value;
};

struct P2P_qhit {
	struct P2P_qhit_front front;
	struct P2P_qhit_entry entries[MAX_QHIT_ENTRIES];
};

//////////////////////////////////////////////////////////

struct P2P_ping_packet {
    struct P2P_h header;
};

struct P2P_pong_A_packet {
    struct P2P_h header;
};

struct P2P_pong_B_packet {
    struct P2P_h header;
    struct P2P_pong body;
};

struct P2P_join_request_packet {
    struct P2P_h header;
};

struct P2P_join_response_packet {
    struct P2P_h header;
    struct P2P_join body;
};

struct P2P_query_packet {
	struct P2P_h header;
	struct P2P_query body;
};

struct P2P_query_hit_packet {
	struct P2P_h header;
	struct P2P_qhit body;
};

struct P2P_bye_packet {
    struct P2P_h header;
};

#endif
