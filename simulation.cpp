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
#define INFECTION_RATE 0.1 													// 10% 
#define INTERACTION_MEAN 12  												// Interactions per Day (Mean, SD)
#define INTERACTION_SD 2
#define DORMANT_PERIOD 9  													// Virus Dormant Range 
#define RECOVERY_MEAN 14 														// Recovery Period (Mean, SD)
#define RECOVERY_SD 5 				
#define RANDOM_PERCENT 0.1													// weight of random encounters 	
#define CLOSE_PEOPLE 30															// Number of Close friends & Family 						

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

	// Initialize Population 
	// ================================================================================================
	void setPopulation(long size){	
		Person random;
		for (long i = 0; i < size; i++){
			people.push_back(random); 
		}
	}
	// Interactions 
	// ================================================================================================
	int setInteractionNum() {

		std::default_random_engine generator; 
		std::normal_distribution<float> distribution(INTERACTION_MEAN,INTERACTION_SD);

		return (int) distribution(generator);  						
	}

	// Recovery Period 
	// ================================================================================================
	int setRecoveryPd() {

		std::default_random_engine generator; 
		std::normal_distribution<float> distribution(RECOVERY_MEAN,RECOVERY_SD);

		return (int) distribution(generator);  						
	}

	// Status Output
	// ================================================================================================
	void outputStatus(long countP, long countN, long countR, long countI) {
		if (day < 10 ) {
			printf("<  DAY   %d >   ", day);
		} else {
			printf("<  DAY  %d >   ", day);
		}
		cout << "Inf: " << left << setw(10) << countP << "    Non-inf: " << left << setw(10) << countN << "    Self-isolation : " << left << setw(10) << countI << "    Rec OR Inn: " << left << countR << endl;					   
	}
 
	// Counting Current Status 
	// ================================================================================================
	auto countStatus() { 																	   // returns total # of infection 
		// Set as reduction 
		long countP = 0;			// total sickness
		long countN = 0; 		  // total potential patients 
		long countR = 0;			// total recovered / innoculated 
		long countI = 0; 			// total isolated 
		long sickTotal = 0;
		int status;
		#ifdef _OPENMP
		#pragma omp parallel for schedule(dynamic) reduction (+:countR,countN,countP,countI)
		for ( long i = 0; i < people.size(); i++) {
			status = people.at(i).getStatus();
			if ( status < 0 ) {
				countR++; 
			} else if ( status == 0 ) {
				countN++; 
			} else if ( status < DORMANT_PERIOD ) {
				countP++; 
			} else {
				countI++;
			}
		}
		#endif

		double isolationPercent = (float) countI / (float) people.size(); 
		int tMean = INTERACTION_MEAN;
		int tSD = INTERACTION_SD;
		
		if (isolationPercent > 0.05) {
			#undef	INTERACTION_MEAN 
			#undef INTERACTION_SD
			#define INTERACTION_MEAN  4
			#define  INTERACTION_SD  1
		} else {
			#undef	INTERACTION_MEAN 
			#undef INTERACTION_SD
			#define INTERACTION_MEAN (int) tMean * isolationPercent
			#define  INTERACTION_SD  (int) tSD * isolationPercent 
		}		
		struct result { long p; long n; long r; long i;};
		return result {countP, countN, countR, countI};
	}	
	// Finding Random Encounters 
	// ================================================================================================
	long randomEncounter(long id) {
		bool found = false; 
		long randomPerson = (long) rand() % people.size(); 	 					// Random Person 
		while (found == false) {
			if (randomPerson != id && people.at(randomPerson).getStatus() < DORMANT_PERIOD) { // Only folks who are not tested 
				found = true;
			} else {
				randomPerson = (long) rand() % people.size(); 	 					// Random Search 
			}
		}
		return randomPerson;
	}

	float chances() {
		return ((float) rand() / RAND_MAX); 		// Chances in Life
	}
	// Finding Known Encounters
	// ================================================================================================
	long knownEncounter(long id) { 
		int interactionSize = people.at(id).getInteraction();													// Interaction Size 
		long cfriend = id + (rand() %CLOSE_PEOPLE - CLOSE_PEOPLE/2);     // Relative Location of Friend in People to "me"
		int temp = cfriend;
		long result; 
		if ((cfriend) < 0 ) {
			cfriend = cfriend + people.size();
		} else if (cfriend >= people.size()) {
			cfriend = cfriend - people.size();
		}
		return cfriend; 
	}

	// Full Encounter Simulation
	// ================================================================================================
	vector<long> encounterSimulation(long id) {
		int interactionSize = people.at(id).getInteraction(); 						// # of interaction 
		int newP = (int) ceil((float) interactionSize * RANDOM_PERCENT); 	// # of new interaction
		vector<long> results;
		for (int i = 0; i < newP; i++) {		
			results.push_back(randomEncounter(id));													// Random Encounter
		} 
		for (int i = 0; i < interactionSize - newP; i++) {								// Known Encounter (Family, coworkers etc. )
			results.push_back(knownEncounter(id));
		}
		return results;
	}

	// ================================================================================================
	//  1st Person Daily Interaction Simulation In Dormant Times 
	// ================================================================================================
	void simulateDailyInteraction(long id){ 
		Person person = people.at(id);
		int status = person.getStatus();
		
		// Infected ====================================================================================
		if (status > 0) {
			status++; 
			if (status > DORMANT_PERIOD ) { 															// Self Isolation & Recovering 
				if (status > person.getRecoveryPd() + DORMANT_PERIOD) {	
					people.at(id).setStatus(-1);															// Person Recovered 
				} else {
					people.at(id).setStatus(status);
				}
			}	else {																							// Infected BUT not identified yet 
				Person rand; 
				int randId;
				vector<long> encounter = encounterSimulation(id);
				for (int i = 0; i < encounter.size(); i++) {
					randId = encounter[i];
					rand = people.at(randId);
					if (rand.getStatus() == 0 && chances() < INFECTION_RATE){
						people.at(randId).setStatus(1);
					}
					people.at(id).setStatus(status);
				} 
			}	
		// Uninfected ====================================================================================
		} else if (status == 0) {															
			bool infected = false; 
			Person rand;
			int randId;
			vector<long> encounter = encounterSimulation(id);
			for (int i = 0; i < encounter.size(); i++) {
				randId = encounter[i];
				rand = people.at(randId);
				if (infected == false) {
					if ((rand.getStatus() > 0 && rand.getStatus() < DORMANT_PERIOD) && chances() < INFECTION_RATE) {
						people.at(id).setStatus(1);
					}
				} else {
					if (rand.getStatus() == 0) {
						people.at(randId).setStatus(1);
					}
				}
			}
			if (infected == true) {
				people.at(id).setStatus(status+1);
			}
		}
	}	

	// Initial Conditions 
	// ================================================================================================  	

	void setInitialCond(long innoculate_num,long infection_num) {
		printf("Setting Initial Conditions \n");
		long final_num = infection_num + innoculate_num;
		
		for (long i = 0; i < final_num; i++ ) {
			if (i < infection_num) {
				people.at(i).setStatus(1);
			} else {
				people.at(i).setStatus(-2);
			}
		}
	}

	// Update Population 
	// ================================================================================================
	void updatePopulation(int days) {	// updates population based on interaction per day until no one's sick 
		long countP, countN, countR, countI;
		
		auto result = countStatus();
		countP = result.p;
		countN = result.n;
		countR = result.r;
		countI = result.i;
		outputStatus(countP,countN,countR,countI);

		std::ofstream outputFile; 
		outputFile.open("Disease_Simualation.csv");
		outputFile << "Total Population :," <<  people.size() << endl;
		outputFile << "Day, Total Sickness, Uninfected, Self-Isolation / Treatmentm, Recovered / Innoculated" << endl;
		
		bool off = false; 
		while (days > day && off == false) {	
			day++; 
			#ifdef _OPENMP
			#pragma omp parallel for schedule(runtime) 
			for (long i = 0; i < people.size(); i++) {
				people[i].setInteraction(setInteractionNum());	// Adjusting Interaction 
				simulateDailyInteraction(i);		// Simulating Daily Interaction 
			}
			#endif
			auto result = countStatus();
			countP = result.p;
			countN = result.n;
			countR = result.r;
			countI = result.i;
			outputFile <<  day << "," << countP << "," << countN << ","  << countI << "," << countR << endl;
			outputStatus(countP,countN,countR,countI);
		}
		outputFile.close();
		printf("Data saved to file!");
		if (countR == people.size()) {
			printf("Everyone has recovered");
			off = true; 
		}
	}	
};

