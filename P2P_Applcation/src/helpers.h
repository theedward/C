#ifndef P2P_HELPERS
#define P2P_HELPERS

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "structures.h"
#include "data.h"
#include <sys/fcntl.h>

//*************************************************************
//                                                            *
//                  DEBUG & PRINT & STUFF                     *
//                                                            *
//*************************************************************

void print_debug(const char * msg) {
	if (DEBUG) {
		printf("DBG: ");
		printf("%s", msg);
		printf("\n");
	}
}

void debug_message_to(const char * msg, int socket) {
	if (DEBUG) {
		printf("DBG: %s --> %d\n", msg, socket);
	}
}

void debug_message_from(const char * msg, int socket) {
	if (DEBUG) {
		printf("DBG:\t\t\t%s <-- %d\n", msg, socket);
	}
}

void ip_hex_to_dec(uint32_t * ip, int * b1, int * b2, int * b3, int * b4) {
    *b1 = (int) (*ip & 0xff);
    *b2 = (int) ((*ip >> 8) & 0xff);
    *b3 = (int) ((*ip >> 16) & 0xff);
    *b4 = (int) ((*ip >> 24) & 0xff);
}

void print_header_inner(struct P2P_h * h, int complete) {
	if (complete == 0) {
		printf("\tMSG:\t\t0x0%x\n", h->msg_type);
		printf("\tLEN:\t\t%d\n", h->length);
		printf("\tMSGID:\t\t0x%x\n", h->msg_id);
	} else {
		int b1, b2, b3, b4;
		ip_hex_to_dec(&h->org_ip, &b1, &b2, &b3, &b4);
		printf("\tVersion:\t%x\n", h->version);
		printf("\tTTL:\t\t%x\n", h->ttl);
		printf("\tMSG:\t\t0x0%x\n", h->msg_type);
		printf("\tRES:\t\t%x\n", h->reserved);
		printf("\tSND PORT:\t%d\n", h->org_port);
		printf("\tLEN:\t\t%d\n", h->length);
		printf("\tIP:\t\t%d.%d.%d.%d (0x%x)\n", b1, b2, b3, b4, h->org_ip);
		printf("\tMSGID:\t\t0x%x\n", h->msg_id);
	}
}

void print_header(struct P2P_h * h) {
	print_header_inner(h, 1);
}

void print_header_simple(struct P2P_h *	 h) {
	print_header_inner(h, 0);
}

void process_from_network(struct P2P_h * h) {
    h->org_port = ntohs(h->org_port);
    h->length = ntohs(h->length);
    h->org_ip = ntohl(h->org_ip);
    h->msg_id = ntohl(h->msg_id);
}

void process_to_network(struct P2P_h * h) {
    h->org_port = htons(h->org_port);
    h->length = htons(h->length);
    h->org_ip = htonl(h->org_ip);
    h->msg_id = htonl(h->msg_id);
}

void print_header_to_network(struct P2P_h * h) {
	process_from_network(h);
	print_header_inner(h, 1);
	process_to_network(h);
}

void print_qhit(struct P2P_query_hit_packet * p) {
	printf("Query hit:\n");
	printf("Header:\n");
	print_header(&p->header);
	printf("Body:\n");
	printf("\tSize:\t\t%d\n", p->body.front.size);
	int i = 0;
	for (i = 0; i < p->body.front.size; ++i) {
		printf("\tID[%d]:\t\t%x\n", i, p->body.entries[i].value);
	}
}

int getHistorySlot() {
	int i = 0;
	for (i = 0; i < MAX_QUERY_HISTORY; ++i) {
		if (query_history[i].status == SLOT_FREE) return i;
	}
	return -1;
}

void insertQueryHistory(struct P2P_query_packet * p, int from, int to) {
	int slot = getHistorySlot();
	if (slot == -1) {
		print_debug("!!! Error getting history slot.");
		return;
	}
	
	query_history[slot].status = SLOT_PENDING;
	strcpy(&query_history[slot].query[0], &p->body.criteria[0]);
	query_history[slot].peer_from = from;
	query_history[slot].peer_to = to;
	query_history[slot].msgid = p->header.msg_id;
}

void processQHitbody(struct P2P_query_hit_packet * p) {
	p->body.front.size = ntohs(p->body.front.size);
	int i = 0;
	for (i = 0; i < MAX_QHIT_ENTRIES; ++i) {
		p->body.entries[i].id = htons(p->body.entries[i].id);
		p->body.entries[i].value = htonl(p->body.entries[i].value);
	}
}

