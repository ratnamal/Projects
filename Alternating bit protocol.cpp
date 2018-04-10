#include "../include/simulator.h"
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <map>
#include<list>
#include<string>

using namespace std;

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

struct pkt_list
{
	int seqnum;
	int acknum;
	int checksum;
	char payload[20];
};

list <pkt_list> p_list;
list <pkt_list> :: iterator it; 

struct pkt p;
struct pkt p1;
int pkt_in_transit, Aseq_num, prev_Aseq_num, Bseq_num;
char msg[20];

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
	int len,s=0;
	struct pkt_list pm;
	
	p.seqnum = Aseq_num;
	p.acknum = 0;
	memset(p.payload,'\0',20);
	strncpy(p.payload,message.data,20);
	
	for(int i=0;i<20;i++)
		s+= p.payload[i];
	p.checksum = s + p.seqnum + p.acknum;
	cout<<"pkt_in_transit: "<<pkt_in_transit<<endl;
	if (pkt_in_transit == 1)
	{
		pm.seqnum = p.seqnum;
		pm.acknum = p.acknum;
		pm.checksum = p.checksum;
		memset(pm.payload,'\0',20);
		strncpy(pm.payload,p.payload,20);
		
		p_list.push_back(pm);
		
		if (Aseq_num == 0)
			Aseq_num = 1;
		else
			Aseq_num = 0;
	}
	else
	{		
			p1.seqnum = p.seqnum;
			prev_Aseq_num = p.seqnum;
			p1.acknum = p.acknum;
			p1.checksum = p.checksum;
			
			strncpy(p1.payload,p.payload,20);
			
			if (Aseq_num == 0)
				Aseq_num = 1;
			else
				Aseq_num = 0;
			
			tolayer3(0,p1);
			pkt_in_transit = 1;
			starttimer(0,20.0);
	}
	
	
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
	int c,seq;
	c = packet.seqnum + packet.acknum;
	if ((c == packet.checksum) && (packet.acknum == prev_Aseq_num))
	{
		
			stoptimer(0);
			pkt_in_transit = 0;
			if(!p_list.empty())
			{
				it = p_list.begin();
				
				p1.seqnum = it->seqnum;
				prev_Aseq_num = p1.seqnum;
				p1.acknum = it->acknum;
				memset(p1.payload,'\0',20);
				strncpy(p1.payload,it->payload,20);		
				p1.checksum = it->checksum;
				
				tolayer3(0,p1);
				pkt_in_transit = 1;
				starttimer(0,20.0);
			}
		
			if(!p_list.empty())
			{
				it = p_list.begin();
				p_list.pop_front();
			}
		
	}
	else
	{
		cout<<"Corrupted ACK"<<endl;
	}
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
	cout<<"timeout"<<endl;
	tolayer3(0,p1);
	starttimer(0,20.0);
	pkt_in_transit = 1;
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	p.seqnum = 0;
	p.acknum = 0;
	p.checksum = 0;
	memset(p.payload,'\0',20);
	pkt_in_transit = 0;
	Aseq_num = 0;
	prev_Aseq_num = 0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
	int c,s=0,present;
	
	memset(msg,'\0',20);
	strncpy(msg,packet.payload,20);
	
	for(int i=0;i<20;i++)
		s+= msg[i];
	c = packet.seqnum + packet.acknum + s;

	if((c == packet.checksum ) && (packet.seqnum == Bseq_num))
	{
		tolayer5(1,msg);
		struct pkt ack;
		ack.seqnum=0;
		ack.acknum = Bseq_num;
		ack.checksum = ack.seqnum + ack.acknum;
		tolayer3(1,ack);
			
		if(Bseq_num == 0)
			Bseq_num = 1;
		else if(Bseq_num == 1)
			Bseq_num = 0;
	}
	
	else
	{
		struct pkt ack;
		int seq;
		if(Bseq_num == 0)
			seq = 1;
		else if (Bseq_num == 1)
			seq = 0;
		ack.seqnum=0;
		ack.acknum = seq;
		ack.checksum = ack.seqnum + ack.acknum;
		tolayer3(1,ack);
	}
	
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
	Bseq_num = 0;
}
