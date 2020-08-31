#include "hash.h"
#include "hash_iterador.h"
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"
#define ERROR -1
#define MAX_CLAVES 20

//usar listas
struct hash{
	lista_t** lista;
	size_t tamanio;
	size_t ocupado;
	hash_destruir_dato_t destruir;
};

typedef struct dato{
	char* clave;
	void* elemento;
}dato_t;




/*Busca en la lista enlazada la clave y devuelve la posicion del elemento*/
size_t buscar_posicion_clave(lista_t* lista, const char* clave){
	if(!lista || !clave) return 0;
	lista_iterador_t* iterador_lista = lista_iterador_crear(lista);
	if(!iterador_lista){
		printf("No se creo el iterador\n");
		return 0;
	} 
	size_t posicion_buscada=0;
	bool encontro = false;
	
	while(lista_iterador_tiene_siguiente(iterador_lista) && !encontro){
		dato_t* dato_lista = (dato_t*)lista_iterador_siguiente(iterador_lista);
		if(strcmp(dato_lista->clave, clave)==0){
			encontro =true;	
		}
		else posicion_buscada++;
	}
	lista_iterador_destruir(iterador_lista);
	return posicion_buscada;
}

/*Verifica si una clave esta repetida en una lista, devuelve true si esta repetida
  False en caso contrario*/
bool clave_repetida(lista_t* lista,const char* clave){
	if(!lista) return false;
	bool repetido=false;

	lista_iterador_t* iterador_lista = lista_iterador_crear(lista);	
	if(!iterador_lista) return false;
	while(lista_iterador_tiene_siguiente(iterador_lista) && !repetido){
		dato_t* dato_lista = (dato_t*)lista_iterador_siguiente(iterador_lista);
		if(strcmp(dato_lista->clave,clave)==0){
			repetido =true;
		}
	}
	lista_iterador_destruir(iterador_lista);
	return repetido;
}

/*Devuelve la posicion del hash*/
size_t hashing(hash_t* hash, const char* clave){
	size_t un_numero = 3;
	size_t posicion;
	for(posicion=0; posicion < strlen(clave); posicion++){
		posicion = un_numero*(posicion+(size_t)clave);
	}
	return (posicion)%(hash->tamanio);
}


/*Pide memoria para un dato_t y lo devuelve*/
dato_t* crear_dato(const char* clave, void* elemento){
	if(!clave) return NULL;
	dato_t* dato_nuevo = malloc(sizeof(dato_t));
	if(!dato_nuevo) return NULL;
	dato_nuevo->clave = (char*)malloc(sizeof(clave)+1);
	strcpy(dato_nuevo->clave, clave);
	dato_nuevo->elemento = elemento;
	return dato_nuevo;
}

/*Libera la memoria de un dato_t , invoca al destruir de hash*/
void eliminar_dato(hash_t* hash,dato_t* dato){
	if(hash->destruir)	hash->destruir(dato->elemento);
	free(dato->clave);
	free(dato);
}

/*
 * Crea el hash reservando la memoria necesaria para el.
 * Destruir_elemento es un destructor que se utilizará para liberar
 * los elementos que se eliminen del hash.  Capacidad indica la
 * capacidad inicial con la que se crea el hash. La capacidad inicial
 * no puede ser menor a 3. Si se solicita una capacidad menor, el hash
 * se creará con una capacidad de 3.
 *
 * Devuelve un puntero al hash creado o NULL en caso de no poder
 * crearlo.
 */
hash_t* hash_crear(hash_destruir_dato_t destruir_elemento, size_t capacidad){
	hash_t* hash = malloc(sizeof(hash_t));
	if(!hash) return NULL;
	hash->ocupado=0;
	if(capacidad<3) {
		hash->lista = calloc(3,sizeof(lista_t*));
		hash->tamanio = 3;
	}
	else{
		hash->lista = calloc(capacidad,sizeof(lista_t*));
		hash->tamanio = capacidad;
	}
	if(!hash->lista){
		free(hash);
		return NULL;	
	}
	hash->destruir = destruir_elemento;
	for(size_t i=0;i<hash->tamanio;i++){
		hash->lista[i]=NULL;
	}
	return hash;
}

