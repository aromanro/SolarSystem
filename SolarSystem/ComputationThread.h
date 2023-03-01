#pragma once

#include <list>
#include "SolarSystemBodies.h"

#include <queue>
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

		std::thread Thread;

		std::vector<Vector3D<double>> accelerations;

		std::queue<std::pair<BodyPositionList, double>>  m_BodiesPositions;

		std::mutex m_DataSection;
	
		double simulationTime;

	public:
		unsigned int m_timestep;

		ComputationThread();
		~ComputationThread();

		void EndThread();
		void StartThread();

		void SignalWantMore();

		void SetNrSteps(unsigned int nr);

		// returns true if the buffer is filled
		bool SetBodiesPosition(const BodyPositionList& BodiesPosition, double simulationTime);

		void SetBodies(const BodyList& bodies)
		{
			m_BodyList = bodies;
		}

		std::pair<BodyPositionList, double> GetBodies();

		bool HasNewData() const { return newData; }

	protected:
		void Initialize(BodyPositionList& BodiesPosition);

		inline Vector3D<double> CalculateAcceleration(int pos, BodyPositionList& Bodies);

#ifdef USE_VERLET
		inline void VerletStep(BodyPositionList& Bodies, double period, double period2);
#else
		inline void VelocityVerletStep(BodyPositionList& Bodies, double period, double period2);
#endif
		inline void CalculateRotations(BodyPositionList& Bodies, double period);
		void Compute();

		bool Wait();
	};

}