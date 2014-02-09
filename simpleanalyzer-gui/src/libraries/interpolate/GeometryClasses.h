/*
 * GeometryClasses.h
 *
 *  Created on: 24.07.2013
 *      Author: valentin
 */
#include <iostream>
#ifndef GEOMETRYCLASSES_H_
#define GEOMETRYCLASSES_H_
/**
 * @brief 3D-Vektorklasse mit nützlichen Operationen.
 */
class Vector3D {
public:
	friend std::ostream &operator<< (std::ostream &out, const Vector3D &vec);
public:
	/**
	 * Konstruktor für Konstruktion aus einzelnen Koordinaten.
	 * @param x X-Element des Vektors.
	 * @param y Y-Element des Vektors.
	 * @param z Z-Element des Vektors.
	 */
	Vector3D(double x,double y,double z);
	/**
	 * Konstruktor für Konstruktion aus einer Koordinatenliste.
	 * @param values Liste der Koordinaten (x,y und z-Wert).
	 */
	Vector3D(const double* values);
	/**
	 * Konstruktor für die Konstruktion aus einem anderen Vektor.
	 * @param other Der Vektor, dessen Eigenschaften übernommen werden sollen.
	 */
	Vector3D(Vector3D* other);
	/**
	 * Gibt eine Kopie des Vektors zurück. Der zurückgegebene Vektor muss manuell mit delete Freigegeben werden!
	 */
	Vector3D* copy();
	/**
	 * Gibt das X-Element des Vektors zurück.
	 */
	double getX();
	/**
	 * Gibt das Y-Element des Vektors zurück.
	 */
	double getY();
	/**
	 * Gibt das Z-Element des Vektors zurück.
	 */
	double getZ();
	/**
	 * Gibt die Länge des Vektors zurück.
	 */
	double getLength();
	/**
	 * Gibt den Winkel zu einem anderen Vektor in RAD zurück.
	 * @param other Der Vektor, zu dem der Winkel ermittelt werden soll.
	 */
	double getAngleTo(Vector3D* other);
	/**
	 * Gibt das Skalarprodukt mit einem anderen Vektor.
	 * @param other Der Vektor, mit dem das Skalarprodukt gebildet werden soll.
	 */
	double dotProduct(Vector3D* other);
	/**
	 * Gibt das Kreuzprodukt mit einem anderen Vektor zurück. Der zurückgegebene Vektor muss manuell mit delete Freigegeben werden!
	 * @param other Der Vektor, mit dem das Kreuzprodukt gebildet werden soll.
	 */
	Vector3D* crossProduct(Vector3D* other);
	/**
	 * Addiert einen Vektor zu diesem Vektor.
	 * @param other Der zu addierende Vektor.
	 */
	void add(Vector3D* other);
	/**
	 * Subtrahiert einen Vektor von diesem Vektor.
	 * @param other Der zu subtrahierende Vektor.
	 */
	void sub(Vector3D* other);
	/**
	 * Multipliziert den Vektor mit einem Skalar.
	 * @param scalar Der Skalar.
	 */
	void mult(double scalar);
	/**
	 * Normalisiert den Vektor.
	 */
	void normalize();
	/**
	 * Testet, ob zwei Vektoren identisch sind.
	 * @param other Der Vektor, mit dem verglichen werden soll.
	 */
	bool equals(Vector3D* other);
	/**
	 * Gibt den Abstand zu einem anderen Vektor zurück. Dabei werden beide Vektoren als Ortsvektoren betrachtet.
	 * @param other Der Vektor, zu dem der Abstand ermittelt werden soll.
	 */
	double getDistanceTo(Vector3D* other);
	/**
	 * Gibt eine Referenz auf die Vektorelemente zurück (Vor allem zur Übergabe an OpenGL verwendet).
	 */
	double* getXYZ();
	/**
	 * Gibt den Vektor auf dem cout-Stream aus.
	 */
	void print();
	/**
	 * Gibt den Vektor auf dem gegebenen Stream aus.
	 * @param stream Der zu verwendende Stream.
	 */
	void printTo(std::ostream &stream) const;
	/**
	 * Der Destruktor.
	 */
	virtual ~Vector3D();
private:
	/**
	 * Die Elemente des Vektors.
	 */
	double coords[3];
};
/**
 * Definition des <<-Operators für die Ausgabe eines Vektors.
 */
std::ostream &operator<< (std::ostream &out, const Vector3D &vec);

/**
 * @brief 3x3-Matrixklasse mit Operationen.
 */