/*
 * Inserta un elemento en el hash asociado a la clave dada.
 *
 * Nota para los alumnos: Recordar que si insertar un elemento provoca
 * que el factor de carga exceda cierto umbral, se debe ajustar el
 * tamaño de la tabla para evitar futuras colisiones.
 *
 * Devuelve 0 si pudo guardarlo o -1 si no pudo.
 */
int hash_insertar(hash_t* hash, const char* clave, void* elemento){
	if(!hash || !clave) return ERROR;
	dato_t* nuevo_dato = crear_dato(clave,elemento);
	if(!nuevo_dato) return -1;

	size_t posicion = hashing(hash, clave);
	
	if(!hash->lista[posicion]){
		lista_t* lista_nueva = lista_crear();
		if(!lista_nueva) return -1;
		hash->lista[posicion] = lista_nueva;
	}
	if(clave_repetida(hash->lista[posicion], clave)){
		/*Elimino el dato repetido ya almacenado*/
		size_t posicion_lista = buscar_posicion_clave(hash->lista[posicion], clave); 
		dato_t* dato_buscado = (dato_t*)lista_elemento_en_posicion(hash->lista[posicion], posicion_lista);
		if(!dato_buscado) return -1;
			eliminar_dato(hash, dato_buscado);
			lista_borrar_de_posicion(hash->lista[posicion],posicion_lista);
			hash->ocupado--;

	} 
	lista_insertar(hash->lista[posicion], nuevo_dato);
	hash->ocupado++;
	return 0;
}

/*
 * Quita un elemento del hash e invoca la funcion destructora
 * pasandole dicho elemento.
 * Devuelve 0 si pudo eliminar el elemento o -1 si no pudo.
 */
int hash_quitar(hash_t* hash, const char* clave){
	if(!hash) return -1;
	size_t posicion = hashing(hash, clave);
	
	if(!hash->lista[posicion] || lista_vacia(hash->lista[posicion])) return -1;
	
	size_t posicion_lista = buscar_posicion_clave(hash->lista[posicion], clave); 
	dato_t* dato_buscado = (dato_t*)lista_elemento_en_posicion(hash->lista[posicion], posicion_lista);
	if(!dato_buscado) return -1;
	eliminar_dato(hash ,dato_buscado);
	lista_borrar_de_posicion(hash->lista[posicion],posicion_lista);
	hash->ocupado--;
	return 0;
}

/*
 * Devuelve un elemento del hash con la clave dada o NULL si dicho
 * elemento no existe (o en caso de error).
 */
void* hash_obtener(hash_t* hash, const char* clave){
	if(!hash) return NULL;
	size_t posicion = hashing(hash,clave);
	/*Buscar en las listas enlazadas*/
	if(!hash->lista[posicion]){
		return NULL;
	}
	size_t posicion_lista = buscar_posicion_clave(hash->lista[posicion], clave);
	dato_t* dato_buscado = (dato_t*)lista_elemento_en_posicion(hash->lista[posicion], posicion_lista);
	if(!dato_buscado){
		return NULL;
	}
	return dato_buscado->elemento;
}

/*
 * Devuelve true si el hash contiene un elemento almacenado con la
 * clave dada o false en caso contrario (o en caso de error).
 */
bool hash_contiene(hash_t* hash, const char* clave){
	if(!hash){
		return false;
	}
	size_t posicion = hashing(hash, clave);
	if(!hash->lista[posicion]) {
		return false;
	}
	bool dato_buscado = clave_repetida(hash->lista[posicion], clave);
	if(!dato_buscado) return false;
	return true;
}

/*
 * Devuelve la cantidad de elementos almacenados en el hash o 0 en
 * caso de error.
 */
size_t hash_cantidad(hash_t* hash){
	if(!hash) return 0;
	return hash->ocupado;
}

