#include "structures.h"
#include "helpers.h"
#include "core.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "data.h"
#include <pthread.h>

void sig_handler(int signo) {
	if (signo == SIGINT) {
		int new_id;
		int sockfd;
		struct P2P_ping_packet ping;
		DEBUG = 0;
		int command = 1;
		printf("\nApplication menu\n");
		printf("-------------------------------------------------\n");
		printf("1: Continue (do nothing)\n");
		printf("2: Issue query\n");
		printf("3: View query history\n");
		printf("4: View peers info\n");
		printf("5: Connect to new peer\n");
		printf("6: Send PING:B\n");
		printf("7: View shared keys\n");
		printf("0: Quit\n");
		printf("> ");
		scanf("%d",&command);
		switch(command) {
			case 0:
				STATE = PROGRAM_STOP;
				break;
			case 1:
				printf("Continuing...\n");
				break;
			case 2:
				printf("Enter query: ");
				char query[MAX_QUERY_LEN];
				scanf("%s", &query[0]);
				struct P2P_query_packet query_packet = createQueryPacket(&query[0]);
				int i = 0;
				for (i = 0; i < MAX_PEERS; ++i) {
					if (sockets[i] != -1 && peers[i].status == PEER_OK) {
						process_to_network(&query_packet.header);
						bytes_sent = send(sockets[i], (void *)&query_packet, 16+strlen(&query[0]), 0);
						DEBUG = 1;
						insertQueryHistory(&query_packet, MYSELF, i);
						debug_message_to("QUERY", i);
						print_header_to_network(&query_packet.header);
					}
				}
				break;
			case 3:
				printQueryHistory();
				break;
			case 4:
				printPeers();
				break;
			case 5:
				new_id = 3;
				if (new_id == -1) {
					printf("No available slots for peers.");
				} else {
					printf("Connecting to new peer:\n");
					printf("Enter IP: ");
					scanf("%s", &peers[new_id].stringip[0]);
					// TODO Convert string IP to hex and save in struct
					printf("Enter port: ");
					scanf("%s", &peers[new_id].stringport[0]);
					
					sockfd = getBootstrapSocket(&peers[new_id].stringip[0], &peers[new_id].stringport[0]);
					
					if (sockfd != -1) {
						//sockets[new_id] = sockfd;
						peers[new_id].status = PEER_JOIN_PENDING;
						printf("Connected successfully to %s:%s\n", &peers[new_id].stringip[0], &peers[new_id].stringport[0]);
						
						// Join request packet (just a header)
						struct P2P_join_request_packet join_request = createJoinRequest();
						
						// Send join request
						bytes_sent = send(sockets[new_id], (void *)&join_request, 16, 0);
						printf("SENT JOIN REQUEST\n");
					} else {
						printf("!!! Could not get socket to new client\n");
					}					
				}
				break;
			case 6:
				ping = createPingPacket_B();
				new_id = 0;
				DEBUG = 1;
				for (new_id = 0; new_id < MAX_PEERS; ++new_id) {
					if (sockets[new_id] != -1 && peers[new_id].status == PEER_OK) {
						bytes_sent = send(sockets[new_id], (void *)&ping, 16, 0);
						debug_message_to("PING:B", new_id);
					}
				}
				break;
			case 7:
				printSharedKeys();
				break;
			default:
				printf("Unknown command.\n");
		}
		printf("-------------------------------------------------\n");
		DEBUG = 1;
	}
}

void init_peers() {
	int i = 0;
	for (i = 0; i < MAX_PEERS; i++) {
		sockets[i] = -1;
	}
}

void init_shared() {
	memset(&shared_data, 0, sizeof shared_data);
	
	shared_data[0].status = DATA_READY;
	strcpy(shared_data[0].name, "error");
	shared_data[0].data = 0xdeadbeef;
}

int main() {
	signal(SIGINT, sig_handler);
	
	printf("Program started. Press Ctrl-C for menu.\n");
	
	init_peers();
	init_shared();
	
    // Bootstrap socket to the server
    sockets[0] = getBootstrapSocket(bootstrap_ip, bootstrap_port);
    strcpy(&peers[0].stringip[0], bootstrap_ip);
    peers[0].ip = bootstrap_ip_hex;
    peers[0].port =  bootstrap_port_hex;
    peers[0].missed_pings = 0;
    peers[0].status = PEER_JOIN_PENDING;
    
    // Join request packet (just a header)
    struct P2P_join_request_packet join_request = createJoinRequest();
    
    // Send join request
    bytes_sent = send(sockets[0], (void *)&join_request, 16, 0);
    if (bytes_sent != 16) {
        print_debug("!!! Error sending join request.");
    }
    /**/print_debug("JOIN:REQ ->");
    //print_header(&join_request.header);
    
    pthread_t threads[3];
    int rc = pthread_create(&threads[0], NULL, sendPings, NULL);
    int rc2 = pthread_create(&threads[1], NULL, listenConnections, NULL);
    int rc3 = pthread_create(&threads[2], NULL, recievePacket, NULL);
    
    // Main program loop
    while(STATE != PROGRAM_STOP) {
		
	}
	
    //Send bye packet!
	struct P2P_bye_packet bye = createByePacket();
	//print_header(&bye.header);
	process_to_network(&bye.header);
	int i = 0;
	for (i = 0; i < MAX_PEERS; i++) {
		if (sockets[i] != -1) {
			bytes_sent = send(sockets[i], (void *)&bye, 16, 0);
			debug_message_to("BYE", i);
		}
	}
	if (bytes_sent == 16) print_debug("Program terminated successfully. Bye.");
    return 0;
}