class Matrix3D {
public:
	/**
	 * Der Standardkonstruktor. Erzeugt eine Standardmatrix:\n
	 \f{eqnarray*}{
        1 & 0 & 0 \\
        0 & 1 & 0 \\
        0 & 0 & 1 \\
   	   \f}
   	*/
	Matrix3D();
	/**
	 * Erzeugt eine Matrix mit den gegebenen Elementen:\n
	 \f{eqnarray*}{
        x1 & y1 & z1 \\
        x2 & y2 & z2 \\
        x3 & y3 & z3 \\
   	   \f}
	 */
	Matrix3D(double x1, double y1, double z1,
			 double x2, double y2, double z2,
			 double x3, double y3, double z3);
	/**
	 * Multipliziert die Matrix mit einer anderen Matrix.
	 * @param other Die Matrix, mit der multipliziert werden soll.
	 */
	void mult(Matrix3D* other);
	/**
	 * Multipliziert die Matrix mit einem Vektor.
	 * @param other Der Vektor, mit dem multipliziert werden soll.
	 * @return Der durch die Multiplikation entstandene Vektor.
	 * Der zurückgegebene Vektor muss manuell mit delete Freigegeben werden!
	 */
	Vector3D* mult(Vector3D* other);
	/**
	 * Rotiert die Matrix um einen bestimmten Winkel auf der X-Achse.
	 * @param angle Der Winkel, um den rotiert werden soll in RAD.
	 */
	void rotateX(double angle);
	/**
	 * Rotiert die Matrix um einen bestimmten Winkel auf der Y-Achse.
	 * @param angle Der Winkel, um den rotiert werden soll in RAD.
	 */
	void rotateY(double angle);
	/**
	 * Rotiert die Matrix um einen bestimmten Winkel auf der Z-Achse.
	 * @param angle Der Winkel, um den rotiert werden soll in RAD.
	 */
	void rotateZ(double angle);
	/**
	 * Transponiert die Matrix.
	 */
	void transpose();
	/**
	 * Gibt die Matrix auf dem cout-Stream aus.
	 */
	void print();
private:
	/**
	 * Die Elemente der Matrix.
	 */
	double elements[9];
};
/**
 * @brief Ein durch 3 Ortsvektoren beschriebenes Dreieck.
 */
class Triangle {
public:
	/**
	 * Der Konstruktor. Die übergebenen Vektorobjekte werden als Element der Klasse gespeichert (nicht kopiert).
	 * @param v1 Ortsvektor zum 1. Punkt des Dreiecks.
	 * @param v2 Ortsvektor zum 2. Punkt des Dreiecks.
	 * @param v3 Ortsvektor zum 3. Punkt des Dreiecks.
	 */
	Triangle(Vector3D* v1,Vector3D* v2, Vector3D* v3);
	/**
	 * Gibt eine Referenz auf den Ortsvektor zum ersten Punkt des Dreiecks zurück.
	 */
	Vector3D* getV1();
	/**
	 * Gibt eine Referenz auf den Ortsvektor zum zweiten Punkt des Dreiecks zurück.
	 */
	Vector3D* getV2();
	/**
	 * Gibt eine Referenz auf den Ortsvektor zum dritten Punkt des Dreiecks zurück.
	 */
	Vector3D* getV3();
	/**
	 * Gibt eine Referenz auf den Ortsvektor zum index+1 Punkt des Dreiecks zurück.
	 * @param index Der Index des gesuchten Punktes (0..2).
	 */
	Vector3D* getVert(int index);
	/**
	 * Gibt die Punkte des Dreiecks auf dem cout-Stream aus.
	 */
	void print();
	/**
	 * Gibt die Normale des Dreiecks zurück. Der zurückgegebene Vektor muss manuell mit delete Freigegeben werden!
	 */
	Vector3D* getNormal();
	/**
	 * Der Destruktor.
	 */
	~Triangle();
private:
	/**
	 * Die Referenzen auf die Ortsvektoren zu den Eckpunkten des Dreiecks.
	 */
	Vector3D* verts[3];
};

/**
 * @brief Ein durch 4 Ortsvektoren beschriebener Tetraeder.
 */
class Tetrahedron {
public:
	/**
	 * Der Konstruktor. Die übergebenen Vektorobjekte werden als Element der Klasse gespeichert (nicht kopiert).
	 * @param v1 Ortsvektor zum 1. Punkt des Tetraeders.
	 * @param v2 Ortsvektor zum 2. Punkt des Tetraeders.
	 * @param v3 Ortsvektor zum 3. Punkt des Tetraeders.
	 * @param v4 Ortsvektor zum 4. Punkt des Tetraeders.
	 */
	Tetrahedron(Vector3D* v1,Vector3D* v2,Vector3D* v3,Vector3D* v4);
	/**
	 * Gibt eine Referenz auf den Ortsvektor zum 1. Punkt des Tetraeders zurück.
	 */
	Vector3D* getV1();
	/**
	 * Gibt eine Referenz auf den Ortsvektor zum 2. Punkt des Tetraeders zurück.
	 */
	Vector3D* getV2();
	/**
	 * Gibt eine Referenz auf den Ortsvektor zum 3. Punkt des Tetraeders zurück.
	 */
	Vector3D* getV3();
	/**
	 * Gibt eine Referenz auf den Ortsvektor zum 3. Punkt des Tetraeders zurück.
	 */
	Vector3D* getV4();
	/**
	 * Gibt eine Referenz auf den Ortsvektor zum index+1 Punkt des Tetraeders zurück.
	 * @param index Der Index des gesuchten Punktes (0..3).
	 */
	Vector3D* getVert(int index);
private:
	/**
	 * Die Referenzen auf die Ortsvektoren zu den Eckpunkten des Tetraeders.
	 */
	Vector3D* verts[4];
};

#endif /* GEOMETRYCLASSES_H_ */
