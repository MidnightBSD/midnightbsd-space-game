#ifndef LINKEDLIST_H
#define LINKEDLIST_H

/*
 *  LinkedList.h
 *
 *  Created by Allen Smith on Mon Jun 14 2004.
 *
 */

#include "Node.h"

template<class Object>
class LinkedList{
	
	public:
		Node<Object> *head;
		Node<Object> *tail;
	
	public:
		LinkedList();
		
		Node<Object> * insertObject(Object *);
		Node<Object> * insertObjectAfterPosition(Node<Object> *, Object *);
		Node<Object> * removeNode(Node<Object> *);
		void clearList();
		bool isEmpty();
};



/****************************************************
 * LinkedList
 *  constructor
 *  creates an empty list with a head and a tail
 ***************************************************/
template<class Object>
LinkedList<Object>::LinkedList(){
	head = new Node<Object>; //creates an empty Node for the head
	tail = new Node<Object>; //creates a tail after the head
	head->nextNode = tail; //connects the head to the tail
	tail->previousNode = head;
}


/****************************************************
 * insertObject
 *  creates a new Node at the head of the list
 ***************************************************/
template<class Object>
Node<Object> * LinkedList<Object>::insertObject(Object *data){
	return insertObjectAfterPosition(head, data);
//Code to do it manually
//	Node<Object> *newNode = new Node<Object>(&head, data);
//	head.nextNode->previousNode = newNode;
//	head.nextNode = newNode;
}


/****************************************************
 * insertObjectAfterPosition
 *  creates a new Node after the specified previous Node
 ***************************************************/
template<class Object>
Node<Object> * LinkedList<Object>::insertObjectAfterPosition(Node<Object> *previous, Object *data){
	Node<Object> *newNode = new Node<Object>(previous, data);
	previous->nextNode->previousNode = newNode;
	previous->nextNode = newNode;
	return newNode;
}


/****************************************************
 * removeNode
 *  takes the specified Node out of the list
 *  returns the Node before the one just deleted
 ***************************************************/
template<class Object>
Node<Object> * LinkedList<Object>::removeNode(Node<Object> *NodeToDie){
	Node<Object> *previousNode = NodeToDie->previousNode;
	NodeToDie->previousNode->nextNode = NodeToDie->nextNode; //connect the previous Node across the Node to be removed
	NodeToDie->nextNode->previousNode = NodeToDie->previousNode; //connect the next Node backward across the Node to be removed
	delete NodeToDie->object;
	delete NodeToDie;
	return previousNode;
}


/****************************************************
 * clearList
 *  removes every node in the list, except the head and tail
 ***************************************************/
template<class Object>
void LinkedList<Object>::clearList(){
	if(isEmpty() == false){ //make sure the list isn't already empty!
		do{
			removeNode(head->nextNode); //we just continually remove the node after the head until we get them all
		}while(isEmpty() == false);
	}
}


/****************************************************
 * isEmpty
 *  returns true if the list has no contents
 ***************************************************/
template<class Object>
bool LinkedList<Object>::isEmpty(){
	return (head->nextNode == tail);
}


#endif