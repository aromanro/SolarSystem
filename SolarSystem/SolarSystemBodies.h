#pragma once

#include <vector>

#include "Body.h"
#include "BodyProperties.h"

typedef std::vector<MolecularDynamics::Body> BodyList;
typedef std::vector<BodyProperties> BodyPropList;


class SolarSystemBodies
{
public:
	BodyList m_Bodies;
	BodyPropList m_BodyProperties;
	double m_simulationTime;

	SolarSystemBodies();
	~SolarSystemBodies();
	void clear();
	void LoadTextures();
};

