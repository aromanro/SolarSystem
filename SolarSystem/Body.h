#pragma once

#include "Vector3D.h"

#define USE_VERLET 1

namespace MolecularDynamics {

	class Body
	{
	public:
		virtual ~Body() = default;

		double m_Mass = 0.;
		double m_Radius = 0.;
		double rotationPeriod = 24. * 3600;
	};

	class BodyPosition
	{
	public:
		double rotation = 0;

		Vector3D<double> m_Position;

		// velocity is actually used only by Velocity Verlet
		// the Verlet algorithm does not use it (except the initial velocity in the initialization step), so actually you could avoid storing it here
		// if the velocity is needed, one can use (r(t+dt) + r(t-dt)) / (2 * dt) to calculate it, the value here will remain the initial velocity
		Vector3D<double> m_Velocity; 

#ifdef USE_VERLET
		Vector3D<double> m_PrevPosition;		
#else
		// Velocity Verlet - for Velocity Verlet you can avoid storing accelerations, but I think the code looks nicer this way
		Vector3D<double> m_Acceleration;
		Vector3D<double> m_PrevAcceleration;
#endif
	};

}