// compilación: g++ -std=c++11 -pthread -o nombre <archivo>.cpp HoareMonitor.cpp Semaphore.cpp


#include <iostream>
#include <thread>
#include <random>
#include "HoareMonitor.h"

using namespace std;
using namespace HM;


template <int min, int max > int aleatorio(){
	static default_random_engine generador((random_device())());
	static uniform_int_distribution <int> distribucion_uniforme(min, max);
	return distribucion_uniforme(generador);
}

class Filo : public HoareMonitor{
private:
static const int num_filosofos = 5;
bool filo_cubiertos[num_filosofos];

public:

	Filo();
	void cogerCubiertos(int id);
	void soltarCubiertos(int id);

};

/////////////////////////////////////////////////////////////////////////////////////////////

Filo::Filo(){
	for (unsigned i = 0; i < num_filosofos; i++){
		filo_cubiertos[i] = false;
	}
}
void Filo::cogerCubiertos(int id){
	cout << "El filo." << id << " coge los cubiertos" << endl << flush;
	filo_cubiertos[id] = true;
}
void Filo::soltarCubiertos(int id){
	cout << "El filo. " << id << " suelta los cubiertos" << endl << flush;
	filo_cubiertos[id] = false;
}


//////////////////////////////////////////////////////////////////////////////////////////////
void comer(int id){

	int tiempo = (aleatorio<500,1000>());
	cout << "El filo. " << id << " comerá durante " << tiempo << " milisec." << endl << flush;
	this_thread::sleep_for(chrono::milliseconds(tiempo));
	cout << "				Filo " << id << " ha terminado de comer." << endl << flush;

}


void descansar(int id){

	int tiempo = (aleatorio<750,1500>());
	cout << "El filo. " << id << " descansará durante " << tiempo << " milisec." << endl << flush;
	this_thread::sleep_for(chrono::milliseconds(tiempo));
	cout << "			Filo " << id << " ha terminado de descansar." << endl << flush;

}

//////////////////////////////////////////////////////////////////////////////////////////////


void funcion_filo(MRef<Filo> monitor, int id){
	while (true){

		monitor->cogerCubiertos(id);
		comer(id);
		monitor->soltarCubiertos(id);
		descansar(id);

	}
}

///////////////////////////////////////////////////////////////////////////////////////////

int main(){
	
	MRef<Filo> monitor = Create<Filo>();
	thread filosofos[5];

	for (unsigned i = 0; i < 5; i++){
		filosofos[i] = thread(funcion_filo, monitor, i);
	}

	for (unsigned i = 0; i < 5; i++){
		filosofos[i].join();
	}

}