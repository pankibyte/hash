#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define ERROR -1;
#define EXITO 0
#define SE_PUDO_INSERTAR true
typedef struct nodo{
	void* valor;
	struct nodo* siguiente;
}nodo_t;

struct lista{
	nodo_t* primero;
	nodo_t* ultimo;
	size_t largo;
};
struct lista_iterador{
	nodo_t* nodo_actual;
	nodo_t* nodo_anterior;
};

/*Crea y reserva memoria para un nodo con un dato.
  Devuelve el nodo con su elemento, si no devuelve NULL
 */
nodo_t* crear_nodo(void* elemento_insertar){
	nodo_t* nuevo_nodo = malloc(sizeof(nodo_t));
	if(!nuevo_nodo) return NULL;
	nuevo_nodo->valor = elemento_insertar;
	nuevo_nodo->siguiente = NULL;
	return nuevo_nodo;
}
/*
 * Crea la lista reservando la memoria necesaria.
 * Devuelve un puntero a la lista creada o NULL en caso de error.
 */
lista_t* lista_crear(){
	lista_t* nueva_lista = malloc(sizeof(lista_t));
	if(!nueva_lista) return NULL;
	nueva_lista->primero = NULL;
	nueva_lista->ultimo = NULL;
	nueva_lista->largo = 0;
	return nueva_lista;
}

/*
 * Inserta un elemento al final de la lista.
 * Devuelve 0 si pudo insertar o -1 si no pudo.
 */
int lista_insertar(lista_t* lista, void* elemento){
	if(!lista) return ERROR;	
	nodo_t* nodo = crear_nodo(elemento);
	if(!nodo) return ERROR;
	if(lista_vacia(lista)){
		lista->primero = nodo;
		lista->ultimo = nodo;
	}
	else{	
		lista->ultimo->siguiente = nodo;
		lista->ultimo = nodo;
	}
	lista->largo++;
	return EXITO;
}
/*Inserta un nodo en una posicion mayor a 0
 Pre condicion el nodo existe y la lista también*/
void insertar_nodo(lista_t* lista, nodo_t* nodo_insertar, size_t posicion){
	nodo_t* nodo_actual = lista->primero;
	for(size_t i=0; i<posicion-1; i++){
		nodo_actual = nodo_actual->siguiente;
	}
	nodo_insertar->siguiente = nodo_actual->siguiente;
	nodo_actual->siguiente= nodo_insertar;
}
/*
 * Inserta un elemento en la posicion indicada, donde 0 es insertar
 * como primer elemento y 1 es insertar luego del primer elemento.  
 * En caso de no existir la posicion indicada, lo inserta al final.
 * Devuelve 0 si pudo insertar o -1 si no pudo.
 */
int lista_insertar_en_posicion(lista_t* lista, void* elemento, size_t posicion){
	if(!lista) return ERROR;
	if(posicion >= lista->largo){
		return lista_insertar(lista, elemento);	
	} 
	nodo_t* nodo_nuevo = crear_nodo(elemento);
	if(!nodo_nuevo) return ERROR;
	if(posicion==0){
		nodo_nuevo->siguiente = lista->primero;
		lista->primero = nodo_nuevo;		
	}
	else if(posicion>0 && posicion<lista->largo){
		insertar_nodo(lista, nodo_nuevo, posicion);
	}
	lista->largo++;
	return EXITO;
}

/*
 * Quita de la lista el elemento que se encuentra en la ultima posición.
 * Devuelve 0 si pudo eliminar o -1 si no pudo.
 */
int lista_borrar(lista_t* lista){
	if(!lista || lista_vacia(lista)) return ERROR;
	nodo_t* nodo=lista->primero;
	for(size_t i=0;i<lista->largo-2;i++){
		if(!nodo->siguiente){
			free(nodo);
			lista->primero = NULL;
			lista->ultimo = NULL;
			lista->largo--;
			return EXITO;
		}
		else
			nodo = nodo->siguiente;
	}
	nodo_t* nodo_borrar;
	nodo_borrar = nodo->siguiente;
	nodo->siguiente = NULL;
	free(nodo_borrar);
	lista->ultimo = nodo;
	lista->largo--;
	return EXITO;
}

/*
 * Quita de la lista el elemento que se encuentra en la posición
 * indicada, donde 0 es el primer elemento.  
 * En caso de no existir esa posición se intentará borrar el último
 * elemento.  
 * Devuelve 0 si pudo eliminar o -1 si no pudo.
 */
int lista_borrar_de_posicion(lista_t* lista, size_t posicion){
	if(!lista || lista_vacia(lista)) return ERROR;
	if(posicion>=lista->largo-1){
		return lista_borrar(lista);
	}
	nodo_t* nodo = lista->primero;
	if(posicion==0){
		lista->primero = lista->primero->siguiente;
		free(nodo);
	}
	else{
		for(size_t i=0;i<posicion-1;i++){
			nodo = nodo->siguiente;
		}
		nodo_t* nodo_borrar = nodo->siguiente;
		nodo->siguiente = nodo_borrar->siguiente;
		free(nodo_borrar);
	}
	lista->largo--;
	return EXITO;
}

/*
 * Devuelve el elemento en la posicion indicada, donde 0 es el primer
 * elemento.
 *
 * Si no existe dicha posicion devuelve NULL.
 */
void* lista_elemento_en_posicion(lista_t* lista, size_t posicion){
	if(!lista || posicion>=lista->largo) return NULL;
	nodo_t* actual = lista->primero;
	void* elemento_para_devolver;
	for(size_t i=0;i<posicion;i++){
		actual= actual->siguiente;
		
	}
	elemento_para_devolver = actual->valor;
	return elemento_para_devolver;
}
/* 
 * Devuelve el último elemento de la lista o NULL si la lista se
 * encuentra vacía.
 */
