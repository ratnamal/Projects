/**
 * @author  Ratnamala Korlepara <ratnamal@buffalo.edu>
 * 
 */

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
 
#include<iostream>
#include<fstream>
#include<string.h>
#include<sys/socket.h>
#include<signal.h>
#include<errno.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<sys/select.h>
#include<sys/queue.h>
#include<unistd.h>
#include<list>
#include<string>
#include<map>
#include<sys/time.h>
#include<climits>
#include<time.h>
#include<stdlib.h>

#include "../include/global.h"
#include "../include/network_util.h"
#include "../include/control_header_lib.h"
#include "../include/author.h"
#include "../include/connection_manager.h"
#include "../include/control_handler.h"

using namespace std;

#define BACKLOG 10
#define MAXDATASIZE 100
#define MAXBUFLEN 100
#define AUTHOR_STATEMENT "I, ratnamal, have read and understood the course academic integrity policy."

#ifndef PACKET_USING_STRUCT
    #define CNTRL_CONTROL_CODE_OFFSET 0x04
    #define CNTRL_PAYLOAD_LEN_OFFSET 0x06
#endif

#ifndef PACKET_USING_STRUCT
    #define CNTRL_RESP_CONTROL_CODE_OFFSET 0x04
    #define CNTRL_RESP_RESPONSE_CODE_OFFSET 0x05
    #define CNTRL_RESP_PAYLOAD_LEN_OFFSET 0x06
#endif

fd_set master_list, watch_list;
struct timeval tv,begin;
int head_fd;

struct links 
{
	uint16_t router_id1;
	uint16_t router_id2;
	uint16_t router_port;
	uint16_t nexthop;
	uint16_t cost;
	uint32_t ip_addr;
};

list <links> l_list;
list <links>:: iterator it1;

struct init_payload1
{
	uint16_t router_id;
	uint16_t router_port1;
	uint16_t router_port2;
	uint16_t cost;
	uint32_t ip_addr;
};

struct __attribute__((__packed__)) init_payload
{
	uint16_t no_of_routers;
	uint16_t interval;
	struct init_payload1 ip1[5];
};

struct init_payload *ip;

struct __attribute__((__packed__)) upt_payload
{
	uint16_t router_id;
	uint16_t cost;
};
struct upt_payload *up;

struct __attribute__((__packed__)) rout_table
{
	uint16_t router_id;
	uint16_t padding;
	uint16_t nexthop_id;
	uint16_t cost;
};
struct rout_table rt1[5];

struct rout_upt1
{
	uint32_t router_ip;
	uint16_t router_port;
	uint16_t padding;
	uint16_t router_id;
	uint16_t cost;
};

struct __attribute__((__packed__)) rout_upt
{
	uint16_t no_of_upt;
	uint16_t src_port;
	uint32_t src_router_ip;
	struct rout_upt1 ru1[5];
};

struct rout_upt *ru;

struct rout1
{
	uint32_t router_ip;
	uint16_t router_port;
	uint16_t padding;
	uint16_t router_id;
	uint16_t cost;
};

struct __attribute__((__packed__)) rout
{
	uint16_t no_of_upt;
	uint16_t src_port;
	uint32_t src_router_ip;
	struct rout1 r1[5];
};

struct rout *r;

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

ssize_t recvALL(int sock_index, char *buffer, ssize_t nbytes)
{
    ssize_t bytes = 0;
    bytes = recv(sock_index, buffer, nbytes, 0);

    if(bytes == 0) return -1;
    while(bytes != nbytes)
        bytes += recv(sock_index, buffer+bytes, nbytes-bytes, 0);

    return bytes;
}

ssize_t sendALL(int sock_index, char *buffer, ssize_t nbytes)
{
    ssize_t bytes = 0;
    bytes = send(sock_index, buffer, nbytes, 0);

    if(bytes == 0) return -1;
    while(bytes != nbytes)
        bytes += send(sock_index, buffer+bytes, nbytes-bytes, 0);

    return bytes;
}

