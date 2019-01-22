#include "../include/simulator.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <list>
#include <map>
using namespace std;

/* ******************************************************************
 SELECTIVE REPEAT NETWORK EMULATOR:

   This code shows unidirectional data transfer protocols (from A to B). 
   Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

struct pkt_list
{
	int seqnum;
	int acknum;
	int checksum;
	char payload[20];
	float time;
};

list <pkt_list> p_l1;
list <pkt_list>::iterator it1;

map <int,pkt_list> p_map;
map <int,pkt_list>:: iterator itm;

struct pmap
{
	int seqnum;
	char payload[20];
};

map <int,pmap> p_map1;
map <int,pmap>::iterator itm1;

struct pkt p;
struct pkt p2;
int send_base,nextseqnum,rcv_base;
char msg[20];

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
	int s=0,c;
	struct pkt_list pl;
				
	memset(p.payload,'\0',20);
	strncpy(p.payload,message.data,20);
		
	if(nextseqnum < (send_base+getwinsize()))
	{
		p.acknum = 0;
		
		for(int i=0;i<20;i++)
		{
			s+= p.payload[i];
		}
		p.checksum = s + p.seqnum + p.acknum;
		
		pl.seqnum = p.seqnum;
		pl.acknum = p.acknum;
		pl.checksum = p.checksum;
		memset(pl.payload,'\0',20);
		strncpy(pl.payload,p.payload,20);
		pl.payload[20] = '\0';
		
		pl.time = get_sim_time();
		tolayer3(0,p);
		starttimer(0,35.0);
		
		p2.seqnum = pl.seqnum;
		p2.acknum = pl.acknum;
		p2.checksum = pl.checksum;
		memset(p2.payload,'\0',20);
		strncpy(p2.payload,pl.payload,20);
		
		p_map[pl.seqnum] = pl;
			
		p.seqnum++;
		nextseqnum = p.seqnum;
		
	}
	else
	{
		p.acknum = 0;		
		for(int i=0;i<strlen(p.payload);i++)
			s+= p.payload[i];
		p.checksum = s + p.seqnum + p.acknum;
		pl.seqnum = p.seqnum;
		pl.acknum = p.acknum;
		pl.checksum = p.checksum;
		memset(pl.payload,'\0',20);
		strncpy(pl.payload,p.payload,20);
		pl.payload[20] = '\0';
		p_l1.push_back(pl);
		p.seqnum++;
		nextseqnum = p.seqnum;
	}
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
	int c,seq;
	float t,min,t1;
	struct pkt_list pl;
	struct pkt pack;
	
	c = packet.seqnum + packet.acknum;
	if(c == packet.checksum)
	{
		if(packet.acknum >= send_base && packet.acknum < send_base+getwinsize())
		{
				stoptimer(0);
				if(!p_map.empty())
				{
					cout<<"130"<<endl;
					int present=0;
					for(itm=p_map.begin();itm!=p_map.end();itm++)
					{
						if(itm->second.seqnum == packet.acknum)
							p_map.erase(itm);
							
					}
					
					for(itm=p_map.begin();itm!=p_map.end();itm++)
					{
						if(itm->second.seqnum < packet.acknum)
							present++;
					}
					
					if(present == 0)
					{
						send_base = packet.acknum + 1;
						if(!p_l1.empty())
						{
							it1 = p_l1.begin();
							pl.seqnum = it1->seqnum;
							pl.acknum = it1->acknum;
							pl.checksum = it1->checksum;
							memset(pl.payload,'\0',20);
							strncpy(pl.payload,it1->payload,20);
							pl.payload[20] = '\0';
							pack.seqnum = pl.seqnum;
							pack.acknum = pl.acknum;
							pack.checksum = pl.checksum;
							memset(pack.payload,'\0',20);
							strncpy(pack.payload,pl.payload,20);
							pack.payload[20] = '\0';
							pl.time = get_sim_time();
							tolayer3(0,pack);

							p_map[pl.seqnum] = pl;
							p_l1.pop_front();
						}
					}
				}	
				
				if(!p_map.empty())
				{
					itm = p_map.begin();
					min = itm->second.time;
					
					for(itm=p_map.begin();itm!=p_map.end();itm++)
					{
						t = 35.0 - (get_sim_time() - itm->second.time);
						if (t <= min)
						{
							min = t;
							seq = itm->second.seqnum;
						}
					}
					
					for(itm=p_map.begin();itm!=p_map.end();itm++)
					{
						if(itm->second.seqnum == seq)
						{
							starttimer(0,min);
							p2.seqnum = itm->second.seqnum;
							p2.acknum = itm->second.acknum;
							p2.checksum = itm->second.checksum;
							memset(p2.payload,'\0',20);
							strncpy(p2.payload,itm->second.payload,20);
							break;
						}
					}
				}
			//}
		}
	}
	else
		cout<<"Corrupted ACK"<<endl;
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
	int seq;
	float t,t0,min,t2;
	t0 = get_sim_time();	
	tolayer3(0,p2);

	for(itm=p_map.begin();itm!=p_map.end();itm++)
	{
		if(itm->second.seqnum == p2.seqnum)
		{
			itm->second.time = get_sim_time();
			break;
		}
	}

	if(!p_map.empty())
	{
		itm = p_map.begin();
		min = itm->second.time;
					
		for(itm=p_map.begin();itm!=p_map.end();itm++)
		{
			t = 35.0 - (t0 - itm->second.time);
			if (t <= min)
			{
				min = t;
				seq = itm->second.seqnum;
			}
		}
		
		itm = p_map.find(seq);
		starttimer(0,min);
		p2.seqnum = itm->second.seqnum;
		p2.acknum = itm->second.acknum;
		p2.checksum = itm->second.checksum;
		memset(p2.payload,'\0',20);
		strncpy(p2.payload,itm->second.payload,20);
	}
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	p.seqnum = 1;
	p.acknum = 0;
	p.checksum = 0;
	memset(p.payload,'\0',20);
	nextseqnum = p.seqnum;
	send_base = 1;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
	int s=0,c,present;
	
	memset(msg,'\0',20);
	strncpy(msg,packet.payload,20);
	msg[20] = '\0';
	for(int i=0;i<20;i++)
		s+= msg[i];
	c = packet.seqnum + packet.acknum + s;
	if(c == packet.checksum)
	{			
		if(packet.seqnum >= rcv_base && packet.seqnum < rcv_base+getwinsize()-1)
		{
			if(packet.seqnum == rcv_base)
			{
				tolayer5(1,msg);
				struct pkt ack;
				ack.seqnum = 0;
				ack.acknum = packet.seqnum;
				ack.checksum = ack.seqnum + ack.acknum;
				tolayer3(1,ack);
					
				itm1 = p_map1.begin();
				while(!p_map1.empty())
				{
					if(itm1->second.seqnum == rcv_base+1)
					{
						char m[20];
						memset(m,'\0',20);
						strncpy(m,itm1->second.payload,20);
						m[20] = '\0';
						tolayer5(1,m);
						p_map1.erase(itm1);
						rcv_base++;
						itm1++;
					}
					else
						break;
				}
				rcv_base++;
			}
			else
			{
				struct pmap pt;
				pt.seqnum = packet.seqnum;
				memset(pt.payload,'\0',20);
				strncpy(pt.payload,msg,20);
				pt.payload[20] = '\0';
				p_map1[pt.seqnum] = pt;
				
				struct pkt ack;
				ack.seqnum = 0;
				ack.acknum = packet.seqnum;
				ack.checksum = ack.seqnum + ack.acknum;
				tolayer3(1,ack);
			}
		}	
		else if (packet.seqnum >= rcv_base-getwinsize() && packet.seqnum <= rcv_base - 1)
		{
			struct pkt ack;
			ack.seqnum = 0;
			ack.acknum = packet.seqnum;
			ack.checksum = ack.seqnum + ack.acknum;
			tolayer3(1,ack);
		}
	}
	else
	{
		cout<<"Corrupted pkt"<<endl;
	}
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
	rcv_base = 1;
}
