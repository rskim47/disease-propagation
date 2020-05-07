// Name: Sungsoo Kim (sk38224)
// Email: rickkim95@gmail.com
// Project: F2018 COE 322 Final Project 
#include <iostream>
#include <random>
#include <vector>
#include <iomanip> 
#include <math.h>

using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::vector;
using namespace std; 

class Person {
	protected: 
	int status; // holds disease status for 
	string name;// name of person 
	public:
	Person(){   // basic contructor 
		status = 0;
		name = "UNKNOWN"; 
	}
	Person(string id,  int s){ // main constructor 
		status = s; 
		name = id;
	}
	void  print_status(){      // Prints current status of person 
		if (status > 0){
			cout << name << " is sick (" << status << " to go)"<< endl; 
		}
		else if (status == 0) {
			cout << name << " is susceptible" << endl; 
		}
		else if (status == -1) {
			cout << name << " is recovered" << endl;
		}
		else if (status == -2) {
			cout << name << " is innoculated" << endl; 
		}
	}
	int update(){		   // Updates the status to next day, returns status value  
		if (status > 0) {
			status = status - 1; // status > 0 indicates the # of days the person will be sick 
		}	
		return status; 
		print_status();    // calling print function 
	} 
	void infect(int n){        // takes day of sickness, measures the chances of sickness & changes status accordingly 
		float badluck  = (float) rand() / (float)RAND_MAX;
		if (badluck > .95){
			if (status > -1){
				status = n; 
			}
		}	 
	}
	void infect(int n, int interaction,vector <Person> people){ // Simulates random transmission between individuals 
		int index = 0;
		for (int i = 0; i < interaction; i++){
			index = (int) rand() % people.size(); // random index generation 
			if (people[index].getStatus() > 0 && (float) rand() / (float)RAND_MAX > 0.95) { // & contact with sick person & chance of transmission
				status = n;
				break; // exits for loop once infected 
			}
		}
	}
	int  getStatus(){ 	// returns current status
		return status;
	}
	string getName(){ 	// returns name 
		return name; 
	}
	void setStatus(int s){  // sets status of person 
		status = s; 
	}
};
class Population : Person {     // object composed of multiple person (inheritance used) 
	protected:
	vector <Person> people; // vector of people 
	int day; 
	public:
	Population(){		// basic constructor 
		int size = 100; 
		setVector(size);
		day = 1;
	}
	Population(int n){ 	// main constructor 
		setVector(n);
		day = 1; 
	}
	void random_infection(){// intial random infection function 
		for ( auto &e : people){
			e.infect(5); // calls Person function
		}	
	}
	int count_infection() { // returns total # of infection 
		int count = 0; 
		for ( auto &e : people) {
			if ( e.getStatus() > 0) {
				count = count + 1; 
			}
		}
		return count; 
	}
	void update_population(int interaction) {	// updates population based on interaction per day until no one's sick 
		current_status(); 		  	// outputs current status 
		while ( count_infection() > 0) {	// checks for a sick person 
			day = day + 1; 
			dayOutput(); 
			int countP = 0;			// total sickness
			int countN = 0; 		// total potential patients (yet to be sick)
			int countR = 0;			// total recovered / innoculated 
			for ( auto &e : people) {
				if ( e.getStatus() == 1) {	// for sick individuals  on last day of sickness 
					e.setStatus(-1); 
				}
				e.update();
				if ( e.getStatus() > 0) {	// sick individuals 
					//cout << "+ ";
					countP++; 
				}
				else if ( e.getStatus() == 0) { // yet-to be infected individuals 
					e.infect(5,interaction,people);
					if (e.getStatus() == 0) { 
						//cout << "? ";
						countN++;
					}
					else {
						//cout << "+ ";
						countP++;
					} 
				}
				else if ( e.getStatus() <= -1) {// reoovered & innoculated 
					//cout << "- "; 
					countR++;
				}
			}
		cout << "Inf: " << left << setw(10) << countP << "    Non-inf: " << left << setw(10) << countN << "    Rec OR Inn: " << left << countR << endl;					      // status output 
		}
	}
	void current_status() {				// outputs current status of population 
		dayOutput();
		int countP = 0;				// total sickness
		int countN = 0;				// total potential patients (yet to be sick)
		int countR = 0;				// total recovered / innoculated 
		for ( auto &e : people) {
			if ( e.getStatus() > 0) {	// for sick individuals 
                        	//cout << "+ ";
				countP++;
                        }
                        else if ( e.getStatus() == 0) {	// for potential patients 
                        	//cout << "? ";
				countN++;
                       	}
                        else if ( e.getStatus() <= -1) {// for recovered & innoculated 
                        	//cout << "- ";
				countR++;
			}
		}		
		cout << "Inf: " << left  << setw(10) <<  countP  << "    Non-inf: " << left << setw(10) <<  countN << "    Rec OR Inn: " << left << countR << endl;			      // status output 
	}	
	vector<Person> getVector(){			// returns vector of Person 
		return people; 
	}
	void setVector(int i){				// sets vector of Person 
		Person random;
		vector<Person> people1(i);
		for (auto &e : people){
			e = random;
		}
		people = people1; 
	}
	void setInnoc(int i){				// sets innoculated people 
		for (int j = 0; j < i; j++) {
			people[j].setStatus(-2); 
		}	
	}
	int getDay(){					// returns day 
		return day;
	}
	void dayOutput(){				// print function for arrangement
		if (day > 9  && day < 100){
		 	cout << "<  DAY  " << day << " >   ";
		}
		else if (day < 10) {
			cout << "<  DAY   " << day << " >   ";
		}
		else if (day > 99 && day < 1000){
			cout << "<  DAY " << day << " >   ";
		}
	}
};	
int main() {
	int pop; 	// population size
	int inn; 	// # of innoculated 
	int inter;	// # of personal interaction per day 
	int off = 1;  	// swtich for terminating program; 

	while (off != 0) {
		cout << "Enter Population: ";
		cin >> pop;
		cout << "Enter # of Innoculated People: "; 
		cin >> inn;
		cout << "Enter # of Interaction Per Day: "; 
		cin >> inter; 
		cout << endl;
		cout << right << setw(41) << "< SIMULATION >" << endl;
		cout << endl;

		Population p1(pop);
		p1.setInnoc(inn);
		p1.random_infection(); 
		p1.update_population(inter);
 
		cout << endl;
		cout << "Another Simulation? 1 for YES | 0 for NO: " << endl;
		cin >> off; 
		while  ( off != 1 &&  off != 0){
			cout << "ERROR: Re-enter the value: 1 (YES) OR 0 (NO): " << endl;
			cin >> off; 
		}
	}
	cout << endl;
	cout << "Program Terminated" << endl; 
	cout << "Rick Sungsoo Kim, 2018" << endl;
	cout << "The University of Texas at Austin, Computational Engineering" << endl;
}