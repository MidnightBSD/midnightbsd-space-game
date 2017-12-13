#ifndef Node_H
#define Node_H

/*
 *  Node.h
 *
 *  Created by Allen Smith on Tue Jun 15 2004.
 *
 */


//Node for a linked list
template<class Object>
class Node{
	public:
		Node<Object> *previousNode;
		Object *object;
		Node<Object> *nextNode;
		
		bool operator==( const Node<Object> &) const;

		Node();
		Node(Node<Object> *, Object *);
};


/****************************************************
 * ==
 *  operator definition
 ***************************************************/
template<class Object>
bool Node<Object>::operator==(const Node<Object> &rightHandSide) const{
	if( previousNode == rightHandSide.previousNode &&
		//object == rightHandSide.object && //actually, there is no need to test the object itself. If two nodes have the same position, then they should be the same in the list.
		nextNode == rightHandSide.nextNode )
		return true;
	else
		return false;
}


/****************************************************
 * Node
 *  constructor
 *  creates a new Node to be positioned after the specified Node
 ***************************************************/
template<class Object>
Node<Object>::Node(){
	previousNode = NULL;
	object = NULL;
	nextNode = NULL;
}
 
template<class Object>
Node<Object>::Node(Node<Object> *specifiedPreviousNode, Object *data){
	previousNode = specifiedPreviousNode;
	object = data;
	nextNode = specifiedPreviousNode->nextNode;
}

#endif