char* create_response_header(int sock_index, uint8_t control_code, uint8_t response_code, uint16_t payload_len)
{
    char *buffer;
    #ifdef PACKET_USING_STRUCT
        /** ASSERT(sizeof(struct CONTROL_RESPONSE_HEADER) == 8) 
          * This is not really necessary with the __packed__ directive supplied during declaration (see control_header_lib.h).
          * If this fails, comment #define PACKET_USING_STRUCT in control_header_lib.h
          */
        BUILD_BUG_ON(sizeof(struct CONTROL_RESPONSE_HEADER) != CNTRL_RESP_HEADER_SIZE); // This will FAIL during compilation itself; See comment above.

        struct CONTROL_RESPONSE_HEADER *cntrl_resp_header;
    #endif
    #ifndef PACKET_USING_STRUCT
        char *cntrl_resp_header;
    #endif

    struct sockaddr_in addr;
    socklen_t addr_size;

    buffer = (char *) malloc(sizeof(char)*CNTRL_RESP_HEADER_SIZE);
    #ifdef PACKET_USING_STRUCT
        cntrl_resp_header = (struct CONTROL_RESPONSE_HEADER *) buffer;
    #endif
    #ifndef PACKET_USING_STRUCT
        cntrl_resp_header = buffer;
    #endif

    addr_size = sizeof(struct sockaddr_in);
    getpeername(sock_index, (struct sockaddr *)&addr, &addr_size);

    #ifdef PACKET_USING_STRUCT
        /* Controller IP Address */
        memcpy(&(cntrl_resp_header->controller_ip_addr), &(addr.sin_addr), sizeof(struct in_addr));
        /* Control Code */
        cntrl_resp_header->control_code = control_code;
        /* Response Code */
        cntrl_resp_header->response_code = response_code;
        /* Payload Length */
        cntrl_resp_header->payload_len = htons(payload_len);
    #endif

    #ifndef PACKET_USING_STRUCT
        /* Controller IP Address */
        memcpy(cntrl_resp_header, &(addr.sin_addr), sizeof(struct in_addr));
        /* Control Code */
        memcpy(cntrl_resp_header+CNTRL_RESP_CONTROL_CODE_OFFSET, &control_code, sizeof(control_code));
        /* Response Code */
        memcpy(cntrl_resp_header+CNTRL_RESP_RESPONSE_CODE_OFFSET, &response_code, sizeof(response_code));
        /* Payload Length */
        payload_len = htons(payload_len);
        memcpy(cntrl_resp_header+CNTRL_RESP_PAYLOAD_LEN_OFFSET, &payload_len, sizeof(payload_len));
    #endif

    return buffer;
}

/* Linked List for active control connections */
struct ControlConn
{
    int sockfd;
    LIST_ENTRY(ControlConn) next;
}*connection, *conn_temp;
LIST_HEAD(ControlConnsHead, ControlConn) control_conn_list;

int create_control_sock()
{
    int sock;
    struct sockaddr_in control_addr;
    socklen_t addrlen = sizeof(control_addr);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
        ERROR("socket() failed");

    /* Make socket re-usable */
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (int[]){1}, sizeof(int)) < 0)
        ERROR("setsockopt() failed");

    bzero(&control_addr, sizeof(control_addr));

    control_addr.sin_family = AF_INET;
    control_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    control_addr.sin_port = htons(CONTROL_PORT);

    if(bind(sock, (struct sockaddr *)&control_addr, sizeof(control_addr)) < 0)
        ERROR("bind() failed");

    if(listen(sock, 5) < 0)
        ERROR("listen() failed");

    LIST_INIT(&control_conn_list);

    return sock;
}

int new_control_conn(int sock_index)
{
    int fdaccept;//, caddr_len;
    struct sockaddr_in remote_controller_addr;
	socklen_t caddr_len;
    caddr_len = sizeof(remote_controller_addr);
    fdaccept = accept(sock_index, (struct sockaddr *)&remote_controller_addr, &caddr_len);
    if(fdaccept < 0)
        ERROR("accept() failed");

    /* Insert into list of active control connections */
    connection = (struct ControlConn*)malloc(sizeof(struct ControlConn));
    connection->sockfd = fdaccept;
    LIST_INSERT_HEAD(&control_conn_list, connection, next);

    return fdaccept;
}

