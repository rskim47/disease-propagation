// Disease Propagation Model with Parallel Implementation (OpenMP)
#include <random>
#include <vector>
#include <iomanip> 
#include <math.h>
#include <stdexcept> 
#include <fstream>

using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::vector;
using namespace std; 

// Macros
#define INFECTION_RATE 0.5 													// 50% 
#define INTERACTION_NUM {12,3}  										// Interactions per Day (Mean, SD)
#define DORMANT_PERIOD 14  													// Days Staying Dormant  
#define RECOVERY_PERIOD {14,2}  										// Recovery Period (Mean, SD)
#define CHANCES (float) rand() / (float)RAND_MAX 		// Chances in Life


// ================================================================================================
// Person Class 
// ================================================================================================
class Person {
	protected: 
	vector <Person> people;
	int status; 							// (Infected : >= 1, Recovered : -1, Susceptible : 0,  Innoculated: -2)
	int interaction;					// Number of interactions per day 
	int recoveryDate;
		
	// Constructors 
	// ================================================================================================
	public:
	Person(){  
		status = 0; 
		interaction = setInteractionNum(); 
		recoveryDate = settRecoveryPd();
	}
	Person(int s){  
		status = s; 
		interaction = setInteractionNum();
		recoveryDate = setRecoveryPd();
	}

	// Update Functions
	// ================================================================================================
	int nextDay(){		   								// Updates the status to the next day
		if (status > 0) {
			status++; 
			if (recoveryDate > status) {
				status = -1;
			} 
		interaction = setInteractionNum(); // Setting NEW Daily Interaction 
		return status; 
	} 

	void infected() {
		if (status == 0) {
			status++;
		}
	}
	
	// Set & Get
	// ================================================================================================ 
	int getStatus(){ 	
		return status;
	}
	
	void setStatus(int s){  
		status = s; 
	}

	int getInteraction() { 
		return interaction; 
	}

	int getRecoveryPd() {
		return recoveryDate;
	}

	int setInteractionNum() {
		std::default_random_engine generator; 
		std::normal_distribution<double> distribution(INTERACTION_NUM[1],INTERACTION_NUM[2]);

		return (int) distribution(generator);  							// Norm-Distribution Value is Returned 
	}

	int setRecoveryPd() {
		if (status > 0) {
			std::default_random_engine generator; 
			std::normal_distribution<double> distribution(RECOVERY_PERIOD[1],RECOVERY_PERIOD[2]);

			return (int) distribution(generator);  						// Norm-Distribution Value is Returned 
		} else {
			return -1;
		}
	}
};

// ================================================================================================
// Population Class
// ================================================================================================
class Population : Person {     // object composed of multiple person (inheritance used) 
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

	// Update Population 
	// ================================================================================================
	void updatePopulation(int days) {	// updates population based on interaction per day until no one's sick 
		current_status(); 		  // outputs current status 

		std::ofstream outputFile; 
		outputFile.open("Disease_Simualation.csv");
		outputFile << "Total Population - %llu", people.size() << endl;
		outputFile << "Day, Total Sickness, Uninfected, Recovered / Innoculated" << endl;
		while (days > day) {	
			day++; 
			dayOutput(); 
			// Set as reduction 
			long countP = 0;			// total sickness
			long countN = 0; 		  // total potential patients 
			long countR = 0;			// total recovered / innoculated 
			for (long i = 0; i < people.size(); i++) { 
				int status = people[i].getStatus();
				if (status >= 0 && status <= DORMANT_PERIOD) {								// Infected & In Dormant Period 
						simulateDailyInteraction(people[i],i);							    // Simulating Daily Interaction 
				} 
			}

			outputStatus(countP,countN,countR);
			outputFile << " %d, %ld, %ld, %ld", day, countP, countN, countR << endl;
		}
		outputFile.close();
		printf("Data saved to file!");
	}

	//  1st Person Daily Interaction Simulation 
	// ================================================================================================
	void simulateDailyInteraction(Person person, long id){ 
		long index = -1;
		int interaction_count = 0;
		bool infected = false; 
		bool found = false; 
		while (interaction_count < person.interaction) {
			found = false;
			while (id != index && found == false) {		 					// Making sure we don't choose himself/herself 
				index = (long) rand() % people.size(); 	 					// Random Search 
				if (people[index].getStatus() < DORMANT_PERIOD) { // Checking for Self-Isolation
					found = true; 
				}
			}
			if (infected == false){															// Social Interaction 	 
				if (people[index].getStatus() > 0 &&  CHANCES <= INFECTION_RATE) { 
					person.infected(); 
				} 
			} else { 																						// Infecting Others
				if (people[index].getStatus() == 0) {
					people[index].infected();
					infected = true;
				}
			}
			interaction_count++;
		}
	}	

	// Set & Get  	
	// ================================================================================================
	vector<Person> getVector(){			 
		return people; 
	}
	
	void setPopulation(long size){	
		Person random;
		for (long i = 0; i < people.size(); i++){
			people[i] = random;
		}
		people
	}

	long countInfection() { 																	   // returns total # of infection 
		long count = 0; 
		for ( auto &e : people) {
			if ( e.getStatus() > 0) {
				count = count + 1; 
			}
		}
		return count; 
	}

	void setInitInnoculated(long innoculate_num){									// sets innoculated people 
		for (long j = 0; j < innoculate_num; j++) {
			people[j].setStatus(-2); 
		}	
	}

	void setInitInfection(long infection_num, long index) {
		int infection_period; 
		long count = 0; 
		long id = 0;
		for (int j = index; j < infection_num + index && j < people.size()) {
			people[j].setStatus(infection_period);
		}
	}

		void setInitialCond(long innoculate_num,long infection_num) {
			if (innoculate_num + infection_num > people.size()) {
				throw std::invalid_argument("There are too many innoculated people! Re-enter the inputs")
			}
			setInitInnoculated(innoculate_num);
			setInitInfection(infection_num,innoculate_num);
		}
	}

	// Status Output 
	// ================================================================================================
	void outputStatus(long countP, long countN, long countR) {
		cout << "Inf: " << left << setw(10) << countP << "    Non-inf: " << left << setw(10) << countN << "    Rec OR Inn: " << left << countR << endl;					   
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

	int getDay(){					// returns day 
		return day;
	}

	void dayOutput(){				// print function for arrangement
		 	printf("<  DAY  %d >   ", day) ;
	}
};	

// ================================================================================================
// Main Function 
// ================================================================================================
int main() {
	long pop_size; 	// population size
	long inn_size; 	// Number of Immune People   
	int off = 1;  	// Code Termination Switch; 

// Simulation Variables 
// ================================================================================================
	cout << "Enter Population: ";
	cin >> pop;
	cout << "Enter # of Intially Infected People: ";
	cin >> infect;
	cout << "Enter # of Innoculated People: "; 
	cin >> inn;
	cout << "Enter # of Days to Simulate: "; 
	cin >> days;
	cout << endl;
	cout << right << setw(41) << "< SIMULATION >" << endl;
	cout << endl;

	Population p1(pop);
	p1.setInitialCond(inn,infect); 
	p1.updatePopulation(days);

	cout << endl;
	cout << "Program Terminated" << endl; 
	cout << "Rick Sungsoo Kim, 2020" << endl;
	cout << "The University of Texas at Austin, Cockrell School of Engineering" << endl;
}