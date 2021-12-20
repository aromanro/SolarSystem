#pragma once

#include <list>
#include "SolarSystemBodies.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace MolecularDynamics {


	class ComputationThread
	{
	private:
		std::mutex mtx; // protect an_event and cv
		std::condition_variable cv;
		unsigned int an_event; // 0, no event, 1 want more data, 2 finish

		std::atomic_uint nrsteps;
		std::atomic_bool newData;

		BodyList m_BodyList;
		BodyPositionList m_SharedBodiesPosition;

		std::thread Thread;

		std::vector<Vector3D<double>> accelerations;
	public:
		std::mutex m_DataSection;

		unsigned int m_timestep;
		double simulationTime;

		ComputationThread();
		~ComputationThread();

		void EndThread();
		void StartThread();

		void SignalWantMore();

		void SetNrSteps(unsigned int nr);

		void SetBodiesPosition(const BodyPositionList& BodiesPosition, double simulationTime);
		void SetBodies(const BodyList& bodies)
		{
			m_BodyList = bodies;
		}
		BodyPositionList& GetBodies();

		bool HasNewData() const { return newData; }

	protected:
		void Initialize(const BodyList& Bodies, BodyPositionList& BodiesPosition);

		inline Vector3D<double> CalculateAcceleration(BodyPositionList::const_iterator& it, BodyPositionList& Bodies);

#ifdef USE_VERLET
		inline void VerletStep(BodyPositionList& Bodies, double period, double period2);
#else
		inline VelocityVerletStep(BodyPositionList& Bodies, double period, double period2);
#endif
		inline void CalculateRotations(BodyPositionList& Bodies, double period);
		void Compute();

		bool Wait();
	};

}