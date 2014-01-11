/*
 * MeshProcessor.h
 *
 *  Created on: 31.08.2013
 *      Author: valentin
 */

#ifndef MESHPROCESSOR_H_
#define MESHPROCESSOR_H_
#include "../libraries/tetgen/tetgen.h"
#include "ObjectData.h"
#include "utils.h"

class MeshProcessor {
public:
	MeshProcessor();
	void process(ObjectData* object);
	virtual ~MeshProcessor();
};

#endif /* MESHPROCESSOR_H_ */