void printQueryHistory() {
	int nothing = 1;
	int i = 0;
	for (i = 0; i < MAX_QUERY_HISTORY; ++i) {
		if (query_history[i].status == SLOT_PENDING) {
			nothing = 0;
			
			printf("----QUERY SLOT %d----\n", i);
			printf("Query: \"%s\"\n", &query_history[i].query[0]);
			if (query_history[i].peer_from == MYSELF) {
				printf("ME ---> %d\n", query_history[i].peer_to);
			} else {
				printf(" %d ---> %d\n", query_history[i].peer_from, query_history[i].peer_to);
			}
			printf("MSGID: %x\n", query_history[i].msgid);
			printf("\n");
		}
	}
	if (nothing) printf("No pending queries.\n");
}

int markQueryHit(struct P2P_query_hit_packet * p) {
	int found = -2;
	int i = 0;
	for (i = 0; i < MAX_QUERY_HISTORY; ++i) {
		if (p->header.msg_id == query_history[i].msgid) {
			query_history[i].status = SLOT_FREE;
			found = query_history[i].peer_from;
		}
	}
	return found;
}

void printPeers() {
	int i = 0;
	printf("---CLIENT INFO---\n\n");
	for (i = 0; i < MAX_PEERS; ++i) {
		if (sockets[i] != -1) {
			printf("Client %d:\n", i);
			if (peers[i].status == PEER_OK) {
				printf("\tStatus: OK\n");
			} else if (peers[i].status == PEER_JOIN_PENDING) {
				printf("\tStatus:  P\n");
			} else {
				printf("\tStatus: ER\n");
			}
			printf("\tIP:  %s:%d (%x)\n\n", peers[i].stringip, peers[i].port, peers[i].ip);
		}
	}
}

void printPongBody(struct P2P_pong_B_packet * p) {
	printf("Pong body:\n");
	printf("Entries: %d\n", p->body.front.entry_size);
	int i = 0;
	for (i = 0; i < p->body.front.entry_size; ++i) {
		int b1,b2,b3,b4;
		ip_hex_to_dec(&p->body.entries[i].ip, &b1, &b2, &b3, &b4);
		printf("\tEntry %d: %d.%d.%d.%d:%d\n", i, b1, b2, b3, b4, p->body.entries[i].port);
	}
}

void printSharedKeys() {
	int printed = 0;
	int i = 0;
	printf("Shared data\n");
	printf("-------------------------------------------------\n");
	for (i = 0; i < MAX_ENTRIES; ++i) {
		if (shared_data[i].status == DATA_READY) {
			printf("\tData %d: \"%s\" => 0x%x\n", i, shared_data[i].name, shared_data[i].data);
			printed++;
		}
	}
	if (printed == 0) printf("\tNo shared data found;\n");
}

//*************************************************************
//                                                            *
//                      GENERATORS                            *
//                                                            *
//*************************************************************


uint32_t getMessageID() {
    uint32_t temp = (uint32_t)my_port;
    CURRENT_MSG += 1;
    return ((my_ip ^ my_port) ^ (temp << 16)) + CURRENT_MSG;
}

int newPeerID() {
	int i = 0;
	for(i = 0; i < MAX_PEERS; ++i) {
		if (sockets[i] == -1) return i;
	}
	return -1;
}

//*************************************************************
//                                                            *
//                      BOOTSTRAPPING                         *
//                                                            *
//*************************************************************

int getBootstrapSocket(char * ip, char * port) {
    struct addrinfo hints;
    struct addrinfo *res;  // will point to the results
    
    printf("Querying for %s:%s...\n",ip, port); 
    
    //Fill server info
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    int status = getaddrinfo(ip, port, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return -1;
    }
    
    //Create socket
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) printf("Socket error\n");
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    connect(sockfd, res->ai_addr, res->ai_addrlen);

    unsigned int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags|O_NDELAY);
    
    return sockfd;
}

int getListenSocket() {
	struct addrinfo hints;
    struct addrinfo *res;  // will point to the results
    
    //Fill server info
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    int status = getaddrinfo(NULL, "12345", &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return -1;
    }
    
    //Create socket
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) printf("Socket error\n");
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    
    unsigned int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags|O_NDELAY);
    
    return sockfd;
}

//*************************************************************
//                                                            *
//                PACKET FORGERY FUNCTIONS                    *
//                                                            *
//*************************************************************

struct P2P_h getGenericHeader(uint32_t org_ip, uint16_t org_port, uint8_t ttl, uint8_t msg_type, uint32_t msgid, uint16_t len) {
    struct P2P_h h;
    h.version = 0x1;
    h.ttl = ttl;
    h.msg_type = msg_type;
    h.reserved = 0x0;
    h.org_port = org_port;
    h.length = len;
    h.org_ip = org_ip;
    h.msg_id = msgid;
    return h;
}

struct P2P_h getHeader(uint8_t ttl, uint8_t msg_type, uint32_t msgid, uint16_t len) {
    return getGenericHeader(my_ip, my_port, ttl, msg_type, msgid, len);
}

