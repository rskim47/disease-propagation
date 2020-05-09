// Disease Propagation Model with Parallel Implementation (OpenMP)
#include <iostream>
#include <cstdlib>
#include <random>
#include <iomanip>
#include <vector>
#include <cmath>
#include <ctime>
#include <time.h>
#include <sys/time.h> 
#include <fstream>
#include <tuple>
#include "Person.cpp"
using namespace std;
#ifdef _OPENMP
#include <omp.h>
#endif
 

// Macros
#define INFECTION_RATE 0.5 													// 50% 
#define INTERACTION_NUM {12,3}  										// Interactions per Day (Mean, SD)
#define DORMANT_PERIOD 14  													// Days Staying Dormant  
#define RECOVERY_PERIOD {14.0,2.0} 									// Recovery Period (Mean, SD)
#define CHANCES (float) rand() / (float)RAND_MAX 		// Chances in Life


// ================================================================================================
// Population Class
// ================================================================================================
class Population : Person {     // object composed of multiple person (inheritance used) 
	protected: 
	vector <Person> people;
	int day; 							
	public:
	Population(){		
		long size = 10000; 
		setPopulation(size);
		day = 0;
	}
	Population(long population_size){ 	
		setPopulation(population_size);
		day = 0; 
	}

	void outputStatus(long countP, long countN, long countR) {
		cout << "Inf: " << left << setw(10) << countP << "    Non-inf: " << left << setw(10) << countN << "    Rec OR Inn: " << left << countR << endl;					   
	}

	void dayOutput(){				// print function for arrangement
		printf("<  DAY  %d >   ", day) ;
	}

	void current_status() {				// outputs current status of population 
		dayOutput();
		long countP = 0;				// total sickness
		long countN = 0;				// total potential patients (yet to be sick)
		long countR = 0;				// total recovered / innoculated 
		for ( auto &e : people) {
			if ( e.getStatus() > 0) {	// for sick individuals 
          countP++;
			}
			else if ( e.getStatus() == 0) {	// for potential patients 
				countN++;
			}
			else if ( e.getStatus() <= -1) {// for recovered & innoculated 
				countR++;
			}
		}		
		outputStatus(countP,countN,countR);
	}

	auto countStatus() { 																	   // returns total # of infection 
		// Set as reduction 
		long countP = 0;			// total sickness
		long countN = 0; 		  // total potential patients 
		long countR = 0;			// total recovered / innoculated 
		long sickTotal = 0;
		int status;
		for ( long i = 0; i < people.size(); i++) {
			status = people.at(i).getStatus();
			if ( status < 0 ) {
				countR++; 
			} else if ( status == 0 ) {
				countN++; 
			} else {
				countR++; 
			}
		}
		struct result { long p; long n; long r;};
		return result {countP, countN, countR};
	}	

	//  1st Person Daily Interaction Simulation 
	// ================================================================================================
	Person simulateDailyInteraction(Person person, long id){ 
		long index = -1;
		int interaction_count = 0;
		bool infected = false; 
		bool found = false; 
		while (interaction_count < person.getInteraction()) {
			found = false;
			while (id != index && found == false) {		 					// Making sure we don't choose himself/herself 
				index = (long) rand() % people.size(); 	 					// Random Search 
				if (people.at(index).getStatus() < DORMANT_PERIOD) { // Checking for Self-Isolation
					found = true; 
				}
			}
			if (infected == false){															// Social Interaction Infection
				if (people.at(index).getStatus() > 0 &&  CHANCES <= INFECTION_RATE) { 
					person.infected(); 
				} 
			} else { 																						// Infecting Others
				if (people.at(index).getStatus() == 0) {
					people.at(index).infected();
					infected = true;
				}
			}
			interaction_count++;
		}
		person.setInteractionNum();

		return person;
	}	

	// Set & Get  	
	// ================================================================================================
	vector<Person> getVector(){			 
		return people; 
	}
	
	void setPopulation(long size){	
		Person random;
		for (long i = 0; i < size; i++){
			people.push_back(random); 
		}
	}

	void setInitInnoculated(long innoculate_num){									// sets innoculated people 
		printf("Setting Inoculated \n");
		for (long j = 0; j < innoculate_num; j++) {
			people.at(j).setStatus(-2); 
		}	
	}

	void setInitInfection(long infection_num, long index) {
		printf("Setting Initial Infection \n");
		long final_num = infection_num + index;
		cout << final_num << endl;
		for (long j = index; j < people.size(); j++) {
			people.at(j).setStatus(1);
		}
	}

	void setInitialCond(long innoculate_num,long infection_num) {
		printf("Setting Initial Conditions \n");

		setInitInnoculated(innoculate_num);
		setInitInfection(infection_num,innoculate_num);
	}

	int getDay(){					// returns day 
		return day;
	}

	// Update Population 
	// ================================================================================================
	void updatePopulation(int days) {	// updates population based on interaction per day until no one's sick 
		current_status(); 		  // outputs current status 
		std::ofstream outputFile; 
		outputFile.open("Disease_Simualation.csv");
		outputFile << "Total Population :," <<  people.size() << endl;
		outputFile << "Day, Total Sickness, Uninfected, Recovered / Innoculated" << endl;
		long countP, countN, countR;
		while (days > day) {	
			day++; 
			dayOutput(); 
			for (long i = 0; i < people.size(); i++) { 
				int status = people.at(i).getStatus();
				if (status >= 0 && status <= DORMANT_PERIOD) {								// Infected & In Dormant Period 
					people.at(i) = simulateDailyInteraction(people.at(i),i);		// Simulating Daily Interaction 
				} 
			}
			auto result = countStatus();
			countP = result.p;
			countN = result.n;
			countR = result.r;
			outputFile <<  day << "," << countP << "," << countN << "," << countR << endl;
			outputStatus(countP,countN,countR);
		}
		outputFile.close();
		printf("Data saved to file!");
	}

	
};		
// ================================================================================================
// Main Function 
// ================================================================================================
int main() {
	long pop_size = 2000; 	// population size
	long inn_size = 20; 	// Number of Immune People  
	long infect_size = 20; 
	int off = 1;  	// Code Termination Switch; 
	int days = 30;

// Simulation Variables 
// ================================================================================================
	// cout << "Enter Population: ";
	// cin >> pop_size;
	// cout << "Enter # of Intially Infected People: ";
	// cin >> infect_size;
	// cout << "Enter # of Innoculated People: "; 
	// cin >> inn_size;
	// cout << "Enter # of Days to Simulate: "; 
	// cin >> days;
	// cout << endl;
	cout << right << setw(41) << "< SIMULATION >" << endl;
	cout << endl;

	Population p1(pop_size);
	cout << "Population Generated" << endl;
	p1.setInitialCond(inn_size,infect_size); 
	cout << "Initial Conditions Set" << endl;
	p1.updatePopulation(days);

	cout << endl;
	cout << "Program Terminated" << endl; 
	cout << "Rick Sungsoo Kim, 2020" << endl;
	cout << "The University of Texas at Austin, Cockrell School of Engineering" << endl;
}