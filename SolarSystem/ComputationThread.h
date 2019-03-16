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

		BodyList m_SharedBodies;

		std::mutex m_DataSection;

		std::thread Thread;
	public:
		unsigned int m_timestep;

		ComputationThread();
		~ComputationThread();

		void EndThread();
		void StartThread();

		void SignalWantMore();

		void SetNrSteps(unsigned int nr);

		void SetBodies(const BodyList& bodies);
		BodyList GetBodies();

	protected:
		void Initialize(BodyList& m_Bodies) const;

		inline static Vector3D<double> CalculateAcceleration(BodyList::const_iterator& it, BodyList& Bodies);

#ifdef USE_VERLET
		inline static void VerletStep(BodyList& Bodies, double period, double period2);
#else
		inline static void VelocityVerletStep(BodyList& Bodies, double period, double period2);
#endif
		inline static void CalculateRotations(BodyList& Bodies, double period);
		void Compute();
	};

}