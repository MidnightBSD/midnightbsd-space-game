#ifndef ITERATOR_H
#define ITERATOR_H

/*
 *  Iterator.h
 *
 *  Allows sequential access to the elements of a linked list
 *
 *  Created by Allen Smith on Mon Jun 14 2004.
 *
 */

#include "LinkedList.h"

template<class Object>
class Iterator{

	private:
		Node<Object> *currentNode;
		LinkedList<Object> *list;
		
	public:
		Iterator(LinkedList<Object> *);
		Iterator(LinkedList<Object> *, Node<Object> *);
		Object * nextObject();
		Node<Object> * getCurrentNode();
		void removeCurrentPosition();
		bool atEnd();
		void rewind();
		
};


/****************************************************
 * Iterator
 *  constructor
 *  creates an iterator for the specified LinkedList
 ***************************************************/
template<class Object>
Iterator<Object>::Iterator(LinkedList<Object> *theList){
	list = theList;
	currentNode = theList->head;
}


/****************************************************
 * Iterator
 *  constructor with Node specified
 *  creates an iterator for the specified LinkedList pointed at the specified Node
 ***************************************************/
template<class Object>
Iterator<Object>::Iterator(LinkedList<Object> *theList, Node<Object> *startNode){
	list = theList;
	currentNode = startNode;
}


/****************************************************
 * nextObject
 *  advances to the next Node and returns the object in it
 ***************************************************/
template<class Object>
Object* Iterator<Object>::nextObject(){
	currentNode = currentNode->nextNode;
	if(currentNode != list->tail)
		return currentNode->object;
	else
		return NULL;
}


/****************************************************
 * getCurrentNode
 *  returns the Node at which we currently reside
 ***************************************************/
template<class Object>
Node<Object> * Iterator<Object>::getCurrentNode(){
	return currentNode;
}



/****************************************************
 * removeCurrentPosition
 *  deletes the current node, setting the iterator to the previous Node
 ***************************************************/
template<class Object>
void Iterator<Object>::removeCurrentPosition(){
	currentNode = list->removeNode(currentNode);
}


/****************************************************
 * atEnd
 *  returns true if the iterator is currently pointing to the tail
 ***************************************************/
template<class Object>
bool Iterator<Object>::atEnd(){
	return (currentNode == list->tail );
}


/****************************************************
 * rewind
 *  returns the beginning of the list
 ***************************************************/
template<class Object>
void Iterator<Object>::rewind(){
	currentNode = list->head;
}


#endif