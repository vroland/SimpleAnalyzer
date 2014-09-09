/*
 * MeshProcessor.h
 *
 *  Created on: 31.08.2013
 *      Author: Valentin Roland
 */

#ifndef MESHPROCESSOR_H_
#define MESHPROCESSOR_H_
#include "../libraries/tetgen/tetgen.h"
#include "ObjectData.h"
#include "utils.h"

/**
 * @brief Errechnet die Temperaturverteilung für ein Objekt.
 */
class MeshProcessor {
public:
	/**
	 * Der Konstruktor.
	 */
	MeshProcessor();

	/**
	 * Berechnet die Temperaturverteilung für ein Objekt.
	 * @param object Das Objekt, für das die Temperaturverteilung ermittelt werden soll.
	 */
	void process(ObjectData* object);

	/**
	 * Der Destruktor.
	 */
	virtual ~MeshProcessor();
};

#endif /* MESHPROCESSOR_H_ */
