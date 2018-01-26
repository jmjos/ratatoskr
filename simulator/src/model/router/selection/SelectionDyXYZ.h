#ifndef SRC_MODEL_SELECTION_SELECTIONDYXYZ_H_
#define SRC_MODEL_SELECTION_SELECTIONDYXYZ_H_


#include "Selection.h"

struct SelectionDyXYZ : public Selection {
	int rrVC=0;

	SelectionDyXYZ(Node* node):Selection(node){

	};
	~SelectionDyXYZ(){

	};
	//void checkValid()=0;
	void select(RoutingInformation* ri, RoutingPacketInformation* rpi);
};


#endif /* SRC_MODEL_SELECTION_SELECTIONROUNDROBIN_H_ */
