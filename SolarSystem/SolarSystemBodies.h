#pragma once

#include <vector>

#include "Body.h"
#include "BodyProperties.h"

using BodyPositionList = std::vector<MolecularDynamics::BodyPosition>;
using BodyList = std::vector<MolecularDynamics::Body>;
using BodyPropList = std::vector<BodyProperties>;


class SolarSystemBodies
{
public:
	BodyList m_Bodies;
	BodyPositionList m_BodiesPosition;
	BodyPropList m_BodyProperties;
	double m_simulationTime;

	SolarSystemBodies();
	~SolarSystemBodies();
	void clear();
	void LoadTextures();
};

