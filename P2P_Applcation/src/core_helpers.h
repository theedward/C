#ifndef P2P_CORE_HELPERS
#define P2P_CORE_HELPERS

#include "structures.h"
#include "helpers.h"
#include "data.h"

/**
 * Return 0 on success, -1 otherwise.
 **/
int getJoinResponseBody(int socket, struct P2P_h * h, struct P2P_join_response_packet * p) {
	p->header = *h;
	bytes_recv = recv(socket, (void *)&p->body, 2, 0);
	//~ if (bytes_recv != 2) {
		//~ print_debug("!!! Error on recieving packet body.");
		//~ return -1;
	//~ }
	p->body.status = ntohs(p->body.status);
	return 0;
}

void getBPongBody(int socket, struct P2P_pong_B_packet * p) {
	bytes_recv = recv(socket, (void *)&p->body, p->header.length, 0);
	processPongBodyFromNetwork(p);
}

void addNewPeers(struct P2P_pong_B_packet * p) {
	int i = 0;
	for (i = 0; i < p->body.front.entry_size; ++i) {
		if (peerHasIP(p->body.entries[i].ip) == 0) {
				int new_id = newPeerID();
				if (new_id == -1) {
					printf("No available slots for peers.");
				} else {
					ip_hex_to_string(&p->body.entries[i].ip, &peers[new_id].stringip[0]);
					sprintf(&peers[new_id].stringport[0], "%d", p->body.entries[i].port);
					int sockfd = getBootstrapSocket(&peers[new_id].stringip[0], &peers[new_id].stringport[0]);
					
					if (sockfd != -1) {
						sockets[new_id] = sockfd;
						peers[new_id].status = PEER_JOIN_PENDING;
						printf("Connected successfully to %s:%s\n", &peers[new_id].stringip[0], &peers[new_id].stringport[0]);
						
						// Join request packet (just a header)
						struct P2P_join_request_packet join_request = createJoinRequest();
						
						// Send join request
						bytes_sent = send(sockets[new_id], (void *)&join_request, 16, 0);
					} else {
						printf("!!! Could not get socket to new client\n");
					}					
				}
		}
	}
}

void getQueryBody(int socket, struct P2P_query_packet * p) {
	bytes_recv = recv(socket, (void *)&p->body, p->header.length, 0);
}

void respondQuery(int socket, struct P2P_query_packet * p, int id) {
	// First we check if we can hit query
	int hits = 0;
	int q_hits[MAX_ENTRIES];
	memset(&q_hits, 0, sizeof q_hits);
	int i = 0;
	for (i = 0; i < MAX_ENTRIES; ++i) {
		if (shared_data[i].status == DATA_READY) {
			if (strcmp(&shared_data[i].name[0],&p->body.criteria[0]) == 0) {
				// We have a hit!
				q_hits[i] = 1;
				hits += 1;
			}
		}
	}
	
	// If we have hits, send QHIT
	if (hits > 0) {
		debug_message_to("QHIT", id);
		struct P2P_query_hit_packet hit = createQueryHitPacket(p->header.msg_id, q_hits, hits);
		bytes_sent = send(socket, (void *)&hit, 16+hit.header.length, 0);
	}
	
	// Finally, forward query (if TTL > 1)
	if (p->header.ttl > 1) {
		p->header.ttl -= 1;
		for (i = 0; i < MAX_PEERS; ++i) {
			if (sockets[i] != -1 && peers[i].status == PEER_OK && i != id) {
				bytes_sent = send(sockets[i], (void *)p, 16+p->header.length, 0);
				debug_message_to("QUERY (FWD)", i);
			}
		}
	}	
}

#endif