double getTime() {
  struct timeval time; 
  gettimeofday(&time,NULL);
  return (double)time.tv_sec + (double)time.tv_usec * .000001; 
}

// ================================================================================================
// Main Function 
// ================================================================================================
int main() {
	#ifdef _OPENMP
	printf("There are %d processors available \n", omp_get_num_procs());
	omp_set_num_threads(8);
	#endif

	long pop_size = 2000000; 	// population size
	long inocuated_size = 1000; 	// Number of Immune People  
	long infect_size = 1; 
	int days = 90;
	double start, tInit, tSet, tSimulate;

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
	printf("Population: %llu    Infection: %llu    Inoculated: %llu \n", pop_size, infect_size, inocuated_size);

	start = getTime();
	Population p1(pop_size);
	tInit = getTime() - start; 
	
	start = getTime(); 
	p1.setInitialCond(inocuated_size,infect_size); 
	tSet = getTime() - start;
	
	start = getTime();
	p1.updatePopulation(days);
	tSimulate = getTime() - start;

	cout << endl;
	printf("Population Generation :		 %g seconds\n", tInit);
	printf("Initial Conditions Set: 	 %g seconds\n", tSet);
	printf("Simulation						:    %g seconds\n", tSimulate);

	cout << endl;
	cout << "Program Terminated" << endl; 
	cout << "Rick Sungsoo Kim, 2020" << endl;
	cout << "The University of Texas at Austin, Cockrell School of Engineering" << endl;
}