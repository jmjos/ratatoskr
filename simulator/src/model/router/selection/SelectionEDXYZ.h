#ifndef SRC_MODEL_SELECTION_SELECTIONEDXYZ_H_
#define SRC_MODEL_SELECTION_SELECTIONEDXYZ_H_


#include "Selection.h"

struct SelectionEDXYZ : public Selection {
	int rrVC=0;

	SelectionEDXYZ(Node* node):Selection(node){

	};
	~SelectionEDXYZ(){

	};
	//void checkValid()=0;
	void select(RoutingInformation* ri, RoutingPacketInformation* rpi);
};


#endif /* SRC_MODEL_SELECTION_SELECTIONROUNDROBIN_H_ */