void remove_control_conn(int sock_index)
{
    LIST_FOREACH(connection, &control_conn_list, next) {
        if(connection->sockfd == sock_index) LIST_REMOVE(connection, next); // this may be unsafe?
        free(connection);
    }

    close(sock_index);
}

Bool isControl(int sock_index)
{
    LIST_FOREACH(connection, &control_conn_list, next)
        if(connection->sockfd == sock_index) return TRUE;

    return FALSE;
}

void author_response(int sock_index)
{
	uint16_t payload_len, response_len;
	char *cntrl_response_header, *cntrl_response_payload, *cntrl_response;

	payload_len = sizeof(AUTHOR_STATEMENT)-1; // Discount the NULL chararcter
	cntrl_response_payload = (char *) malloc(payload_len);
	memcpy(cntrl_response_payload, AUTHOR_STATEMENT, payload_len);

	cntrl_response_header = create_response_header(sock_index, 0, 0, payload_len);

	response_len = CNTRL_RESP_HEADER_SIZE+payload_len;
	cntrl_response = (char *) malloc(response_len);
	/* Copy Header */
	memcpy(cntrl_response, cntrl_response_header, CNTRL_RESP_HEADER_SIZE);
	free(cntrl_response_header);
	/* Copy Payload */
	memcpy(cntrl_response+CNTRL_RESP_HEADER_SIZE, cntrl_response_payload, payload_len);
	free(cntrl_response_payload);

	sendALL(sock_index, cntrl_response, response_len);

	free(cntrl_response);
}

int neighbour[5],n=0,neighbour_id[5],neighbour_ip[5],src_id,port,interval=2000000000,prev_time=0;
uint32_t src_ip;

/* 
 * Contains the data required to initialize a router. It lists the number of routers in the network, their IP address, router and 
 * data port numbers, and, the initial costs of the links to all routers. It also contains the periodic interval (in seconds) for 
 * the routing updates to be broadcast to all neighbors (routers directly connected).
*/
void init(int sock_index, char *cntrl_payload)
{
	int i;
	struct links l;
	ip = (struct init_payload *) cntrl_payload;
	interval = ntohs(ip->interval);
	
	for(i=0;i<ntohs(ip->no_of_routers);i++)
	{
		if(ntohs(ip->ip1[i].cost) == 0)
		{
			src_id = ntohs(ip->ip1[i].router_id);
			port = (ntohs(ip->ip1[i].router_port1));
			src_ip = ntohl(ip->ip1[i].ip_addr);
		}
		else if (ntohs(ip->ip1[i].cost) > 0 && ntohs(ip->ip1[i].cost) != 65535)
		{
			neighbour[n] = ntohs(ip->ip1[i].router_port1);
			neighbour_id[n] = ntohs(ip->ip1[i].router_id);
			neighbour_ip[n] = ntohl(ip->ip1[i].ip_addr);
			n++;
		}
	}
	
	for(i=0;i<ntohs(ip->no_of_routers);i++)
	{
		l.router_id1 = src_id;
		l.router_id2 = ntohs(ip->ip1[i].router_id);
		l.router_port = ntohs(ip->ip1[i].router_port1);
		l.cost = ntohs(ip->ip1[i].cost);
		if(l.cost == 65535)
		{
			l.nexthop = 65535;
		}
		else if (l.cost == 0)
			l.nexthop = src_id;
		else
			l.nexthop = ntohs(ip->ip1[i].router_id);
		l.ip_addr = ntohl(ip->ip1[i].ip_addr);
		l_list.push_back(l);
	}
	
	uint16_t payload_len, response_len;
	char *cntrl_response_header, *cntrl_response_payload, *cntrl_response;

	payload_len = 0;
	
	cntrl_response_header = create_response_header(sock_index, 1, 0, payload_len);

	response_len = CNTRL_RESP_HEADER_SIZE;
	cntrl_response = (char *) malloc(response_len);
	/* Copy Header */
	memcpy(cntrl_response, cntrl_response_header, CNTRL_RESP_HEADER_SIZE);
	free(cntrl_response_header);

	sendALL(sock_index, cntrl_response, response_len);

	free(cntrl_response);
		
    struct sockaddr_in control_addr;
    socklen_t addrlen = sizeof(control_addr);

    router_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(router_socket < 0)
        ERROR("socket() failed");

    /* Make socket re-usable */
    if(setsockopt(router_socket, SOL_SOCKET, SO_REUSEADDR, (int[]){1}, sizeof(int)) < 0)
        ERROR("setsockopt() failed");

	bzero(&control_addr, sizeof(control_addr));

    control_addr.sin_family = AF_INET;
    control_addr.sin_addr.s_addr = htonl(src_ip);
    control_addr.sin_port = htons(port);

    if(bind(router_socket, (struct sockaddr *)&control_addr, sizeof(control_addr)) < 0)
        ERROR("bind() failed");
	
	cout<<"router sckt: "<<router_socket<<endl;
	FD_SET(router_socket,&master_list);
	if(router_socket > head_fd) head_fd = router_socket;
	cout<<"end of init"<<endl;
}

