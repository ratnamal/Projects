#include<iostream>
#include<iomanip>
#include<fstream>
#include<cstring>
using namespace std;
static int p=0;
class buses
{
	char busname[20],arrivaltime[5],departtime[5],from[10],to[10];
	int busnum,seat[8][4];
	public:
	
	//enter bus details
	void binput(void)
	{
		cout<<" Enter bus no: ";
		cin>>busnum;
		cout<<"\n Enter bus name: ";
		cin>>busname;
		cout<<"\n Departure time: ";
		cin>>departtime;
		cout<<"\n Arrival time: ";
		cin>>arrivaltime;
		cout<<"\n From: ";
		cin>>from;
		cout<<"\n To: ";
		cin>>to;
		bempty();
		p++;
	}

	//book a bus ticket
	void bbooking(void)
	{
		int number,st,n;
		cout<<"Bus no: ";
		cin>>number;
		for(n=0;n<p;n++)
		{
			if(busnum==number)
			break;
		}
		while(n<=p)
		{
			cout<<"\n Seat number: ";
			cin>>st;
			if (st>32)
			{
				cout<<"\n There are only 32 seats available in this bus.";
			}
			else
			{
				if (seat[st/4][(st%4)]==0)
				{
					seat[st/4][st%4]=1;
					break;
				}
				else
					cout<<"The seat no. is already reserved.\n";
			}
		}
	}

	//if entering a new bus entry, all seats should be empty
	void bempty(void)
	{
		for(int i=0;i<8;i++)
		{
			for(int j=0;j<4;j++)
			{
				seat[i][j]=0;
			}
		}
	}

	//display bus details of the bus to be booked
	void bshow(void)
	{
		int n;
		int number;
		cout<<"Enter bus no: ";
		cin>>number;
		for(n=0;n<p;n++)
		{
			if(busnum==number)
			{
				cout<<"******************************************************"<<"\n";
				cout<<"\t\tBus no:\t"<<busnum<<"\n";
				cout<<"\t\tBus name:\t"<<busname<<"\n";
				cout<<"\t\tDeparture time:\t"<<departtime<<"\n";
				cout<<"\t\tArrival time:\t"<<arrivaltime<<"\n";
				cout<<"\t\tFrom:\t"<<from<<"\n";
				cout<<"\t\tTo:\t"<<to<<"\n";
				cout<<"******************************************************"<<"\n";
				bposition();
			}
	}

	//displays details for all buses
	void bavailable(void)
	{
		for(int n=0;n<p;n++)
		{
			cout<<"*******************************************************"<<"\n";
			cout<<"\t\tBus no:\t"<<busnum<<"\n";
			cout<<"\t\tBus name:\t"<<busname<<"\n";
			cout<<"\t\tArrival time:\t"<<arrivaltime<<"\n";
			cout<<"\t\tDeparture Time:\t"<<departtime<<"\n";
			cout<<"\t\tFrom:\t"<<from<<"\n";
			cout<<"\t\tTo:\t"<<to<<"\n";
			cout<<"*******************************************************"<<"\n";
		}
	}

	//displays the seats, their positions and 1 for booked/0 for unbooked
	void bposition(void)
	{
		int s=0,k=0;
		for(int i=0;i<8;i++)
		{
			cout<<"\n";
			for(int j=0;j<4;j++)
			{
				s++;
				if(seat[i][j]==0)
				{
					cout<<setw(5);
					cout<<s<<".";
					cout<<seat[i][j];
					k++;
				}
				else
				{
					cout<<setw(5);
					cout<<s<<".";
					cout<<seat[i][j];
				}
			}
		}
		cout<<"\n\n There are "<<k<<"seats empty in bus"<<"\n";
	}
	
	// cancellation of the booked ticket by changing value for that seat to 0
	void bcancellation(void)
	{
		int number,st,n;
		cout<<"Enter the bus number"<<"\n";
		cin>>number;
		for(n=0;n<p;n++)
		{
			if(busnum==number)
			{
				cout<<"Enter your seat number"<<"\n";
				cin>>st;
				if(seat[st/4][st%4]==1)
				{
					seat[st/4][st%4]=0;
				}
			}
		}
		bposition();
		cout<<"Your ticket has been cancelled"<<"\n";
	}
};

class train
{       
	public:
	int d,m,y;
	char clas,trainname[20],bdpt[20],dest[20],dtime[5],artime[5];
	int tno,fcfare,scfare,pnr,fseat[40],sseat[40];
	public:
	
