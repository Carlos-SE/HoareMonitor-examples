/*
En los monitores la funcionalidad de los elementos,
en este caso de los fumadores y del estanquero se declaran
como funciones externas al monitor.
Todas las funcionalidades que requieran del monitor serán
métodos públicos a los cuales podrán acceder por medio de un 
parámetro monitor  los elementos del problema

*/



#include <iostream>
#include <iomanip>
#include <random>
#include <thread>
#include "HoareMonitor.h"

using namespace std;
using namespace HM;


template <int min, int max > int aleatorio(){
	static default_random_engine generador((random_device())());
	static uniform_int_distribution <int> distribucion_uniforme(min, max);
	return distribucion_uniforme(generador);
}

class Estanco : public HoareMonitor{
private:

	CondVar fumadores[3], estanquero;
	int ingrediente;

public:

	Estanco();
	//void Estanquero();
	//void fumador(int ingrediente);
	int generar_ingrediente();
	void fumar(int f);

};


// Inicialización de losatributos
Estanco :: Estanco(){
	ingrediente = -1;
	for (unsigned i = 0; i < 3; i++){
		fumadores[i] = newCondVar();
	}
	estanquero = newCondVar();

}

/*void Estanco :: Estanquero(){
	int f;
	while (true){
		f = generar_ingrediente();

		fumadores[f].signal();

		cout << "El estanquero ha producido el ingrediente " << f << endl;
	}
}*/




/*void Estanco :: fumador(int ingrediente){
	while (true){
		fumadores[ingrediente].wait();
		int tiempo = aleatorio<0, 15>();
		this_thread::sleep_for(chrono::milliseconds(tiempo));
		
	}
}*/

int Estanco :: generar_ingrediente(){
	//estanquero.wait();
	int i =aleatorio<0,2>();
	this_thread::sleep_for(chrono::milliseconds(aleatorio<10,100>()));
	fumadores[i].signal();
	return i;
}
void Estanco :: fumar(int f){
	fumadores[f].wait();
	estanquero.signal();
}

//***************************************************************************

void estanquero(MRef<Estanco> monitor){
	int f;
	while (true){
		f = monitor->generar_ingrediente();

		cout << "El estanquero ha producido el ingrediente " << f << endl;
	}
}

void fumador(MRef<Estanco> monitor, int ingrediente){
	while (true){

		monitor ->fumar(ingrediente);
		cout << "El fumador " << ingrediente << " coge su tabaco y se pone a fumar" << endl;

		int tiempo = aleatorio<50, 100>();
		this_thread::sleep_for(chrono::milliseconds(tiempo));

		cout << "El fumador " << ingrediente << " ha fumado" << endl;
	}
}


//***************************************************************************

int main(){
	MRef<Estanco> monitor = Create<Estanco>();
	thread hebra_estanquero(estanquero, monitor);
	thread hebra_fumadores[3];
	for (unsigned i = 0; i < 3; i++){
		hebra_fumadores[i] = thread(fumador, monitor, i);
	}

	hebra_estanquero.join();
	for (unsigned i = 0; i < 3; i ++){
		hebra_fumadores[i].join();
	}
}