/* 
 * The controller uses this to request the current routing/forwarding table from a given router. The table sent as a response 
 * should contain an entry for each router in the network (including self) consisting of the next hop router ID (on the least cost 
 * path to that router) and the cost of the path to it.
 */
void rtable(int sock_index)
{
	//send the response

	int k=0;
	unsigned char *resp;
	resp = (unsigned char*)malloc(sizeof(rout_table) * 5 * sizeof(char));
	memset(resp,'0', sizeof(rout_table) * sizeof(char) * 5);
	
	for(it1=l_list.begin();it1!=l_list.end();it1++)
	{
		rt1[k].router_id = htons(it1->router_id2);
		rt1[k].padding = 0;
		rt1[k].padding = htons(rt1[k].padding);
		rt1[k].nexthop_id = htons(it1->nexthop);
		rt1[k].cost = htons(it1->cost);
		k++;
	}
	
	memcpy(resp, (const unsigned char*)rt1,sizeof(rout_table)*5);
	
	for(int i=0;i<sizeof(rout_table)*5;i++)
		printf("%02X ",resp[i]);
	 cout<<endl;
	
	uint16_t payload_len, response_len;
	char *cntrl_response_header, *cntrl_response_payload, *cntrl_response;
	
	payload_len = sizeof(rout_table)*5; 
	cntrl_response_payload = (char *) malloc(payload_len);
	memcpy(cntrl_response_payload, resp, payload_len);

	for(int i=0;i<sizeof(rout_table)*5;i++)
		printf("%02X ",cntrl_response_payload[i]);
	cout<<endl;
	cntrl_response_header = create_response_header(sock_index, 2, 0, payload_len);

	response_len = CNTRL_RESP_HEADER_SIZE+payload_len;
	cntrl_response = (char *) malloc(response_len);
	/* Copy Header */
	memcpy(cntrl_response, cntrl_response_header, CNTRL_RESP_HEADER_SIZE);
	free(cntrl_response_header);
	/* Copy Payload */
	memcpy(cntrl_response+CNTRL_RESP_HEADER_SIZE, cntrl_response_payload, payload_len);
	free(cntrl_response_payload);

	sendALL(sock_index, cntrl_response, response_len);

	free(cntrl_response);
}

/*
 * The controller uses this to change/update the link cost between between the router receiving this message and a neighboring router.
*/
void update(int sock_index,char *cntrl_payload)
{
	int i;
	up = (struct upt_payload *) cntrl_payload;
	for(it1=l_list.begin();it1!=l_list.end();it1++)
	{
		if((it1->router_id2 == ntohs(up->router_id)) && (it1->router_id1 == src_id))
		{
			it1->cost = ntohs(up->cost);
			break;
		}
	}
	
	uint16_t payload_len, response_len;
	char *cntrl_response_header, *cntrl_response_payload, *cntrl_response;

	payload_len = 0;
	
	cntrl_response_header = create_response_header(sock_index, 3, 0, payload_len);

	response_len = CNTRL_RESP_HEADER_SIZE;
	cntrl_response = (char *) malloc(response_len);
	/* Copy Header */
	memcpy(cntrl_response, cntrl_response_header, CNTRL_RESP_HEADER_SIZE);
	free(cntrl_response_header);

	sendALL(sock_index, cntrl_response, response_len);

	free(cntrl_response);
	
}

