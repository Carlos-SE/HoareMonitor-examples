// no funciona nada bien XD

#include <iostream>
#include <thread>
#include "HoareMonitor.h"

using namespace std;
using namespace HM;

/*

Ejercicio de prod-cons con monitores

tenemos un buffer de tamaño constante y una pareja de productor y consumidor

tratar con el buffer como FIFO para que cumpla con la vivacidad.


*/

const int TAM = 5;


class Buffer : public HoareMonitor{

private:

	int buffer[TAM];
	int ultimo_escrito;
	int ultimo_leido;
	CondVar leido, escrito;

public:

	Buffer();
	int consumir();
	void add(int valor);

};

Buffer :: Buffer(){

	ultimo_escrito = 4;
	ultimo_leido = -1;
	leido = newCondVar();
	escrito = newCondVar();

	for (unsigned i = 0; i < TAM; i++){
		buffer[i] = i;
	}
	

}

int Buffer :: consumir(){

	if (ultimo_escrito != TAM-1){
		escrito.wait();
	}

	ultimo_leido = (ultimo_leido + 1) % TAM;
	int valor = buffer[ultimo_leido]; 

	if (ultimo_leido == TAM-1){
			leido.signal();
	}

	return valor;

}

void Buffer :: add(int valor){

	if (ultimo_leido != TAM-1){
		leido.wait();
	}

	ultimo_escrito = (ultimo_escrito + 1) % TAM;
	buffer[ultimo_escrito] = valor;

	escrito.signal();

}

//*****************************************************************************

void consumidor(MRef<Buffer> monitor){

	int valor;

	while (true){
		valor = monitor->consumir();
		cout << "Consumidor: " << valor << flush << endl;
	}

}

void productor(MRef<Buffer> monitor){

	int valor = 0;

	while (true){
		
		cout << "Productor: " << valor << flush << endl;
		monitor->add(valor);
		valor++;

	}

}

//*****************************************************************************

int main(){

	MRef<Buffer> monitor = Create<Buffer>();
	
	thread hebra_consumidor(consumidor, monitor);
	thread hebra_productor(productor, monitor);

	hebra_consumidor.join();
	hebra_productor.join();

}