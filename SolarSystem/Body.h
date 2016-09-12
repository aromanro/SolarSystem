#pragma once

#include "Vector3D.h"

namespace MolecularDynamics {

	class Body
	{
	public:
		double m_Mass;
		double m_Radius;
		double rotationPeriod;
		double rotation;

		Vector3D<double> m_Position;
		Vector3D<double> m_Velocity;
		Vector3D<double> m_Acceleration;
		Vector3D<double> m_PrevAcceleration;



		Body();
		~Body();
	};

}