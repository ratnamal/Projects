#include "../include/simulator.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <list>

using namespace std;

/* ******************************************************************
 GO-BACK-N NETWORK EMULATOR: 
 
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
};

list <pkt_list> p_l;
list <pkt_list>::iterator it;
list <pkt_list> p_l1;
list <pkt_list>::iterator it1;

struct pkt p;
int base,seq_num,expectedseqnum,old_ack;
char msg[20];

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
	int s=0,c;
	struct pkt_list pl;
	
	if(seq_num < (base+getwinsize()))
	{
		p.acknum = 0;
		memset(p.payload,'\0',20);
		strncpy(p.payload,message.data,20);
		
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
		
		p_l.push_back(pl);
		
		tolayer3(0,p);
		if(base == seq_num)
			starttimer(0,20.0);
		
		p.seqnum++;
		seq_num = p.seqnum;
	}
	else
	{
		p.acknum = 0;
		memset(p.payload,'\0',20);
		strncpy(p.payload,message.data,20);
		
		for(int i=0;i<strlen(p.payload);i++)
			s+= p.payload[i];
		p.checksum = s + p.seqnum + p.acknum;
		
		pl.seqnum = p.seqnum;
		pl.acknum = p.acknum;
		pl.checksum = p.checksum;
		memset(pl.payload,'\0',20);
		strncpy(pl.payload,p.payload,20);
		
		p_l1.push_back(pl);
		p.seqnum++;
		seq_num = p.seqnum;
	}
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
	int c;
	struct pkt_list pl;
	struct pkt pack;
	
	c = packet.seqnum + packet.acknum;
	if(c == packet.checksum)
	{
		if(packet.acknum == base)
		{
			if(!p_l.empty())
			{
				old_ack = packet.acknum;
				p_l.pop_front();
				base = packet.acknum + 1;
				if(!p_l1.empty())
				{
					it1 = p_l1.begin();
					pl.seqnum = it1->seqnum;
					pl.acknum = it1->acknum;
					pl.checksum = it1->checksum;
					memset(pl.payload,'\0',20);
					strncpy(pl.payload,it1->payload,20);
					p_l.push_back(pl);
					p_l1.pop_front();
					
					pack.seqnum = pl.seqnum;
					pack.acknum = pl.acknum;
					pack.checksum = pl.checksum;
					memset(pack.payload,'\0',20);
					strncpy(pack.payload,pl.payload,20);
					tolayer3(0,pack);
				}
				
				if (p_l.empty())
					stoptimer(0);
				else
					starttimer(0,20.0);
			}
			
		}
	}
	else
		cout<<"Corrupted or Old ACK"<<endl;
	
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
	struct pkt pack;

	for(it=p_l.begin();it!=p_l.end();it++)
	{
		pack.seqnum = it->seqnum;
		pack.acknum = it->acknum;
		pack.checksum = it->checksum;
		memset(pack.payload,'\0',20);
		strncpy(pack.payload,it->payload,20);
		tolayer3(0,pack);
	}
	starttimer(0,20.0);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	p.seqnum = 1;
	p.acknum = 0;
	p.checksum = 0;
	memset(p.payload,'\0',20);
	seq_num = p.seqnum;
	base = 1;
	old_ack = 0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
	int s=0,c;
	
	memset(msg,'\0',20);
	strncpy(msg,packet.payload,20);
	
	for(int i=0;i<20;i++)
		s+= msg[i];
	c = packet.seqnum + packet.acknum + s;
	
	if(c == packet.checksum)
	{
		if(packet.seqnum == expectedseqnum)
		{
			tolayer5(1,msg);
			struct pkt ack;
			ack.seqnum = 0;
			ack.acknum = expectedseqnum;
			ack.checksum = ack.seqnum + ack.acknum;
			tolayer3(1,ack);
			expectedseqnum++;
		}
	}
	else
	{
		struct pkt ack;
		ack.seqnum = 0;
		ack.acknum = old_ack;
		ack.checksum = ack.seqnum + ack.acknum;
		tolayer3(1,ack);
	}
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
	expectedseqnum = 1;
}
