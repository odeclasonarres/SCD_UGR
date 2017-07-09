// *****************************************************************************
//
// Prácticas de SCD. Práctica 1.
// Plantilla de código para el ejercicio de los fumadores
//
// *****************************************************************************

#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>      // incluye "time(....)"
#include <unistd.h>    // incluye "usleep(...)"
#include <stdlib.h>    // incluye "rand(...)" y "srand"

using namespace std ;

// Constantes y variables
const int num=3;
sem_t puede_producir;
sem_t puede_fumar[num];


// ---------------------------------------------------------------------
// introduce un retraso aleatorio de duración comprendida entre
// 'smin' y 'smax' (dados en segundos)
void retraso_aleatorio( const float smin, const float smax ){
  static bool primera = true ;
  if ( primera ){        // si es la primera vez:
      srand(time(NULL)); //   inicializar la semilla del generador
      primera = false ;  //   no repetir la inicialización
  }
  // calcular un número de segundos aleatorio, entre {\ttbf smin} y {\ttbf smax}
  const float tsec = smin+(smax-smin)*((float)random()/(float)RAND_MAX);
  // dormir la hebra (los segundos se pasan a microsegundos, multiplicándos por 1 millón)
  usleep( (useconds_t) (tsec*1000000.0)  );
}

// ----------------------------------------------------------------------------
// función que simula la acción de fumar, como un retardo aleatorio de la hebra.
// recibe como parámetro el numero de fumador
// el tiempo que tarda en fumar está entre dos y ocho décimas de segundo.
void* fumar(void * arg_ptr){
  unsigned long arg_ent = (unsigned long) arg_ptr;	//Convertir puntero a entero
  while(true){
    sem_wait(&puede_fumar[arg_ent]);
    cout << "Fumador número " << arg_ent << ": comienza a fumar." << endl << flush ;
    sem_post(&puede_producir);
    retraso_aleatorio( 0.2, 0.8 );
    cout << "Fumador número " << arg_ent << ": termina de fumar." << endl << flush ;
    cout << "Fumador " << arg_ent << " esperando:";
    if (arg_ent == 0){
			cout << "cerillas.\n";
		}
		else{
			if(arg_ent == 1){
				cout << "tabaco.\n";
			}
			else{
				cout << "papel.\n";
			}
		}
  }
}

// ----------------------------------------------------------------------------
// función estanquero
void * estanquero(void * i){
  while(true){
    cout << "El estanquero esta esperando." << endl;
    sem_wait(&puede_producir);
    int ingrediente=rand()%num;
    cout << "Produzco: " << ingrediente << endl;
    sem_post(&puede_fumar[ingrediente]);
  }
}

// ----------------------------------------------------------------------------
//MAIN
int main(){
  srand( time(NULL) ); // inicializa semilla aleatoria para selección aleatoria de fumador

  //Se declaran las hebras
  pthread_t hebra_estanquero;
  pthread_t hebra_fumador[num];

  //Se inicializan los semáforos
  sem_init( &puede_producir, 0, 1 );
  for (int i = 0; i < num; i++) {
    sem_init( &puede_fumar[i], 0, 0 );
  }

  //Se crean las hebras y asocian a las funciones a desarrollar
  pthread_create(&hebra_estanquero, NULL, estanquero, NULL);
  for(int i=0; i<num; i++){
    void * arg_ptr=(void *) i;//Convertir entero a puntero
    pthread_create(&hebra_fumador[i], NULL, fumar, arg_ptr);
  }

  //
  pthread_join(hebra_estanquero, NULL);
  for (int i = 0; i < num; i++) {
    pthread_join(hebra_fumador[i], NULL);
  }

  //Se destruyen los semaforos
  sem_destroy(&puede_producir);
  sem_destroy(&puede_fumar[num]);

  return 0 ;
}