	//enter new train details
	void tinput(void)
	{
		cout<<"Enter pnr number: "<<"\n";
		cin>>pnr;
		cout<<"Enter train number: "<<"\n";
		cin>>tno;
		cout<<"Enter train name: "<<"\n";
		cin>>trainname;
		cout<<"Enter boarding point: "<<"\n";
		cin>>bdpt;
		cout<<"Enter destination: "<<"\n";
		cin>>dest;
		cout<<"Enter departure time: "<<"\n";
		cin>>dtime;
		cout<<"Enter arrival time: "<<"\n";
		cin>>artime;
		cout<<"Enter first class fare: "<<"\n";
		cin>>fcfare;
		cout<<"Enter second class fare: "<<"\n";
		cin>>scfare;
		tempty();
	}
	
	void toutput(void)
	{
		cout<<"***************************************************"<<"\n";
		cout<<"\t\tTrain number: "<<tno<<"\n";
		cout<<"\t\tTrain name: "<<trainname<<"\n";
		cout<<"\t\tBoarding point: "<<bdpt<<"\n";
		cout<<"\t\tDestination point: "<<dest<<"\n";
		cout<<"\t\tDeparture time: "<<dtime<<"\n";
		cout<<"\t\tArrival time: "<<artime<<"\n";
		cout<<"***************************************************"<<"\n";
	}
	
	//booking first/second class ticket
	void tbooking(void)
	{
		int st,i=1;
		cout<<"Enter First class or Second class [(f/s)]: "<<"\n";
		cin>>clas;
		cout<<"Enter date of travel(dd/mm/yyyy): "<<"\n";
		cin>>d>>m>>y;
		if(clas=='f')
		{
			while(i!=0)
			{
				cout<<"Enter seat number of your choice: "<<"\n";
				cin>>st;
				if(st>40)
					cout<<"There are only 40 seats in the class"<<"\n";
				else
				{
					if(fseat[st]==0)
					{
						fseat[st]=1;
						break;
					}
					else
						cout<<"The seat is already reserved by another passenger"<<"\n";
				}
			}
		}
		else if(clas=='s')
		{
			while(i!=0)
			{
				cout<<"Enter seat number of your choice: "<<"\n";
				cin>>st;
				if(st>40)
					cout<<"There are only 40 seats available"<<"\n";
				else
				{
					if(sseat[st]==0)
					{
						sseat[st]=1;
						i--;
					}
					else
					{
						cout<<"The seat is already reserved by another passenger"<<"\n";
					}
				}
			}
		}
	}
	
	//if entering a new train entry, all seats should be empty
	void tempty(void)
	{
		int i;
		if(clas=='f')
		{
			for(i=0;i<40;i++)
			{
				fseat[i]=0;
			}
		}
		else if(clas=='s')
		{
			for(i=0;i<40;i++)
			{
				sseat[i]=0;
			}
		}
	}
	void displaytrain(void)
	{
		cout<<"-----------YOUR RESERVATION-------------"<<"\n";
		cout<<"PNR number: "<<pnr<<"\n";
		cout<<"Train number: "<<tno<<"\n";
		cout<<"Train name: "<<trainname<<"\n";
		cout<<"Boarding point: "<<bdpt<<"\n";
		cout<<"Destination: "<<dest<<"\n";
		cout<<"Class: "<<clas<<"\n";
		cout<<"Date of departure: "<<d<<"/"<<m<<"/"<<y<<"\n";
		if(clas=='f')
			cout<<"Fare is: "<<fcfare<<"\n";
		else if(clas=='s')
			cout<<"Fare is: "<<scfare<<"\n";
		cout<<"-----------------------------------------"<<"\n";
	}
	
