/**
 * @ratnamal_assignment1
 * @author  Ratnamala Korlepara <ratnamal@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <iostream>
#include <stdio.h>

#include "../include/global.h"
#include "../include/logger.h"

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<signal.h>
#include<string.h>
#include<errno.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<map>
#include<string>
#include<list>

using namespace std;

#define BACKLOG 10
#define MAXDATASIZE 100

char IPAddress[200];
char Hostname[200];

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

in_port_t get_in_port(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return (((struct sockaddr_in*)sa)->sin_port);
    }

    return (((struct sockaddr_in6*)sa)->sin6_port);
}

int validateIP(string str)
{
	struct sockaddr_in sa;
	int ret = inet_pton(AF_INET,str.c_str(),&(sa.sin_addr));
	return ret;
}

int validatePort(char* port)
{
	int len = strlen(port);
	int count=0;
	
	for(int i=0;i<len;i++)
	{
		if(port[i] >= '0' && port[i]<= '9')
			count++;
		else if ((port[i] >= 'a' && port[i]<= 'z') || (port[i] >= 'A' && port[i]<= 'Z'))
			return -1;
	}
	
	if(count == len)
	{
		int p =atoi(port);
		if(p>1023 && p<65535)
			return 1;
		else return -1;
	}
}

void ipaddress()
{

	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int res,r;
	struct sockaddr_in sa;
	socklen_t sa_size = sizeof(sa);
	char s[INET6_ADDRSTRLEN];
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; 
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; 

	char extIP[] = "8.8.8.8";
	char extPort[] = "53";
	
	
	getaddrinfo(extIP,extPort,&hints,&servinfo);
		
	for (p=servinfo;p!=NULL;p=p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
		{
			continue;
		}

		if ((connect(sockfd,p->ai_addr,p->ai_addrlen)) == -1)
		{
			close(sockfd);
			continue;
		}
		break;
	}

	if (p == NULL) 
		exit(2);
	
	getsockname(sockfd, (struct sockaddr*)&sa, &sa_size);
		
	memset(s,'\0',INET6_ADDRSTRLEN);
	memset(IPAddress,'\0',200);
	inet_ntop(sa.sin_family, get_in_addr((struct sockaddr *)&sa),s,INET6_ADDRSTRLEN);
	
	strcpy(IPAddress,s);
	close(sockfd);
}

struct sending
{
	int b;
	char cip_addr[200];
	char ip_addr[200];
	char msg[256];
};

struct send_broad
{
	char cip_addr[200];
	char msg[256];
};

struct block_unblock
{
	char client1_addr[200];
	char client2_addr[200];
};

struct list_map
{
	char hostname[200];
	char ip_addr[200];
	int port_no;
};

map<int,list_map> listmap;
map<int,list_map>::iterator lit;

struct login_map
{
	char hostname[200];
	char ip_addr[200];
	int port_no;
};

map<int,login_map> logmap;
map<int,login_map>::iterator itlog;

struct client_map
{
	char hostname[200];
	char ip_addr[200];
	int port_no;
	int socketid;
	char status[20];
	int msg_sent;
	int msg_recv;
	char msg_buf[20][256];
};

map<int,client_map> smap;
map<int,client_map>::iterator it;

list<string> values_list;
list<string>::iterator itv;

list<string> block_list;
list<string>::iterator itb;

map<string,list<string> > lmap;
map<string,list<string> >::iterator itl;

struct login
{
	char ip_addr[200];
	char port[10];
};


void client(char *port)
{
	int client_sockfd,addr_status,bytes_recvd,yes=1,fdmax,connection=0;
	char buf[sizeof(send_broad)],host[200],buffer[20];
	fd_set readfds,master;
	string command_str;
	
	struct sending sd;
	struct addrinfo hints,*servinfo,*p;
	
	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;

	FD_ZERO(&master);
	FD_ZERO(&readfds);
	
	ipaddress();
	
	getaddrinfo(IPAddress,port,&hints,&servinfo);

	for (p=servinfo;p!=NULL;p=p->ai_next)
	{
		if((client_sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
			continue;
		
		if((bind(client_sockfd,p->ai_addr,p->ai_addrlen)) == -1)
			continue;
		
		if((connect(client_sockfd,p->ai_addr,p->ai_addrlen)) == -1)
		{
			close(client_sockfd);
			continue;
		}
		break;
	}

	freeaddrinfo(servinfo);
	int client_port = atoi(port);
	
	FD_SET(0,&master);
	FD_SET(client_sockfd,&master);
	fdmax = client_sockfd;
	
	while(1)
	{		
		readfds = master;
  		select(fdmax+1,&readfds,NULL,NULL,NULL);
			
		for(int i=0;i<=fdmax;i++)
		{
			if (FD_ISSET(i,&readfds))
			{
				if(i==0)
				{
					getline (cin,command_str);
					if(command_str.compare("AUTHOR") == 0)
					{
						cse4589_print_and_log("[%s:SUCCESS]\n", command_str.c_str());
						cse4589_print_and_log("I, ratnamal, have read and understood the course academic integrity policy.\n");
						cse4589_print_and_log("[%s:END]\n", command_str.c_str());
					}

					if(command_str.compare("IP") == 0)
					{
						string word;
						ipaddress();
						for(int i=0;i<sizeof(IPAddress);i++)
						{
							 word += IPAddress[i];
						}
						
						int ret = validateIP(word);
						if(ret == 0)
						{
							cse4589_print_and_log("[%s:ERROR]\n", command_str.c_str());
							cse4589_print_and_log("[%s:END]\n", command_str.c_str());
						}
						else
						{
							cse4589_print_and_log("[%s:SUCCESS]\n", command_str.c_str());
							cse4589_print_and_log("IP:%s\n", IPAddress);
							cse4589_print_and_log("[%s:END]\n", command_str.c_str());
						}
					}
		
					if(command_str.compare("PORT") == 0)
					{
						cse4589_print_and_log("[%s:SUCCESS]\n", command_str.c_str());
						cse4589_print_and_log("PORT:%d\n", client_port);
						cse4589_print_and_log("[%s:END]\n", command_str.c_str());
					}
					
					if((command_str.substr(0,5)).compare("LOGIN") == 0)
					{
						
						
						struct login l; 
									
						int j=0,spaces=0,k=0,conn;
						string word[3];
						
						for(int i=0;i<command_str.length();i++)
						{
							if (command_str[i] == ' ')
							{
								spaces++;
								if (spaces <=2) { j++;
									continue;}
								else if(spaces >2) { j++;
									goto label3; }	
							}
							label3:
							word[j] += command_str[i];		
						}	
		
						strcpy(l.ip_addr,word[1].data());
						strcpy(l.port,word[2].data());
						
						int ret = validateIP(word[1]);
						int ret1 = validatePort(l.port);
						
						if(ret == 0 || ret1 == -1)
						{
							
							cse4589_print_and_log("[%s:ERROR]\n", command_str.substr(0,5).c_str());
							cse4589_print_and_log("[%s:END]\n", command_str.substr(0,5).c_str());
							
						}
						else
						{
							cse4589_print_and_log("[%s:SUCCESS]\n", command_str.substr(0,5).c_str());
							getaddrinfo(l.ip_addr,l.port,&hints,&servinfo);
							struct login_map log_m; 
							for (p=servinfo;p!=NULL;p=p->ai_next)
							{
								if((client_sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1) 
									continue; 
								
								conn = connect(client_sockfd,p->ai_addr,p->ai_addrlen);
								if(conn == -1)
								{
									connection=0;
									close(client_sockfd);
									continue;
								}
								else if(conn == 0)
								{
									FD_SET(client_sockfd,&master);
									fdmax = client_sockfd;
									send(client_sockfd,port,sizeof(port),0);
									connection=1;
									continue;
								}
								break;
							}
							cse4589_print_and_log("[%s:END]\n", command_str.substr(0,5).c_str());
						}
					}
					
					if(command_str.compare("EXIT") == 0)
					{
						cse4589_print_and_log("[%s:SUCCESS]\n", command_str.c_str());
						send(client_sockfd,"EXIT",20,0);
						ipaddress();
						send(client_sockfd,IPAddress,sizeof(IPAddress),0);
						close(client_sockfd);
						cse4589_print_and_log("[%s:END]\n", command_str.c_str());
						exit(0);
						
					}	
					
					if(connection == 1)
					{
						if(command_str.compare("LIST") == 0)	
						{
							int list_id=0,j=0;
							char car[3000],p[10];
							string word[12];
							struct list_map lm;
							
							send(client_sockfd,"LIST",20,0);
						
							bytes_recvd = recv(client_sockfd,car,3000,0);
							if (bytes_recvd <= 0)
							{
								cout<<bytes_recvd<<endl;
								cse4589_print_and_log("[%s:ERROR]\n", command_str.c_str());
								cse4589_print_and_log("[%s:END]\n", command_str.c_str());
							
							}
							else
							{
								cout<<"Else loop for list"<<endl;
								for(int i=0;i<sizeof(car);i++)
								{
									if(car[i] == ' ')
									{
										i++;
										j++;
									}
									else if (car[i] == '-' && car[i+1] == '1' && car[i+2] != '\0')
									{
										i = i+2;
										j++;
										goto label6;
									}
									else if (car[i] == '-' && car[i+1] == '1' && car[i+2] == '\0')
										break;
									
									label6:	
									if(car[i] != ' ')
									{
										word[j] += car[i];
									}
								}
								
								for(int i=0;i<j;i+=3)
								{
									memset(lm.hostname,'\0',200);
									memset(lm.ip_addr,'\0',200);
									strcpy(lm.hostname,word[i].data());
									strcpy(lm.ip_addr,word[i+1].data());
									strcpy(p,word[i+2].data());
									int port_2 = atoi(p);
									lm.port_no = port_2;
									
									listmap[port_2] = lm;
								}
								cse4589_print_and_log("[%s:SUCCESS]\n", command_str.c_str());
								for(lit=listmap.begin();lit!=listmap.end();lit++)
								{
									cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", ++list_id, lit->second.hostname, lit->second.ip_addr, lit->second.port_no);
								}
								cse4589_print_and_log("[%s:END]\n", command_str.c_str());
							}
							
						}
					
						if(command_str.compare("REFRESH") == 0)
						{
							int list_id=0,j=0;
							char car[3000],p[10];
							string word[12];
							struct list_map lm;
							
							send(client_sockfd,"REFRESH",20,0);
						
							bytes_recvd = recv(client_sockfd,car,3000,0);
							
							if (bytes_recvd <= 0)
							{
								cse4589_print_and_log("[%s:ERROR]\n", command_str.c_str());
								cse4589_print_and_log("[%s:END]\n", command_str.c_str());
							}
							else
							{
								for(int i=0;i<sizeof(car);i++)
								{
									if(car[i] == ' ')
									{
										i++;
										j++;
									}
									else if (car[i] == '-' && car[i+1] == '1' && car[i+2] != '\0')
									{
										i = i+2;
										j++;
										goto label7;
									}
									else if (car[i] == '-' && car[i+1] == '1' && car[i+2] == '\0')
										break;
									
									label7:	
									if(car[i] != ' ')
									{
										word[j] += car[i];
									}
								}
								
								for(int i=0;i<j;i+=3)
								{
									memset(lm.hostname,'\0',200);
									memset(lm.ip_addr,'\0',200);
									strcpy(lm.hostname,word[i].data());
									strcpy(lm.ip_addr,word[i+1].data());
									strcpy(p,word[i+2].data());
									int port_2 = atoi(p);
									lm.port_no = port_2;
									
									listmap[port_2] = lm;
								}
								cse4589_print_and_log("[%s:SUCCESS]\n", command_str.c_str());
								cse4589_print_and_log("[%s:END]\n", command_str.c_str());
							}
							
						}
		
						if((command_str.substr(0,4)).compare("SEND") == 0)
						{
														
							sd.b=0;
							int j=0,spaces=0,k=0;
							string word[50];
							char stsend[sizeof(sending)],car[256],c[256];
							char *car1 = car;
						
							for(int i=0;i<command_str.length();i++)
							{
								if (command_str[i] == ' ')
								{
									spaces++;
									if (spaces <=2) { j++;
										continue;}
									else if(spaces >2) { j++;
										goto label1; }	
								}
								label1:
								word[j] += command_str[i];		
							}		
							ipaddress();
							strcpy(sd.cip_addr,IPAddress);
							int ret = validateIP(word[1]);
							strcpy(sd.ip_addr,word[1].data());
							
							for(int i=2;i<=j;i++)
							{
								memset(c,'\0',256);
								strcpy(c,word[i].data());
								car1 += snprintf(car1,sizeof(c),"%s",c);
							}
							
							strncpy(sd.msg,car,sizeof(car));
															
							if(ret > 0)
							{
							
								memset(stsend,'\0', sizeof(sending));
								memcpy(stsend,&sd,sizeof(sending));
								
								send(client_sockfd,"RELAY",20,0);
												
								if ((send(client_sockfd,stsend,sizeof(sending),0)) == -1)
								{
									cse4589_print_and_log("[%s:ERROR]\n", command_str.substr(0,4).c_str());
									cse4589_print_and_log("[%s:END]\n",command_str.substr(0,4).c_str() );
									continue;
								}
								cse4589_print_and_log("[%s:SUCCESS]\n", command_str.substr(0,4).c_str());
								cse4589_print_and_log("[%s:END]\n", command_str.substr(0,4).c_str());
							}
							else
							{
								cse4589_print_and_log("[%s:ERROR]\n", command_str.substr(0,4).c_str());
								cse4589_print_and_log("[%s:END]\n",command_str.substr(0,4).c_str() );
							}
						}

						if((command_str.substr(0,9)).compare("BROADCAST") == 0)
						{
							send(client_sockfd,"RELAY",20,0);
								
							sd.b = 1;
							string word[2];
							int j=0,spaces=0,k=0;
							char stsend[sizeof(sending)];
			
							for(int i=0;i<command_str.length();i++)
							{
								if (command_str[i] == ' ')
								{
									spaces++;
									if(spaces == 1) {
									cout<<"Word: "<<word[j];
									j++;
									continue;
									}
									else{
									cout<<"word: "<<word[j];
									j++;
									goto label;
									}
								}
								label:
								word[j] += command_str[i];
							} 
							ipaddress();
							strcpy(sd.cip_addr,IPAddress);
							memset(sd.ip_addr,'\0',200);
							strcpy(sd.msg,word[1].data());			
							
							memset(stsend,'\0', sizeof(sending));			
							memcpy(stsend,&sd,sizeof(sending));				
				
							if ((send(client_sockfd,stsend,sizeof(sending),0)) == -1)
							{
								cse4589_print_and_log("[%s:ERROR]\n", command_str.substr(0,9).c_str());
								cse4589_print_and_log("[%s:END]\n",command_str.substr(0,9).c_str() );
								continue;
							}
							
							cse4589_print_and_log("[%s:SUCCESS]\n", command_str.substr(0,9).c_str());
							cse4589_print_and_log("[%s:END]\n", command_str.substr(0,9).c_str());
						}
					
						if((command_str.substr(0,5)).compare("BLOCK") == 0)
						{													
							string word[2];
							int j=0,spaces=0,blocked=0;
							bool found=false;
							struct block_unblock bu;
							char block_ip[sizeof(block_unblock)];

							for(int i=0;i<command_str.length();i++)
							{
								if (command_str[i] == ' ')
								{
									spaces++;
									if(spaces == 1) {
										j++;
										continue;
									}
									else{
										j++;
										goto label4;
									}
								}
								label4:
								word[j] += command_str[i];
							} 
			
							strcpy(bu.client2_addr,word[1].data());
							ipaddress();
							strcpy(bu.client1_addr,IPAddress);
							
							if(strcmp(bu.client2_addr," ") ==0)
							{
								cse4589_print_and_log("[%s:ERROR]\n", command_str.substr(0,5).c_str());
								cse4589_print_and_log("[%s:END]\n",command_str.substr(0,5).c_str() );
							}
							
							int ret = validateIP(word[1]);
							
							if(ret)
							{								
								for(itb=block_list.begin();itb!=block_list.end();itb++)
								{
									if(strcmp(bu.client2_addr,itb->c_str()) == 0)
									{
										blocked++;
										break;
									}
								}
							}

							if(ret == 0 || blocked > 0)
							{
								cout<<"ret: "<<ret<<endl;
								cse4589_print_and_log("[%s:ERROR]\n", command_str.substr(0,5).c_str());
								cse4589_print_and_log("[%s:END]\n",command_str.substr(0,5).c_str() );
								continue;
							}
							else
							{								
								if(block_list.empty())
								{
									found = false;
									block_list.push_back(word[1]);
								}
								else
								{
									for(itb=block_list.begin();itb!=block_list.end();itb++)
									{
										if (strcmp(itb->c_str(),bu.client2_addr) == 0) {
											
											found = true;
											break; 
										} else {
											
											found = false;
											block_list.push_back(word[1]);
											break;
										}
									}
								}
									
								if(!found)
								{
									send(client_sockfd,"BLOCK",20,0);
									
									memset(block_ip,'\0', sizeof(block_unblock));			
									memcpy(block_ip,&bu,sizeof(block_unblock));	
										
									if (((send(client_sockfd,block_ip,sizeof(block_unblock),0)) == -1))
									{
										cse4589_print_and_log("[%s:ERROR]\n", command_str.substr(0,5).c_str());
										cse4589_print_and_log("[%s:END]\n",command_str.substr(0,5).c_str() );
										continue;
									}
									cse4589_print_and_log("[%s:SUCCESS]\n", command_str.substr(0,5).c_str());		
									cse4589_print_and_log("[%s:END]\n", command_str.substr(0,5).c_str());
								}
								else
								{
									cse4589_print_and_log("[%s:ERROR]\n", command_str.substr(0,5).c_str());
									cse4589_print_and_log("[%s:END]\n",command_str.substr(0,5).c_str() );
									
								}
							}
						}
						if((command_str.substr(0,7)).compare("UNBLOCK") == 0)
						{
							send(client_sockfd,"UNBLOCK",20,0);
							
							string word[2];
							int j=0,spaces=0,blocked=0;
							struct block_unblock bu;
							char unblock_ip[sizeof(block_unblock)];
							
							for(int i=0;i<command_str.length();i++)
							{
								if (command_str[i] == ' ')
								{
									spaces++;
									if(spaces == 1) {
										j++;
										continue;
									}
									else{
										j++;
										goto label5;
									}
								}
								label5:
								word[j] += command_str[i];
							} 
			
							strcpy(bu.client2_addr,word[1].data());
							ipaddress();
							strcpy(bu.client1_addr,IPAddress);
							
							if(strcmp(bu.client2_addr," ") ==0)
							{
								cse4589_print_and_log("[%s:ERROR]\n", command_str.substr(0,7).c_str());
								cse4589_print_and_log("[%s:END]\n",command_str.substr(0,7).c_str() );
							}
							
							printf("ip to unblock : %s\n",bu.client2_addr);
							
							int ret = validateIP(word[1]);
							for(itb=block_list.begin();itb!=block_list.end();itb++)
							{
								if(strcmp(bu.client2_addr,itb->c_str()) == 0)
								{
									blocked++;
								}
								
							}	
															
							if((ret == 0) || (blocked == 0))
							{
								cse4589_print_and_log("[%s:ERROR]\n", command_str.substr(0,7).c_str());
								cse4589_print_and_log("[%s:END]\n",command_str.substr(0,7).c_str() );
							}
							else
							{
								memset(unblock_ip,'\0', sizeof(block_unblock));			
								memcpy(unblock_ip,&bu,sizeof(block_unblock));	
									
								if (((send(client_sockfd,unblock_ip,sizeof(block_unblock),0)) == -1))
								{
									cse4589_print_and_log("[%s:ERROR]\n", command_str.substr(0,7).c_str());
									cse4589_print_and_log("[%s:END]\n",command_str.substr(0,7).c_str() );
									continue;
										
								}
								cse4589_print_and_log("[%s:SUCCESS]\n", command_str.substr(0,7).c_str());
								cse4589_print_and_log("[%s:END]\n", command_str.substr(0,7).c_str());
							}
						}
					
						if(command_str.compare("LOGOUT") == 0)
						{
							cse4589_print_and_log("[%s:SUCCESS]\n", command_str.c_str());
							int retval = send(client_sockfd,"LOUT",20,0);
							cout<<retval<<endl;
							ipaddress();
							cout<<IPAddress<<endl;
							send(client_sockfd,IPAddress,sizeof(IPAddress),0);
							close(client_sockfd);
							FD_CLR(client_sockfd,&master);
							cse4589_print_and_log("[%s:END]\n", command_str.c_str());
						}					
					}
				}
				else if(i == client_sockfd)
				{
					struct send_broad sb;
						memset(buf,'\0',sizeof(send_broad));
						bytes_recvd = recv(client_sockfd,buf,sizeof(send_broad),0);
						if(bytes_recvd <= 0)
						{
							cse4589_print_and_log("[%s:ERROR]\n", "RECEIVED");
							close(client_sockfd);
						}
						
						else
						{
							memcpy(&sb,buf,sizeof(send_broad));
							
							cse4589_print_and_log("[%s:SUCCESS]\n", "RECEIVED");
							cse4589_print_and_log("msg from:%s\n[msg]:%s\n", sb.cip_addr, sb.msg);
							cse4589_print_and_log("[%s:END]\n","RECEIVED" );
						}	
					//}						
				}
			}
		}
	}
}

void server(char *port)
{
	int server_sockfd,new_sockfd,i,number_of_clients=0;
	int addr_status,bytes_recvd,bytes_got,fdmax,bytes_sent;
	fd_set readfds,master;
	char buf[sizeof(sending)],block_buf[sizeof(block_unblock)],s[INET_ADDRSTRLEN],ebuf[200],buffer[20],host[200],service[10];
	string command_str; 
	
	struct addrinfo hints,*servinfo,*p;
	struct sockaddr_storage their_addr;
	struct client_map value;
	socklen_t their_addr_size;	
	
	FD_ZERO(&master);
	FD_ZERO(&readfds);

	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	getaddrinfo(NULL,port,&hints,&servinfo);
		
	for (p=servinfo;p!=NULL;p=p->ai_next)
	{
		if((server_sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
			continue;
	
		if((bind(server_sockfd,p->ai_addr,p->ai_addrlen)) == -1)
		{
			close(server_sockfd);
			continue;
		}
		break;
	}
	freeaddrinfo(servinfo);	
	
	listen(server_sockfd,BACKLOG);
		
	int server_port = atoi(port);
	FD_SET(0,&master);
	FD_SET(server_sockfd,&master);
	fdmax = server_sockfd;

	while(1)
	{
		readfds = master;
  		select(fdmax+1,&readfds,NULL,NULL,NULL);

		for(i=0;i<=fdmax;i++)
		{
			if (FD_ISSET(i,&readfds))
			{
				if(i==0)
				{
					getline(cin,command_str);
					if(command_str.compare("AUTHOR") == 0)
					{
						cse4589_print_and_log("[%s:SUCCESS]\n", command_str.c_str());
						cse4589_print_and_log("I, ratnamal, have read and understood the course academic integrity policy.\n");
						cse4589_print_and_log("[%s:END]\n", command_str.c_str());
					}

					if(command_str.compare("IP") == 0)
					{
						string word;
						ipaddress();
						for(int i=0;i<sizeof(IPAddress);i++)
						{
							 word += IPAddress[i];
						}
						
						int ret = validateIP(word);
						if(ret == 0)
						{
							cse4589_print_and_log("[%s:ERROR]\n", command_str.c_str());
							cse4589_print_and_log("[%s:END]\n", command_str.c_str());
						}
						else
						{
							cse4589_print_and_log("[%s:SUCCESS]\n", command_str.c_str());
							cse4589_print_and_log("IP:%s\n", IPAddress);
							cse4589_print_and_log("[%s:END]\n", command_str.c_str());
						}
					}
		
					if(command_str.compare("PORT") == 0)
					{
						cse4589_print_and_log("[%s:SUCCESS]\n", command_str.c_str());
						cse4589_print_and_log("PORT:%d\n", server_port);
						cse4589_print_and_log("[%s:END]\n", command_str.c_str());
					}
		
					if(command_str.compare("LIST") == 0)	
					{
						int list_id=0;
						cse4589_print_and_log("[%s:SUCCESS]\n", command_str.c_str());
						
						for(it=smap.begin();it!=smap.end();it++)
						{
							if(strcmp(it->second.status,"logged-in") == 0)
								cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", ++list_id, it->second.hostname, it->second.ip_addr, it->second.port_no);
						}
						cse4589_print_and_log("[%s:END]\n", command_str.c_str());
					}	
					
					if(command_str.compare("STATISTICS") == 0)
					{
						int list_id=0;
						cse4589_print_and_log("[%s:SUCCESS]\n", command_str.c_str());
						for(it=smap.begin();it!=smap.end();it++)
						{
							cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n",++list_id,it->second.hostname,it->second.msg_sent,it->second.msg_recv,it->second.status);
						}
						cse4589_print_and_log("[%s:END]\n", command_str.c_str());
					}
					
					if(command_str.substr(0,7).compare("BLOCKED") == 0)
					{
						int j=0,spaces=0,list_id=0;
						char message[256],c1[200],c2[200];
						string word[2];
						
						for(int i=0;i<command_str.length();i++)
						{
							if (command_str[i] == ' ')
							{
								spaces++;
								if(spaces == 1) {
									//cout<<"Word: "<<word[j];
									j++;
									continue;
								}
								else{
									//cout<<"word: "<<word[j];
									j++;
									goto label8;
								}
							}
							label8:
							word[j] += command_str[i];
						}
						memset(message,'\0',256);
						strcpy(message,word[1].data());
						
						int ret = validateIP(word[1]);
						if(ret == 0)
						{
							cse4589_print_and_log("[%s:ERROR]\n", command_str.substr(0,7).c_str());
							cse4589_print_and_log("[%s:END]\n",command_str.substr(0,7).c_str() );
							continue;
						}
						else
						{
							int count=0;
							for(it=smap.begin();it!=smap.end();it++)
							{
								if(strcmp(message,it->second.ip_addr) == 0)
								{
									count++;
								}
								else
								{
									continue;
								}
							}
							if(count>0)
							{
								cse4589_print_and_log("[%s:SUCCESS]\n", command_str.substr(0,7).c_str());
								for(itl=lmap.begin();itl!=lmap.end();itl++)
								{
									memset(c1,'\0',200);
									strcpy(c1,itl->first.data());
									if(strcmp(c1,message) == 0)
									{
										for(itv=itl->second.begin();itv!=itl->second.end();itv++)
										{
											memset(c2,'\0',200);
											memcpy(c2,itv->c_str(),200);
											for(it=smap.begin();it!=smap.end();it++)
											{
												if(strcmp(c2,it->second.ip_addr) == 0)
													cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", ++list_id, it->second.hostname, it->second.ip_addr, it->second.port_no);
											}
										}
									}
								}
								cse4589_print_and_log("[%s:END]\n", command_str.substr(0,7).c_str());
							}
							else
							{
								cse4589_print_and_log("[%s:ERROR]\n", command_str.substr(0,7).c_str());
								cse4589_print_and_log("[%s:END]\n",command_str.substr(0,7).c_str() );
							}
						}
					}
				}
				else if (i==server_sockfd)
				{
					their_addr_size = sizeof their_addr;
					if((new_sockfd = accept(server_sockfd,(struct sockaddr *)&their_addr,&their_addr_size)) == -1)
					{
						close(new_sockfd);
						continue; 
					}
					
					if(bytes_recvd = recv(new_sockfd,buffer,20,0) == -1)
						exit(1);

					getnameinfo((struct sockaddr *)&their_addr, sizeof their_addr, host, sizeof host, service, sizeof service, 0);
					

					FD_SET(new_sockfd,&master);
					if(new_sockfd > fdmax)
						fdmax = new_sockfd;
					
					number_of_clients += 1;		
					//printf("%d\n",number_of_clients);
					
					inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr*)&their_addr),s,INET6_ADDRSTRLEN);
					
					int port_1 = atoi(buffer);
					value.port_no = port_1;
							
					memset(value.ip_addr,'\0', 200);
					strncpy(value.ip_addr,s,sizeof(s));
							
					value.socketid = new_sockfd;
							
					memset(value.hostname,'\0', 200);
					strncpy(value.hostname,host,sizeof(host));
							
					memset(value.status,'\0',20);
					strcpy(value.status,"logged-in");
					printf("%s %s %d %d %s\n",value.hostname,value.ip_addr,value.port_no,value.socketid,value.status);
					smap[port_1] = value;
			
					char car[3000];
					char *car1 = car;
					int count=0;
					memset(car,'\0',3000);
					for(it = smap.begin(); it != smap.end(); ++it)
					{		
						if(strcmp(it->second.status,"logged-in") == 0)
						{
							count++;
							car1 += snprintf(car1,sizeof(it->second.hostname)+sizeof(it->second.ip_addr)+sizeof(it->second.port_no)+1+1+2,"%s %s %d-1",it->second.hostname,it->second.ip_addr,it->second.port_no);
						}
					}
				} 
				else
				{
					memset(buffer,'\0',20);
					if ((bytes_got = recv(i,buffer,20,0)) == -1)
					{
						printf("what command in exit: %s\n",buffer);
						
					}
					
					else
					{						
						if(strcmp(buffer,"EXIT") == 0)
						{
							bytes_recvd=0;
							memset(ebuf,'\0',200);
							recv(i,ebuf,sizeof(IPAddress),0);
							for(it=smap.begin();it!=smap.end();it++)
							{
								if(strcmp(it->second.ip_addr,ebuf) == 0)
								{
									smap.erase(it->first);
								}
							}
						}
						
						else if(strcmp(buffer,"LOUT") == 0)
						{
							bytes_recvd=0;
							memset(ebuf,'\0',200);
							recv(i,ebuf,sizeof(IPAddress),0);
					
							for(it=smap.begin();it!=smap.end();it++)
							{
								if(strcmp(it->second.ip_addr,ebuf) == 0)
								{
									strcpy(it->second.status,"logged-out");
									break;
								}
								else
									continue;
							}
							
							close(i);
							FD_CLR(i,&master);
						}
						
						else if(strcmp(buffer,"BLOCK") == 0)
						{
							bytes_recvd=0;
							struct block_unblock bu;
							
							recv(i,block_buf,sizeof(block_unblock),0);
	
							memcpy(&bu,block_buf,sizeof(block_unblock));
							string key(bu.client1_addr);
															
							itl=lmap.find(key);
							if(itl != lmap.end())
							{									
								list<string> values_list1;
								values_list1 = lmap[key];
								values_list1.push_back(bu.client2_addr);	
								lmap[key] = values_list1;										
							}
							else
							{									
								list<string> values_list1;
								values_list1.push_back(bu.client2_addr);
								for(itv=values_list1.begin(); itv!=values_list1.end(); ++itv)
										cout<<*itv;
								lmap[key] = values_list1;
							}	
						}
						
						else if (strcmp(buffer,"UNBLOCK") == 0)
						{
							bytes_recvd=0;
							struct block_unblock bu;
							if ((bytes_recvd = recv(i,block_buf,sizeof(block_unblock),0)) == -1)
								exit(1);
			
							if (bytes_recvd == 0)
							{
								number_of_clients-=1;
								printf("%d\n",number_of_clients);
								close(i);
								FD_CLR(i,&master);
							}
							else
							{
								memcpy(&bu,block_buf,sizeof(block_unblock));
								string key(bu.client1_addr);
								itl=lmap.find(key);
								if(itl != lmap.end())
								{
									
									list<string> values_list1;
									values_list1 = lmap[key];
									values_list1.remove(bu.client2_addr);
									if(values_list1.empty())
										lmap.erase(itl);
									else
										lmap[key] = values_list1;
								}
							}
						}
						
						else if (strcmp(buffer,"LIST") == 0)
						{									
							char car[3000];
							char *car1 = car;
							memset(car,'\0',3000);
							for(it = smap.begin(); it != smap.end(); ++it)
							{		
								if(strcmp(it->second.status,"logged-in") == 0)
									car1 += snprintf(car1,sizeof(it->second.hostname)+sizeof(it->second.ip_addr)+sizeof(it->second.port_no)+1+1+2,"%s %s %d-1",it->second.hostname,it->second.ip_addr,it->second.port_no);
							}
							
							send(i,car,3000,0);							
						}
						
						else if (strcmp(buffer,"REFRESH") == 0)
						{
							char car[3000];
							char *car1 = car;
							memset(car,'\0',3000);
							for(it = smap.begin(); it != smap.end(); ++it)
							{		
								if(strcmp(it->second.status,"logged-in") == 0)
									car1 += snprintf(car1,sizeof(it->second.hostname)+sizeof(it->second.ip_addr)+sizeof(it->second.port_no)+1+1+2,"%s %s %d-1",it->second.hostname,it->second.ip_addr,it->second.port_no);
							}
							
							send(i,car,3000,0);
						}
						
						else if (strcmp(buffer,"RELAY") == 0)
						{
							printf("In relay\n");
							memset(buf,'\0',sizeof(sending));
							recv(i,buf,sizeof(sending),0);
										
								struct sending sdg;
								memset(sdg.cip_addr,'\0',200);
								memset(sdg.ip_addr,'\0',200);
								memset(sdg.msg,'\0',256);
								memcpy(&sdg,buf,sizeof(sending));
								
								if (sdg.b == 0)
								{
									struct send_broad sb;
									char sb_send[sizeof(send_broad)];
									int second_socket,blocked=0,logout=0;
									
									strncpy(sb.cip_addr,sdg.cip_addr,sizeof(sdg.cip_addr));
									strncpy(sb.msg,sdg.msg,sizeof(sdg.msg));
									memset(sb_send,'\0',sizeof(send_broad));
									memcpy(sb_send,&sb,sizeof(send_broad));
																		
									for(itl=lmap.begin();itl!=lmap.end();itl++)  
									{
										if(strcmp(itl->first.c_str(),sdg.ip_addr) == 0)
										{
											for(itv=itl->second.begin(); itv!=itl->second.end(); ++itv)
											{
												if(strcmp(itv->c_str(),sdg.cip_addr) == 0)
												{
													blocked++;
												}
											}
										}
									}
									if(blocked>0)
									{
										for(it=smap.begin();it!=smap.end();it++)
										{
											if(strcmp(it->second.ip_addr,sdg.cip_addr) == 0)
											{
												it->second.msg_sent++;
											}
										}
									}
									
									for(it=smap.begin();it!=smap.end();it++)
									{
										if(strcmp(sdg.ip_addr,it->second.ip_addr) == 0)
										{
											if(strcmp(it->second.status,"logged-out") == 0)
											{
												logout++;
												
											}
											
										}
									}
									
									if(blocked == 0 && logout == 0)
									{
										for(it=smap.begin();it!=smap.end();it++)
										{
											if(strcmp(it->second.ip_addr,sdg.cip_addr) == 0)
											{
												it->second.msg_sent += 1;
											}
											else if(strcmp(it->second.ip_addr,sdg.ip_addr) == 0)
											{
												it->second.msg_recv += 1;
											}
										}
										
										for(it=smap.begin(); it!=smap.end(); ++it)
										{
											if (strcmp((sdg.ip_addr),(it->second.ip_addr)) == 0)
											{
												second_socket = it->second.socketid;
												if (send(second_socket,sb_send,sizeof(send_broad),0) == -1)
												{
													cse4589_print_and_log("[%s:ERROR]\n", "RELAYED");
													cse4589_print_and_log("[%s:END]\n", "RELAYED");
													continue;
												}
											}
										}
										cse4589_print_and_log("[%s:SUCCESS]\n", "RELAYED");
										cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", sdg.cip_addr, sdg.ip_addr, sdg.msg);
										cse4589_print_and_log("[%s:END]\n", "RELAYED");
									}
								}
								else if (sdg.b == 1)
								{
									struct send_broad sb;
									char sb_send[sizeof(send_broad)];
									
									strncpy(sb.cip_addr,sdg.cip_addr,sizeof(sdg.cip_addr));
									strncpy(sb.msg,sdg.msg,sizeof(sdg.msg));
									memset(sb_send,'\0',sizeof(send_broad));
									memcpy(sb_send,&sb,sizeof(send_broad));
									
									for (int j=0;j<=fdmax;j++)
									{	
										if(FD_ISSET(j,&master)) 
										{
											if ((j != server_sockfd) && j!=i)
											{
												if(send(j,sb_send,sizeof(send_broad),0) == -1)
												{
													cse4589_print_and_log("[%s:ERROR]\n", "RELAYED");
													cse4589_print_and_log("[%s:END]\n", "RELAYED");
													continue;	
												}
											}
										}
									}
									cse4589_print_and_log("[%s:SUCCESS]\n", "RELAYED");
									cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", sdg.cip_addr, "255.255.255.255", sdg.msg);
									cse4589_print_and_log("[%s:END]\n", "RELAYED");
								}
					
						}
					}
			
				}
			}
		}
	}
}

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/* Clear LOGFILE*/
    fclose(fopen(LOGFILE, "w"));

	/*Start Here*/
       char *port_num = argv[2];
	
	if (strcmp(argv[1],"s") == 0)
		server(port_num);
	else if(strcmp(argv[1],"c") == 0)
	{
		client(port_num);
	}
	
	return 0;
}