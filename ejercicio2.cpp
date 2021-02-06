/*
El cocinero no puede rellenar la olla hasta que esté
completamente vacía. Los comensales comerán mientras haya 
raciones en la olla y si no hay solicitarán al cocinero que
la rellene
*/


// Mejorar con un semáforo que de el turno de escritura en
// la terminal. Para obtener una representación clara

#include <iostream>
#include <thread>
#include <random>
#include "HoareMonitor.h"

using namespace std;
using namespace HM;


const int RACIONES_OLLA = 5;
const int COMENSALES = 3;

//**********************************************************************************

template <int min, int max > int aleatorio(){
	static default_random_engine generador((random_device())());
	static uniform_int_distribution <int> distribucion_uniforme(min, max);
	return distribucion_uniforme(generador);
}


void comer(int comensal){

	this_thread::sleep_for(chrono::milliseconds(aleatorio<50,100>()));

	cout << "El comensal " << comensal << " ha terminado de comer" << flush << endl;

}

void cocinar(){

	cout << "El cocinero empieza a cocinar..." << flush << endl;

	this_thread::sleep_for(chrono::milliseconds(aleatorio<10,100>()));

	cout << "El cocinero ha terminado de cocinar" << flush << endl;
}

//**********************************************************************************

class Restaurante : public HoareMonitor{
private:
	int olla;
	CondVar olla_vacia;
	CondVar comensales[COMENSALES];

public:

	Restaurante();
	void rellenarOlla();
	void servirse(int comensal);

};

Restaurante :: Restaurante(){
	olla = 0;
	olla_vacia = newCondVar();
	for (unsigned i = 0; i < COMENSALES; i++)
		comensales[i] = newCondVar();
}

void Restaurante :: rellenarOlla(){
	if (olla > 0){
		olla_vacia.wait();
	}


	olla = RACIONES_OLLA;
	for (unsigned i = 0; i < COMENSALES; i++)
		comensales[i].signal();

}

void Restaurante :: servirse(int comensal){
	if (olla < 1){
		comensales[comensal].wait();
	}

	cout << "Comensal " << comensal << " comienza se sirve" << flush << endl;

	olla--;

	if (olla == 0){
		olla_vacia.signal();
	}

}


//***********************************************************************************

void comensal(MRef<Restaurante> monitor, int comensal){

	while (true){
		monitor->servirse(comensal);
		comer(comensal);
	}

}

void cocinero(MRef<Restaurante> monitor){

	while (true){
		cocinar();
		monitor->rellenarOlla();
	}
}

//***********************************************************************************

int main(){
	MRef<Restaurante> monitor = Create<Restaurante>();
	thread hebra_cocinero(cocinero, monitor);
	thread hebra_comensales[COMENSALES];
	for (unsigned i = 0; i < COMENSALES; i++)
		hebra_comensales[i] = thread(comensal, monitor, i);

	hebra_cocinero.join();
	for (unsigned i = 0; i < COMENSALES; i++)
		hebra_comensales[i].join();
}