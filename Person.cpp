#include <iostream>
#include <cstdlib>
#include <random>
#include <iomanip>
#include <cmath>
using namespace std; 

// Macros
#define INFECTION_RATE 0.5 													// 50% 
#define INTERACTION_NUM {12,3}  										// Interactions per Day (Mean, SD)
#define DORMANT_PERIOD 14  													// Days Staying Dormant  
#define RECOVERY_PERIOD {14.0,2.0} 									// Recovery Period (Mean, SD)
#define CHANCES (float) rand() / (float)RAND_MAX 		// Chances in Life

// ================================================================================================
// Person Class 
// ================================================================================================
class Person {
	protected: 
	int status; 							// (Infected : >= 1, Recovered : -1, Susceptible : 0,  Innoculated: -2)
	int interaction;					// Number of interactions per day 
	int recoveryDate;
		
	// Constructors 
	// ================================================================================================
	public:
	void setInteractionNum() {
		std::default_random_engine generator; 
		std::normal_distribution<double> distribution(12,3);

		interaction = (int) distribution(generator);  							// Norm-Distribution Value is Returned 
	}

	void setRecoveryPd() {
		if (status > 0) {
			std::default_random_engine generator; 
			std::normal_distribution<double> distribution(14,2);

			recoveryDate = (int) distribution(generator);  						// Norm-Distribution Value is Returned 
		} else {
			recoveryDate = -1;
		}
	}

	Person(){  
		status = 0; 
		setInteractionNum(); 
		setRecoveryPd();
	}
	Person(int s){  
		status = s; 
		setInteractionNum();
		setRecoveryPd();
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

	void infected() {
		if (status == 0) {
			status++;
		}
	}
};
