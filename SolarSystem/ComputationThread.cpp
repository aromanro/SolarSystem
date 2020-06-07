#include "stdafx.h"
#include "ComputationThread.h"

#include "Constants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const double G = 6.67408E-11;
const double TWO_M_PI = 2.*M_PI;

#define EPS 1E-8

namespace MolecularDynamics {


	ComputationThread::ComputationThread()
		: an_event(0), nrsteps(1), m_timestep(300)
	{
	}


	ComputationThread::~ComputationThread()
	{
		EndThread();
	}



	inline Vector3D<double> ComputationThread::CalculateAcceleration(BodyList::const_iterator& it, BodyList& Bodies)
	{
		static const double EPS2 = EPS*EPS;

		Vector3D<double> acceleration(0., 0., 0.);

		for (auto cit = Bodies.cbegin(); cit != Bodies.cend(); ++cit)
		{
			if (cit == it) continue;

			const Vector3D<double> r21 = cit->m_Position - it->m_Position;
			const double length = r21.Length();

			acceleration += r21 * cit->m_Mass / ((length*length + EPS2) * length);
		}

		return G * acceleration;
	}

#ifdef USE_VERLET

	inline void ComputationThread::VerletStep(BodyList& Bodies, double /*timestep*/, double timestep2)
	{
		std::vector<Vector3D<double>> accelerations(Bodies.size());
		int i = 0;
		for (auto it = Bodies.cbegin(); it != Bodies.cend(); ++it, ++i)
			accelerations[i] = CalculateAcceleration(it, Bodies);

		i = 0;
		for (auto it = Bodies.begin(); it != Bodies.end(); ++it, ++i)
		{
			const Vector3D<double> nextPosition = 2. * it->m_Position - it->m_PrevPosition + accelerations[i] * timestep2;
			it->m_PrevPosition = it->m_Position;
			it->m_Position = nextPosition;
		}
	}

#else


	inline void ComputationThread::VelocityVerletStep(BodyList& Bodies, double timestep, double timestep2)
	{
		for (auto &body : Bodies)
			body.m_Position += body.m_Velocity * timestep + 0.5 * body.m_Acceleration * timestep2;

		for (auto it = Bodies.begin(); it != Bodies.end(); ++it)
		{
			it->m_PrevAcceleration = it->m_Acceleration;
			it->m_Acceleration = CalculateAcceleration(it, Bodies);

			it->m_Velocity += (it->m_Acceleration + it->m_PrevAcceleration) * timestep * 0.5;
		}
	}

#endif


	inline void ComputationThread::CalculateRotations(BodyList& Bodies, double timestep)
	{
		for (auto &body : Bodies)
		{
			const double angular_speed = TWO_M_PI / body.rotationPeriod;
			body.rotation += angular_speed * timestep;
			if (body.rotation >= TWO_M_PI) body.rotation -= TWO_M_PI;
			else if (body.rotation < 0) body.rotation += TWO_M_PI;
		}
	}


	void ComputationThread::Initialize(BodyList& m_Bodies) const
	{
#ifdef USE_VERLET
		const double timestep = m_timestep;
		const double timestep2 = timestep*timestep;

		std::vector<Vector3D<double>> accelerations(m_Bodies.size());
		int i = 0;
		for (auto it = m_Bodies.cbegin(); it != m_Bodies.cend(); ++it, ++i)
			accelerations[i] = CalculateAcceleration(it, m_Bodies);

		i = 0;
		for (auto it = m_Bodies.begin(); it != m_Bodies.end(); ++it, ++i)
		{
			it->m_PrevPosition = it->m_Position;
			it->m_Position += it->m_Velocity * timestep + 0.5 * accelerations[i] * timestep2;
		}
#else // VelocityVerlet
		for (auto it = m_Bodies.begin(); it != m_Bodies.end(); ++it)
			it->m_Acceleration = CalculateAcceleration(it, m_Bodies);
#endif	
	}



	void ComputationThread::Compute()
	{
		const double timestep = m_timestep;
		const double timestep2 = timestep*timestep;

		BodyList m_Bodies(GetBodies());
		Initialize(m_Bodies);

		for (;;)
		{
			const unsigned int local_nrsteps = nrsteps;

			// do computations
			for (unsigned int i = 0; i < local_nrsteps; ++i)
#ifdef USE_VERLET
				VerletStep(m_Bodies, timestep, timestep2);
#else
				VelocityVerletStep(m_Bodies, timestep, timestep2);
#endif

			CalculateRotations(m_Bodies, local_nrsteps*timestep);

			// give result to the main thread
			SetBodies(m_Bodies);

			// is signaled to kill? also waits for a signal to do more work
			{
				std::unique_lock<std::mutex> lock(mtx);
				cv.wait(lock, [this] { return an_event > 0; });
				if (an_event > 1) break;
				an_event = 0;
			}
		}
	}

	void ComputationThread::EndThread()
	{
		if (Thread.joinable())
		{
			{
				std::lock_guard<std::mutex> lock(mtx);
				an_event = 2;
			}
			cv.notify_one();
			Thread.join();
			an_event = 0;
		}
	}

	void ComputationThread::StartThread()
	{
		EndThread();

		Thread = std::thread(&ComputationThread::Compute, this);
	}

	void ComputationThread::SignalWantMore()
	{
		{
			std::lock_guard<std::mutex> lock(mtx);
			an_event = 1;
		}
		cv.notify_one();
	}


	void ComputationThread::SetNrSteps(unsigned int nr)
	{
		nrsteps = nr;
	}


	void ComputationThread::SetBodies(const BodyList& bodies)
	{
		std::lock_guard<std::mutex> lock(m_DataSection);

		m_SharedBodies = bodies;
	}


	BodyList ComputationThread::GetBodies()
	{
		std::lock_guard<std::mutex> lock(m_DataSection);

		BodyList bodies(m_SharedBodies);

		return std::move(bodies);
	}

}

