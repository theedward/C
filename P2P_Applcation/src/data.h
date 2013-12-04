#ifndef P2P_DATA
#define P2P_DATA

int DEBUG = 1;

#define NO_BODY 0

#define MYSELF -1
#define SLOT_FREE 0
#define SLOT_PENDING 1

#define PEER_OK 0
#define PEER_JOIN_PENDING 1
#define PEER_OFFLINE 2

#define MAX_QUERY_LEN 100
#define MAX_QHIT_ENTRIES 5
#define MAX_ENTRIES 10
#define MAX_PEERS 5
#define MAX_QUERY_HISTORY 10

#define PROGRAM_STOP 0
#define PROGRAM_RUN 1

#define DATA_UNKNOWN 0
#define DATA_READY 1

#define PING_A_TTL 0x01

// Own data
uint32_t my_ip  = 0x5282080f;
uint16_t my_port = 12345;

// Hardcoded data of the bootstrap server
char * bootstrap_ip = "130.233.43.40";
char * bootstrap_port = "18347";
uint32_t bootstrap_ip_hex = 0x82e92b28;
uint16_t bootstrap_port_hex = 18347;

// Internal state data
int STATE = PROGRAM_RUN;

int bytes_sent = 0;
int bytes_recv = 0;

uint32_t CURRENT_MSG = 0;

int sockets[MAX_PEERS];

struct P2P_client_info {
	char stringip[INET6_ADDRSTRLEN];
	char stringport[6];
	uint32_t ip;
	uint16_t port;
	int status;
	int missed_pings;
};

struct P2P_entry {
	char name[MAX_QUERY_LEN];
	uint32_t value;
};

struct P2P_query_history {
	int status;
	char query[MAX_QUERY_LEN];
	int peer_from; // Or MYSELF if own query
	int peer_to;
	uint32_t msgid;
};

struct P2P_client_info peers[MAX_PEERS];

struct P2P_entry entries[MAX_ENTRIES];

struct P2P_query_history query_history[MAX_QUERY_HISTORY];

struct P2P_data_entry {
	int status;
	char name[MAX_QUERY_LEN];
	uint32_t data;
};

struct P2P_data_entry shared_data[MAX_ENTRIES];

#endif