/*
 * The controller uses this to simulate a crash (unexpected failure) on a router. On receiving this message, the router 
 * should exit immediately.
*/
void crash(int sock_index)
{
	uint16_t payload_len, response_len;
	char *cntrl_response_header, *cntrl_response_payload, *cntrl_response;

	payload_len = 0;
	
	cntrl_response_header = create_response_header(sock_index, 4, 0, payload_len);

	response_len = CNTRL_RESP_HEADER_SIZE;
	cntrl_response = (char *) malloc(response_len);
	/* Copy Header */
	memcpy(cntrl_response, cntrl_response_header, CNTRL_RESP_HEADER_SIZE);
	free(cntrl_response_header);

	sendALL(sock_index, cntrl_response, response_len);

	free(cntrl_response);
	
	exit(0);
}

Bool control_recv_hook(int sock_index)
{
    char *cntrl_header, *cntrl_payload;
    uint8_t control_code;
    uint16_t payload_len;

    /* Get control header */
    cntrl_header = (char *) malloc(sizeof(char)*CNTRL_HEADER_SIZE);
    bzero(cntrl_header, CNTRL_HEADER_SIZE);

    if(recvALL(sock_index, cntrl_header, CNTRL_HEADER_SIZE) < 0){
        remove_control_conn(sock_index);
        free(cntrl_header);
        return FALSE;
    }

    /* Get control code and payload length from the header */
    #ifdef PACKET_USING_STRUCT
        /** ASSERT(sizeof(struct CONTROL_HEADER) == 8) 
          * This is not really necessary with the __packed__ directive supplied during declaration (see control_header_lib.h).
          * If this fails, comment #define PACKET_USING_STRUCT in control_header_lib.h
          */
        BUILD_BUG_ON(sizeof(struct CONTROL_HEADER) != CNTRL_HEADER_SIZE); // This will FAIL during compilation itself; See comment above.

        struct CONTROL_HEADER *header = (struct CONTROL_HEADER *) cntrl_header;
        control_code = header->control_code;
        payload_len = ntohs(header->payload_len);
    #endif
    #ifndef PACKET_USING_STRUCT
        memcpy(&control_code, cntrl_header+CNTRL_CONTROL_CODE_OFFSET, sizeof(control_code));
        memcpy(&payload_len, cntrl_header+CNTRL_PAYLOAD_LEN_OFFSET, sizeof(payload_len));
        payload_len = ntohs(payload_len);
    #endif

    free(cntrl_header);

    /* Get control payload */
    if(payload_len != 0){
        cntrl_payload = (char *) malloc(sizeof(char)*payload_len);
        bzero(cntrl_payload, payload_len);

        if(recvALL(sock_index, cntrl_payload, payload_len) < 0){
            remove_control_conn(sock_index);
            free(cntrl_payload);
            return FALSE;
        }
    }

    /* Triage on control_code */
    switch(control_code){
        case 0: author_response(sock_index);
                break;
		
		case 1: init(sock_index,cntrl_payload);
				gettimeofday(&begin,NULL);
				prev_time = begin.tv_sec;
				break;
				
		case 2: rtable(sock_index);
				break;
				
		case 3: update(sock_index,cntrl_payload);
				break;
				
		case 4: crash(sock_index);
				break;
    }

    if(payload_len != 0) free(cntrl_payload);
    return TRUE;
}