////////////////////////////////////////////////////////////////////////

void fillPongBody(struct P2P_pong_B_packet * p, int peer_from) {
	memset(&p->body, 0, sizeof p->body);
	
	int allocated = 0;
	
	int i = 0;
	for (i = 0; i < MAX_PEERS && allocated < 5; ++i) {
		if (sockets[i] != -1 && i != peer_from) {
			p->body.entries[allocated].ip = peers[i].ip;
			p->body.entries[allocated].port = peers[i].port;
		}
	}
	
	// Set header length!
	p->body.front.entry_size = allocated;
	p->header.length = htons(4+8*allocated); // For now, because it is empty.
}

void processPongBodyToNetwork(struct P2P_pong_B_packet * p) {
	p->body.front.entry_size = htons(p->body.front.entry_size);
	int i = 0;
	for (i = 0; i < 5; i++) {
		p->body.entries[i].ip = htonl(p->body.entries[i].ip);
		p->body.entries[i].port = htons(p->body.entries[i].port);
	}
}

void processPongBodyFromNetwork(struct P2P_pong_B_packet * p) {
	p->body.front.entry_size = ntohs(p->body.front.entry_size);
	int i = 0;
	for (i = 0; i < 5; i++) {
		//p->body.entries[i].ip = ntohl(p->body.entries[i].ip);
		p->body.entries[i].port = ntohs(p->body.entries[i].port);
	}
}

int peerHasIP(uint32_t ip) {
	int i = 0;
	for (i = 0; i < MAX_PEERS; ++i) {
		if (sockets[i] != -1 && peers[i].ip == ip) return 1;
	}
	return 0;
}

void ip_hex_to_string(uint32_t * hex, char * str) {
	int b1,b2,b3,b4;
	ip_hex_to_dec(hex, &b1, &b2, &b3, &b4);
	sprintf(str, "%d.%d.%d.%d", b1, b2, b3, b4);
}

////////////////////////////////////////////////////////////////////////

struct P2P_join_request_packet createJoinRequest() {
    struct P2P_join_request_packet p;
    p.header = getHeader(MAX_TTL, MSG_JOIN, getMessageID(), NO_BODY);
    return p;
}

struct P2P_join_response_packet createJoinResponse(uint32_t msgid) {
    struct P2P_join_response_packet p;
    p.header = getHeader(MAX_TTL, MSG_JOIN, msgid, 2);
    p.body.status = htons(0x0200);
    return p;
}

struct P2P_ping_packet createPingPacket_A() {
    struct P2P_ping_packet p;
    p.header = getHeader(PING_A_TTL, MSG_PING, getMessageID(), NO_BODY);
    return p;
}

struct P2P_ping_packet createPingPacket_B() {
    struct P2P_ping_packet p;
    p.header = getHeader(MAX_TTL, MSG_PING, getMessageID(), NO_BODY);
    return p;
}

struct P2P_pong_A_packet createPongPacket_A(uint32_t msgid) {
    struct P2P_pong_A_packet p;
    p.header = getHeader(PING_A_TTL, MSG_PONG, msgid, NO_BODY);
    return p;
}

struct P2P_pong_B_packet createPongPacket_B(uint32_t msgid, int peer_to) {
    struct P2P_pong_B_packet p;
    p.header = getHeader(MAX_TTL, MSG_PONG, msgid, NO_BODY);
    fillPongBody(&p, peer_to);
    processPongBodyToNetwork(&p);
    return p;
}

struct P2P_query_packet createQueryPacket(char * query) {
	struct P2P_query_packet p;
	p.header = getHeader(MAX_TTL, MSG_QUERY, getMessageID(), strlen(query));
	strcpy(&p.body.criteria[0], query);
	return p;
}

struct P2P_query_hit_packet createQueryHitPacket(uint32_t msgid, int q_hits[MAX_ENTRIES], int hits) {
	struct P2P_query_hit_packet p;
	p.header = getHeader(MAX_TTL, MSG_QHIT, msgid, 4+(hits*8));
	memset(&p.body, 0, sizeof p.body);
	int i = 0;
	for (i = 0; i < MAX_ENTRIES; ++i) {
		if (q_hits[i] == 1) {
			if (p.body.front.size < 5) {
				p.body.entries[p.body.front.size].id = i;
				p.body.entries[p.body.front.size].value = shared_data[i].data;
				p.body.front.size += 1;
			}
		}
	}
	return p;
}

struct P2P_bye_packet createByePacket() {
    struct P2P_bye_packet p;
    p.header = getHeader(MAX_TTL, MSG_BYE, getMessageID(), NO_BODY);
    return p;
}

#endif
