#ifndef P2P_CORE
#define P2P_CORE

#include "structures.h"
#include "helpers.h"
#include "core_helpers.h"
#include "data.h"

void processPacket(int socket, struct P2P_h header, int id) {
	//print_header(&header);
	process_from_network(&header);
	//print_header(&header);
	
	switch (header.msg_type) {
		case MSG_PING:
			if (header.ttl == 1) {
				debug_message_from("PING:A", id);
				// Reply with PONG:A
				struct P2P_pong_A_packet reply = createPongPacket_A(header.msg_id);
				bytes_sent = send(socket, (void *)&reply, 16, 0);
				debug_message_to("PONG:A", id);
			} else {
				debug_message_from("PING:B",id);
				//Reply with PONG:B
				struct P2P_pong_B_packet reply = createPongPacket_B(header.msg_id, id);
				bytes_sent = send(socket, (void *)&reply, sizeof reply, 0);
				debug_message_to("PONG:B", id);
				//print_header_to_network(&reply.header);
			}
			break;
		case MSG_PONG:
			if (header.length == 0) {
				peers[id].missed_pings -= 1;
				char message[25];
				sprintf(&message[0], "PONG:A (%d)", peers[id].missed_pings);
				debug_message_from(&message[0], id);
			} else {
				struct P2P_pong_B_packet pong;
				getBPongBody(socket, &pong);
				debug_message_from("PONG:B", id);
				printPongBody(&pong);
				addNewPeers(&pong); 
			}
			break;
		case MSG_BYE:
			debug_message_from("BYE", id);
			peers[id].status = PEER_OFFLINE;
			sockets[id] = -1;
			break;
		case MSG_JOIN:
			if (header.length == 0) { // JOIN REQUEST
				debug_message_from("JOIN:REQ", id);
				peers[id].ip = header.org_ip;
				peers[id].port = header.org_port;
				peers[id].status = PEER_OK;
				struct P2P_join_response_packet response = createJoinResponse(header.msg_id);
				bytes_sent = send(socket, (void *)&response, 18, 0);
				debug_message_to("JOIN:RES (OK)", id);
				int b1, b2, b3, b4;
				ip_hex_to_dec(&peers[id].ip, &b1, &b2, &b3, &b4);
				if (DEBUG != 0) printf("DBG:\t\t\tNew peer: %d.%d.%d.%d:%d\n", b1, b2, b3, b4, peers[id].port);
			} else if (header.length == 2) { // JOIN RESPONSE
				debug_message_from("JOIN:RES", id);
				struct P2P_join_response_packet join_response;
				
				if (getJoinResponseBody(socket, &header, &join_response) == 0) {
					if (join_response.body.status == 0x0200) {
						debug_message_from("Join request accepted", id);
						peers[id].status = PEER_OK;
					} else {
						print_debug("\t\t\t   Unknown status code.");
					}
				}
			} else {
				print_debug("!!! Error: Unexpected length of JOIN packet.");
			}
			break;
		case MSG_QUERY:
		{
			struct P2P_query_packet p;
			p.header = header;
			getQueryBody(socket, &p); // TODO THIS IS ALL UNTESTED
			char msg[100];
			sprintf(&msg[0], "QUERY: \"%s\"",&p.body.criteria[0]);
			debug_message_from(&msg[0], id);
			respondQuery(socket, &p, id); // TODO UNTIL HERE
			break;
		}
		case MSG_QHIT:
			debug_message_from("QHIT", id);
			struct P2P_query_hit_packet qhit;
			qhit.header = header;
			bytes_recv = recv(socket, (void *)&qhit.body, header.length, 0);
			process_from_network(&qhit.header);
			processQHitbody(&qhit);
			int res = markQueryHit(&qhit); // TODO MOSTLY UNTESTED
			if (res == -2) {
				// Query did not hit, drop.
				debug_message_from("ERR: QHIT did not match QUERY, dropping.", id);
			} else if (res == -1) {
				// Own query hit, done.
				debug_message_from("OK: QHIT responded own query.", id);
				print_qhit(&qhit);
			} else {
				// Alien query hit, forward.
				debug_message_from("OK: QHIT from other peer, forwarding...", id);
				bytes_sent = send(sockets[res], (void *)&qhit,16+qhit.header.length, 0);
				debug_message_to("QHIT (FWD)", res);
			}
			break;
		default:
			printf("!!! Unknown packet header:\n");
			print_header(&header);
			printf("Dropping packet.\n");
	}
}

void *recievePacket() {
	struct P2P_h header[MAX_PEERS];
	int i = 0;
	while (STATE != PROGRAM_STOP) {
		if (sockets[i] != -1) {
			bytes_recv = recv(sockets[i], (void *)&header[i], 16, 0);
			if (bytes_recv != -1) {
				processPacket(sockets[i], header[i], i);
			}
		}
		i += 1;
		if (i >= MAX_PEERS) i = 0;
	}
		
}

void *sendPings() {
	int i = 0;
	while (STATE != PROGRAM_STOP) {
		if (sockets[i] != -1 && peers[i].status == PEER_OK) {
			struct P2P_ping_packet p = createPingPacket_A();
			bytes_sent = send(sockets[i], (void *)&p, 16, 0);
			peers[i].missed_pings += 1;
			char message[25];
			sprintf(&message[0], "PING:A (%d)", peers[i].missed_pings);
			debug_message_to(&message[0], i);
		}
		i += 1;
		if (i >= MAX_PEERS) {
			i = 0;
			sleep(5);
		}
	} 
}

void *listenConnections() {
	int listen_socket = getListenSocket();
	
	if(listen(listen_socket, 5) < 0) {
		printf("!!! Error on listen socket!");
	}
	
	//print_debug("Listening...\n");
	
	struct sockaddr_storage their_addr;
    socklen_t addr_size;
    addr_size = sizeof their_addr;
    void *addr;
	
	while(STATE != PROGRAM_STOP) {
		int new_socket = accept(listen_socket, (struct sockaddr *)&their_addr, &addr_size);
		if (new_socket != -1) {
			struct sockaddr_in * caddr = ((struct sockaddr_in *)((struct sockaddr *)&their_addr));
			addr = &(caddr->sin_addr);
			char ipstr[INET6_ADDRSTRLEN];
			inet_ntop(their_addr.ss_family, addr, ipstr, sizeof ipstr);
			if (DEBUG != 0) printf("Connection from %s\n", ipstr);
			
			int new_id = 3;
			if (new_id != -1) {
				if (DEBUG != 0) printf("DBG: New socket: %d\n", new_id);
				sockets[new_id] = new_socket;
				peers[new_id].status = PEER_JOIN_PENDING;
				peers[new_id].missed_pings = 0;
				strcpy(&peers[new_id].stringip[0], &ipstr[0]);
			} else {
				debug_message_to("Closed connection", -1);
				close(new_socket);
			}
		}
	}
}

#endif
