// *****************************************************************************
//
// Prácticas de SCD. Práctica 1.
// Plantilla de código para el ejercicio del productor-consumidor con
// buffer intermedio.
//
// *****************************************************************************
#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // necesario para {\ttbf usleep()}
#include <stdlib.h> // necesario para {\ttbf random()}, {\ttbf srandom()}
#include <time.h>   // necesario para {\ttbf time()}

using namespace std ;

// ---------------------------------------------------------------------
// constantes configurables:
const unsigned
  num_items  = 40 ,    // numero total de items que se producen o consumen
  tam_vector = 10 ;    // tamaño del vector, debe ser menor que el número de items

// variables
int buffer[tam_vector];
int primera_libre=0;
int primera_ocupada=0;

sem_t puede_escribir, puede_leer, mutex;

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

// ---------------------------------------------------------------------
// función que simula la producción de un dato
unsigned producir_dato(){
  static int contador = 0 ;
  contador = contador + 1 ;
  retraso_aleatorio( 0.1, 0.5 );
  cout << "Productor :      dato producido: " << contador << endl << flush ;
  return contador ;
}

// ---------------------------------------------------------------------
// función que simula la consumición de un dato
void consumir_dato( int dato ){
   retraso_aleatorio( 0.1, 1.5 );
   cout << "Consumidor:      dato consumido: " << dato << endl << flush ;
}

// ---------------------------------------------------------------------
// función que ejecuta la hebra del productor
void * funcion_productor( void * ){
  for( unsigned i = 0 ; i < num_items ; i++ ){
    int dato = producir_dato() ;
    sem_wait(&puede_escribir);
    buffer[primera_libre]=dato;
    primera_libre=(primera_libre+1)%tam_vector;
    cout << "Productor: dato insertado: " << dato << endl << flush ;
    sem_post(&puede_leer);
  }
  return NULL ;
}

// ---------------------------------------------------------------------
// función que ejecuta la hebra del consumidor
void * funcion_consumidor( void * ){
  for( unsigned i = 0 ; i < num_items ; i++ ){
    int dato=0;
    sem_wait(&puede_leer);
    dato=buffer[primera_ocupada];
    primera_ocupada=(primera_ocupada+1)%tam_vector;
    consumir_dato(dato);
    sem_wait(&mutex);
    cout << "Consumidor: dato extraído : " << dato << endl << flush ;
    sem_post(&mutex);
    sem_post(&puede_escribir);
  }
  return NULL ;
}

//----------------------------------------------------------------------
//MAIN
int main(){

  //se crean las hebras
  pthread_t lectora, escritora;

  //se inicializan los semáforos
  sem_init(&mutex,0,1);
  sem_init(&puede_leer,0,0);
  sem_init(&puede_escribir,0,tam_vector);

  //se asocian las hebras con la función a desarrollar
  pthread_create(&lectora,NULL,funcion_consumidor,NULL);
  pthread_create(&escritora,NULL,funcion_productor,NULL);

  pthread_join(lectora, NULL);
  pthread_join(escritora, NULL);

  //se destruyen los semáforos
  sem_destroy(&puede_escribir);
  sem_destroy(&puede_leer);
  sem_destroy(&mutex);

  //se devuelve el valor "fin" y se acaba la ejecución
  std::cout << '\n'<<"fin" <<endl ;
  return 0 ;
}