void main_loop()
{
    int selret, sock_index, fdaccept,numbytes,src;
	int resend_ip,resend_port;
	struct sockaddr their_addr;
	char buf[sizeof(rout_upt)] = {0};
	socklen_t their_addr_len;
	
	
    while(TRUE){
        watch_list = master_list;
		
	gettimeofday(&begin,NULL);
	tv.tv_sec = (interval + prev_time) - begin.tv_sec;
	tv.tv_usec = 0;
			
        selret = select(head_fd+1, &watch_list, NULL, NULL, &tv);
		
        if(selret < 0)
            ERROR("select failed.");
		
	if(selret == 0)
	{
		//then send routing update to neighbours
		int k=0;
		unsigned char *routup;
		r = (struct rout*)malloc(sizeof(struct rout));
					
		memset(r,'\0',sizeof(rout));
		routup = (unsigned char*)malloc(sizeof(rout) * sizeof(char));
		memset(routup, '0', sizeof(rout) * sizeof(char));
					
		r->no_of_upt = l_list.size();					
		r->src_port = port;
		r->src_router_ip = src_ip;
				
		for(it1=l_list.begin();it1!=l_list.end();it1++)
		{
			r->r1[k].router_ip = it1->ip_addr;
			r->r1[k].router_port = it1->router_port;
			r->r1[k].padding = 0;
			r->r1[k].router_id = it1->router_id2;
			r->r1[k].cost = it1->cost;
			k++;
		}
			
		memcpy(routup,(const unsigned char*)r,sizeof(rout));
			
		struct sockaddr_in control_addr;
		socklen_t addrlen = sizeof(control_addr);
					
		for(int i=0;i<n;i++)
		{					
			control_addr.sin_family = AF_INET;
			control_addr.sin_addr.s_addr = htonl(neighbour_ip[i]);
			control_addr.sin_port = htons(neighbour[i]);
					
			int snd = sendto(router_socket, routup, sizeof(rout), 0,(struct sockaddr *)&control_addr, sizeof(control_addr));
		}		
		free(r);
		free(routup);
		r = NULL;
		routup = NULL;
		gettimeofday(&begin,NULL);
		prev_time = begin.tv_sec;
	}
		
        /* Loop through file descriptors to check which ones are ready */
        for(sock_index=0; sock_index<=head_fd; sock_index+=1){
			
            if(FD_ISSET(sock_index, &watch_list)){
                /* control_socket */
                if(sock_index == control_socket){
					
                    fdaccept = new_control_conn(sock_index);

                    /* Add to watched socket list */
                    FD_SET(fdaccept, &master_list);
                    if(fdaccept > head_fd) head_fd = fdaccept;
                }

                /* router_socket */
                else if(sock_index == router_socket){
				
			their_addr_len = sizeof(their_addr);
                    //call handler that will call recvfrom() .....
			if((numbytes = recvfrom(sock_index,buf,sizeof(rout_upt),0,&their_addr,&their_addr_len)) == -1)
				ERROR("recvfrom");
					
			ru = (struct rout_upt *) buf;
	
			for(it1=l_list.begin();it1!=l_list.end();it1++)
				if(ru->src_port == it1->router_port)
					src = it1->router_id2;
												
			//distance vector calc and update forwarding table
			int sum,initial_cost, cost;
					
			for(it1=l_list.begin();it1!=l_list.end();it1++)
			{
				if(ru->src_port == it1->router_port)
				{
					initial_cost = it1->cost;
					break;
				}
			}
					
			for(it1=l_list.begin();it1!=l_list.end();it1++)
			{
				if(it1->router_id1 == it1->router_id2)
					continue;
				if(it1->router_port == ru->src_port)
				{
					continue;
				}
					
				for(int i=0;i<ru->no_of_upt;i++)
				{
					if(it1->router_port == ru->ru1[i].router_port)
					{
						cost = ru->ru1[i].cost;
						break;
					}
				}
				if((cost + initial_cost) < it1->cost)
				{
					it1->cost = cost + initial_cost;
					it1->nexthop = src;
				}
			}
		}

                /* data_socket */
                else if(sock_index == data_socket){
					
                    //new_data_conn(sock_index);
					fdaccept = new_control_conn(sock_index);
					
					FD_SET(fdaccept, &master_list);
                    if(fdaccept > head_fd) head_fd = fdaccept;
                }

                /* Existing connection */
                else{
                    if(isControl(sock_index)){
                        if(!control_recv_hook(sock_index)) FD_CLR(sock_index, &master_list);
                    }
                    //else if isData(sock_index);
                    else ERROR("Unknown socket index");
                }
            }
        }
    }
}

void init()
{
    control_socket = create_control_sock();

    //router_socket and data_socket will be initialized after INIT from controller

    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);

    /* Register the control socket */
    FD_SET(control_socket, &master_list);
    head_fd = control_socket;

    main_loop();
}
 
int main(int argc, char **argv)
{
	/*Start Here*/
	
	//sscanf(argv[1], "%" SCNu16, &CONTROL_PORT);
	CONTROL_PORT = atoi(argv[1]);
    init();

	return 0;
}
