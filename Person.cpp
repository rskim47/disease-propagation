#include <iostream>
#include <cstdlib>
#include <random>
#include <iomanip>
#include <cmath>
using namespace std; 

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
	Person(){  
		status = 0; 
		interaction = 12;  
		recoveryDate = 14;
	}
	Person(int s, int i, int r ){  
		status = s; 
		interaction = i;  
		recoveryDate = r;

	}
	
	// Set & Get
	// ================================================================================================ 
	int getStatus(){ 	
		return status;
	}
	
	void setStatus(int s){  
		status = s; 
	}

	void setInteraction(int i) {
		interaction = i;
	}

	int getInteraction() { 
		return interaction; 
	}

	void setRecoveryPd(int r) {
		recoveryDate = r; 
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