void* lista_ultimo(lista_t* lista){
	if(!lista || lista_vacia(lista)) return NULL;
	return lista->ultimo->valor;
}

/* 
 * Devuelve true si la lista está vacía o false en caso contrario.
 */
bool lista_vacia(lista_t* lista){
	if(!lista) return true;
	return lista->largo == 0;
}

/*
 * Devuelve la cantidad de elementos almacenados en la lista.
 */
size_t lista_elementos(lista_t* lista){
	if(!lista) return 0;
	return lista->largo;
}

/* 
 * Apila un elemento.
 * Devuelve 0 si pudo o -1 en caso contrario.
 */
int lista_apilar(lista_t* lista, void* elemento){
	if(!lista) return ERROR;
	nodo_t* nodo_pila = crear_nodo(elemento);
	if(!nodo_pila) return ERROR;
	if(lista_vacia(lista)){
		lista->primero = nodo_pila;
		lista->ultimo = nodo_pila;
	}
	else{
		lista->ultimo->siguiente = nodo_pila;
		lista->ultimo = nodo_pila;
	}
	lista->largo++;
	return EXITO;
}
/* 
 * Desapila un elemento.
 * Devuelve 0 si pudo desapilar o -1 si no pudo.
 */
int lista_desapilar(lista_t* lista){
	if(!lista || lista_vacia(lista)) return ERROR;
	nodo_t* nodo = lista->primero;
	nodo_t* borrar;
	for(size_t i=0;i<lista->largo-2;i++){
		if(!nodo->siguiente){
			free(nodo);
			lista->ultimo = NULL;
			lista->primero = NULL;
			lista->largo--;
			return EXITO;
		}
		nodo = nodo->siguiente;	
	}	
	borrar = nodo->siguiente;
	nodo->siguiente = NULL;
	free(borrar);
	lista->ultimo = nodo;
	lista->largo--;
	return EXITO;
}

/*
 * Devuelve el elemento en el tope de la pila o NULL
 * en caso de estar vacía.
 */
void* lista_tope(lista_t* lista){
	if(!lista || lista_vacia(lista)) return NULL;
	return lista->ultimo->valor;
}

/* 
 * Encola un elemento.
 * Devuelve 0 si pudo encolar o -1 si no pudo.
 */
int lista_encolar(lista_t* lista, void* elemento){
	return lista_insertar(lista, elemento);
}

/* 
 * Desencola un elemento.
 * Devuelve 0 si pudo desencolar o -1 si no pudo.
 */
int lista_desencolar(lista_t* lista){
	if(!lista) return ERROR;
	nodo_t* nodo_borrar = lista->primero;
	lista->primero = lista->primero->siguiente;
	free(nodo_borrar);
	lista->largo--;
	return EXITO;
}

/*
 * Devuelve el primer elemento de la cola o NULL en caso de estar
 * vacía.
 */
void* lista_primero(lista_t* lista){
	if(!lista || lista_vacia(lista)) return NULL;
	return lista->primero->valor;
}

/*
 * Libera la memoria reservada por la lista.
 */
void lista_destruir(lista_t* lista){
	while(!lista_vacia(lista)){
		lista_borrar(lista);
	}
	free(lista);
}
/*
 * Crea un iterador para una lista. El iterador creado es válido desde
 * el momento de su creación hasta que no haya mas elementos por
 * recorrer o se modifique la lista iterada (agregando o quitando
 * elementos de la lista).
 *
 * Devuelve el puntero al iterador creado o NULL en caso de error.
 */
lista_iterador_t* lista_iterador_crear(lista_t* lista){
	if(!lista) return NULL;
	lista_iterador_t* iterador = calloc(1, sizeof(lista_iterador_t));
	if(!iterador) return NULL;
	iterador->nodo_actual = lista->primero;
	iterador->nodo_anterior = lista->primero;
	return iterador;
}

/*
 * Devuelve true si hay mas elementos sobre los cuales iterar o false
 * si no hay mas.
 */
bool lista_iterador_tiene_siguiente(lista_iterador_t* iterador){
	if(!iterador) return false;
	if(!iterador->nodo_actual) return false;
	return true;
}

/*
 * Devuelve el próximo elemento disponible en la iteración.
 * En caso de error devuelve NULL.
 */
void* lista_iterador_siguiente(lista_iterador_t* iterador){
	if(!iterador || !lista_iterador_tiene_siguiente(iterador)) return NULL;	
	void* elemento = iterador->nodo_actual->valor;
	iterador->nodo_anterior = iterador->nodo_anterior->siguiente;
	iterador->nodo_actual = iterador->nodo_actual->siguiente;
	return elemento;
}
/*
 * Libera la memoria reservada por el iterador.
 */
void lista_iterador_destruir(lista_iterador_t* iterador){
	free(iterador);
}

/*
 * Iterador interno. Recorre la lista e invoca la funcion con cada
 * elemento de la misma.
 */
void lista_con_cada_elemento(lista_t* lista, void (*funcion)(void*, void*), void *contexto){
	if(!funcion) return;
	nodo_t* nodo_aux = lista->primero;
	bool continuar = true;
	while(nodo_aux && continuar){
		funcion(nodo_aux->valor, contexto);
		nodo_aux = nodo_aux->siguiente;
	}
	return;
}//checkear con esta ultima

//compilar: gcc *.c -o lista_se -g -std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O0
//valgrind: valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./lista_se -g