	//cancellation of a booked ticket
	void tcancellation(void)
	{
		int st,i=1;
		if(clas=='f')
		{
			cout<<"Enter seat number of your choice: "<<"\n";
			cin>>st;
			if(fseat[st]==1)
			{
				fseat[st]=0;
			}
		}
		else if(clas=='s')
		{
			cout<<"Enter seat number of your choice: "<<"\n";
			cin>>st;
			if(sseat[st]==1)
			{
				sseat[st]=0;
			}
		}
		cout<<"Your ticket has been cancelled"<<"\n";
	}
};

class passenger:public buses,public train
{
	char name[20],addr[20],email[20],job[20],password[20];
	long phno;
	int date,month,year;
	public:
	void getdata()
	{
		cout<<setw(30)<<"Enter the details:"<<"\n";
		cout<<"Enter the name:"<<"\n";
		cin>>name;
		cout<<"Enter the date of birth (dd/mm/yyyy):"<<"\n";
		cin>>date>>month>>year;
		cout<<"Enter the address:"<<"\n";
		cin>>addr;
		cout<<"Enter the phone number:"<<"\n";
		cin>>phno;
		cout<<"Enter the email id:"<<"\n";
		cin>>email;
		cout<<"Enter the occupation:"<<"\n";
		cin>>job;
		cout<<"Enter the password you want:"<<"\n";
		cin>>password;
	}
	void showdata()
	{
		cout<<setw(30)<<"Your details"<<"\n";
		cout<<"Name: "<<name<<"\n";
		cout<<"Date of birth: "<<date<<"/"<<month<<"/"<<year<<"\n";
		cout<<"Address: "<<addr<<"\n";
		cout<<"Phone number: "<<phno<<"\n";
		cout<<"Email id: "<<email<<"\n";
		cout<<"Occupation: "<<job<<"\n";
	}
	void changeadd()
	{
		cout<<"Enter the new address:"<<"\n";
		cin>>addr;
	}
	void changeph()
	{
		cout<<"Enter the new number:"<<"\n";
		cin>>phno;
	}
	void changepass()
	{
		cout<<"Enter the new password:"<<"\n";
		cin>>password;
	}
	char* getname()
	{
		return name;
	}
	char* getpass()
	{
		return password;
	}
};

//sign in for a user
int signin()
{
	passenger pa;
	int i=1;
	char pass[20];
	fstream file("reservation.dat",ios::in);
	while(file && i<=3)
	{
		cout<<"Enter password:"<<"\n";
		cin>>pass;
		if(strcmp(pa.getpass(),pass))
		{
			cout<<"CORRECT PASSWORD"<<"\n";
			break;
		}
		else
		{
			cout<<"WRONG PASSWORD"<<"\n";
			i++;
		}
	}
	return i;
}

void account()
{
	passenger pa[3];
	int i;
	fstream file("reservation.dat",ios::out);
	for(i=0;i<3;i++)
	{
		pa[i].getdata();
		file.write((char*)&pa[i],sizeof(pa[i]));
	}
	file.close();
}

//creating a new account in the system
void signup()
{
	passenger pa;
	fstream file("reservation.dat",ios::app);
	pa.getdata();
	file.write((char*)&pa,sizeof(pa));
	file.close();
}

//updating details of account
void update()
{
	passenger pa;
	int cg;
	char nm[20],nm1[20];
	fstream file("reservation.dat",ios::in);
	fstream file2("ticket.dat",ios::out);
	cout<<"Enter the name"<<"\n";
	cin>>nm;
	while(file)
	{
		file.read((char*)&pa,sizeof(pa));
		if(!strcmp(pa.getname(),nm))
		{
			cout<<"What do you want to change:"<<"\n";
			cout<<"1: ADDRESS"<<"\n";
			cout<<"2: PHONE NUMBER"<<"\n";
			cout<<"3: PASSWORD"<<"\n";
			cin>>cg;
			switch(cg)
			{
				case 1:
				 pa.changeadd();
				 break;
				case 2:
				 pa.changeph();
				 break;
				case 3:
				 pa.changepass();
				 break;
			}
		}
		file2.write((char*)&pa,sizeof(pa));
	}
	file.close();
	file2.close();
	remove ("reservation.dat");
	rename ("ticket.dat","reservation.dat");
	int i;
	fstream file3("reservation.dat",ios::in);
	file3.seekg(0);
	cout<<"Enter the name"<<"\n";
	cin>>nm1;
	while(file3)
	{
		file3.read((char*)&pa,sizeof(pa));
		if(!strcmp(pa.getname(),nm1))
		{
			pa.showdata();
		}
	}
	file3.close();
}

//deleting a account from the system
void deletion()
{
	passenger pa[3];
	char nm[20];
	int i;
	fstream file("reservation.dat",ios::in);
	fstream file2("tick.dat",ios::out);
	cout<<"Enter the name"<<"\n";
	cin>>nm;
	for(i=0;i<3;i++)
	{
		file.read((char*)&pa[i],sizeof(pa[i]));
		if(strcmp(pa[i].getname(),nm))
		{
			file2.write((char*)&pa[i],sizeof(pa[i]));
		}
	}
	file.close();
	file2.close();
	remove("reservation.dat");
	rename("tick.dat","reservation.dat");
	fstream file3("reservation.dat",ios::in);
	file3.seekg(0);
	for(i=0;i<2;i++)
	{
		file3.read((char*)&pa[i],sizeof(pa[i]));
		pa[i].showdata();
	}
	file3.close();
}

void display()
{
	passenger pa[3];
	int i;
	fstream file("reservation.dat",ios::in);
	for(i=0;i<3;i++)
	{
		file.read((char*)&pa[i],sizeof(pa[i]));
		pa[i].showdata();
	}
	file.close();
}

int main()
{
	passenger pa;
	cout<<setw(20)<<"-------------------------------------------------------"<<endl;
	cout<<setw(40)<<"INDIAN ONLINE RESERVATION SYSTEM"<<endl;
	cout<<setw(20)<<"-------------------------------------------------------"<<endl;
	char ch,ch2,ch3,c,a,b,EmpOrUser[20];
	int r,busOrtrain,menu_number,i=0;
	do
	{
		cout<<"Employee OR User ????"<<"\n";
		cin>>EmpOrUser;
		if(strcmp(EmpOrUser,"EMPLOYEE")==0)
		{
			account();
			cout<<"******************************************"<<"\n";
			display();
			cout<<"---------||||||BUS DETAILS||||||----------"<<"\n";
			pa.binput();
			cout<<"---------||||||TRAIN DETAILS||||||----------"<<"\n";
			pa.tinput();
		}
		else if(strcmp(EmpOrUser,"USER")==0)
		{
			label:
			cout<<"Do you have an account (y/n)or(Y/N):"<<endl;
			cin>>ch;
			if(ch=='y'||ch=='Y')
			{
				r=signin();
				if(r>3)
				{
					cout<<"XXXXXXXXX---INTRUDER---XXXXXXXXX"<<endl;
				}
				else
				{
					do
					{
						cout<<"MENU:"<<endl;
						cout<<"1: TRAVEL"<<endl;
						cout<<"2: UPDATE"<<endl;
						cout<<"3: DELETE"<<endl;
						cin>>menu_number;
						switch(menu_number)
						{
							case 1:
								cout<<"MENU:"<<endl;
								cout<<"1: BUS"<<endl;
								cout<<"2: TRAIN"<<endl;
								cin>>busOrtrain;
								switch(busOrtrain)
								{
									case 1:
										pa.bavailable();
										up:
										pa.bbooking();
										pa.bshow();
										cout<<"Do you want to cancel the ticket you have booked (y/n)or(Y/N):"<<endl;
										cin>>ch2;
										if(ch2=='y'||ch2=='Y')
											pa.bcancellation();
										cout<<"Do you want to book another ticket (y/n)or(Y/N):"<<endl;
										cin>>c;
										if(c=='y'||c=='Y')
											goto up;
										break;
									case 2:
										pa.toutput();
										down:
										pa.tbooking();
										pa.displaytrain();
										cout<<"Do you want to cancel the ticket you booked (y/n)or(Y/N):"<<endl;
										cin>>a;
										if(a=='y'||a=='Y')
											pa.tcancellation();
										cout<<"Do you want to book another ticket???"<<"\n";
										cin>>b;
										if(b=='y'||b=='Y')
											goto down;
										break;
								}
								cout<<"*********************************************THANK YOU***********************************************"<<endl;
								break;
							case 2:
								update();
								break;
							case 3:
								deletion();
								break;
						}
					cout<<"Do you want to continue(y/n)or(Y/N)"<<"\n";
					cin>>ch3;
					}while(ch3=='y'||ch3=='Y');
				}
			}
			else
			{
				signup();
				goto label;
			}
		}
		i++;
	}while(i<2);
	return 0;
}