void hash_destruir(hash_t* hash){
	if(!hash) return;
	for(size_t i=0 ; i< hash->tamanio;i++){
		lista_iterador_t* iterador_lista = lista_iterador_crear(hash->lista[i]);
		if(iterador_lista!=NULL){
			while(lista_iterador_tiene_siguiente(iterador_lista)){
				dato_t* dato = (dato_t*)lista_iterador_siguiente(iterador_lista);
				eliminar_dato(hash, dato);
			}
			lista_destruir(hash->lista[i]);
			lista_iterador_destruir(iterador_lista);
		}	
	}
	free(hash->lista);
	free(hash);
}

/*
 * Recorre cada una de las claves almacenadas en la tabla de hash e
 * invoca a la función funcion, pasandole como parámetros el hash, la
 * clave en cuestión y el puntero auxiliar.
 *
 * Mientras que queden mas claves o la funcion retorne false, la
 * iteración continúa. Cuando no quedan mas claves o la función
 * devuelve true, la iteración se corta y la función principal
 * retorna.
 *
 * Devuelve la cantidad de claves totales iteradas (la cantidad de
 * veces que fue invocada la función) o 0 en caso de error.
 *
 */
size_t hash_con_cada_clave(hash_t* hash, bool (*funcion)(hash_t* hash, const char* clave, void* aux), void* aux){
	if(!hash) return 0;
	size_t contador_listas;
	size_t contador_claves=0;
	bool valido = false;
	for(contador_listas=0 ; contador_listas<hash->tamanio && !valido; contador_listas++){
		lista_iterador_t* iterador_lista = lista_iterador_crear(hash->lista[contador_listas]);
		while(lista_iterador_tiene_siguiente(iterador_lista) && !valido){
				dato_t* dato = (dato_t*)lista_iterador_siguiente(iterador_lista);
				if(!dato){
					continue;
				}
				else{
					valido = funcion(hash, dato->clave, aux);
				}
				contador_claves++;
		}
		lista_iterador_destruir(iterador_lista);
	}
	return contador_claves;
}

//compilar: gcc *.c -o hash -g -std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O0
//valgrind: valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./hash

/* Iterador externo para el HASH */
struct hash_iter{
	lista_t** listas;
	size_t posicion;
	size_t tamanio_hash;
	lista_t* lista_actual;
	size_t elementos_lista;
};

/*
 * Crea un iterador de claves para el hash reservando la memoria
 * necesaria para el mismo. El iterador creado es válido desde su
 * creación hasta que se modifique la tabla de hash (insertando o
 * removiendo elementos).
 *
 * Devuelve el puntero al iterador creado o NULL en caso de error.
 */
hash_iterador_t* hash_iterador_crear(hash_t* hash){
	if(!hash) return NULL;
	hash_iterador_t* iterador_hash = malloc(sizeof(hash_iterador_t));
	if(!iterador_hash) return NULL;
	iterador_hash->listas = hash->lista;
	iterador_hash->posicion = 0;
	iterador_hash->tamanio_hash = hash->tamanio;
	iterador_hash->lista_actual = NULL;
	iterador_hash->elementos_lista=0;
	return iterador_hash;
}	


/*
 * Devuelve la próxima clave almacenada en el hash y avanza el iterador.
 * Devuelve la clave o NULL si no habia mas.
 */
const char* hash_iterador_siguiente(hash_iterador_t* iterador){
	if(!iterador || !iterador->listas) return NULL;
	iterador->lista_actual = iterador->listas[iterador->posicion];

	if(lista_vacia(iterador->lista_actual)){
		iterador->posicion++;
		return NULL;
	}
	if(iterador->elementos_lista != lista_elementos(iterador->lista_actual)){
		dato_t* dato = lista_elemento_en_posicion(iterador->lista_actual, iterador->elementos_lista);
		iterador->elementos_lista++;
		return dato->clave;
	}
	iterador->posicion++;
	iterador->elementos_lista=0;
	return NULL;
}

/*
 * Devuelve true si quedan claves por recorrer o false en caso
 * contrario o de error.
 */
bool hash_iterador_tiene_siguiente(hash_iterador_t* iterador){
	if(!iterador) return false;
	if(iterador->posicion == iterador->tamanio_hash){
		return false;
	}
	return true;
}

/*
 * Destruye el iterador del hash.
 */
void hash_iterador_destruir(hash_iterador_t* iterador){
	if(!iterador) return;
	free(iterador);
}