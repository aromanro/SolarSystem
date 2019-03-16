#include "stdafx.h"
#include "Body.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace MolecularDynamics {

	Body::Body()
		: m_Mass(0), m_Radius(0), rotation(0), rotationPeriod(24 * 3600),
#ifndef USE_VERLET		
		m_Acceleration(0, 0, 0), m_PrevAcceleration(0, 0, 0), 
#endif		
		m_Velocity(0, 0, 0),
		m_Position(0, 0, 0)
	{
	}


	Body::~Body()
	{
	}

}