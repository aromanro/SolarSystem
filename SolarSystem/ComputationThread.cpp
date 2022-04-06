#include "stdafx.h"
#include "ComputationThread.h"

#include "Constants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const double G = 6.67408E-11;
const double TWO_M_PI = 2. * M_PI;

const int maxQueueSize = 5;

#define EPS 1E-8

namespace MolecularDynamics {


	ComputationThread::ComputationThread()
		: an_event(0), nrsteps(1), newData(false), m_timestep(60), simulationTime(0)
	{
	}


	ComputationThread::~ComputationThread()
	{
		EndThread();
	}



	inline Vector3D<double> ComputationThread::CalculateAcceleration(BodyPositionList::const_iterator& it, BodyPositionList& BodiesPosition)
	{
		static const double EPS2 = EPS * EPS;

		Vector3D<double> acceleration(0., 0., 0.);

		int i = 0;
		for (auto cit = BodiesPosition.cbegin(); cit != BodiesPosition.cend(); ++cit, ++i)
		{
			if (cit == it) continue;

			const Vector3D<double> r21 = cit->m_Position - it->m_Position;
			const double length2 = r21 * r21;

			acceleration += r21 * m_BodyList[i].m_Mass / ((length2 + EPS2) * sqrt(length2));
		}

		return G * acceleration;
	}

#ifdef USE_VERLET

	inline void ComputationThread::VerletStep(BodyPositionList& BodiesPosition, double /*timestep*/, double timestep2)
	{
		int i = 0;
		for (auto it = BodiesPosition.cbegin(); it != BodiesPosition.cend(); ++it, ++i)
			accelerations[i] = CalculateAcceleration(it, BodiesPosition);

		i = 0;
		for (auto it = BodiesPosition.begin(); it != BodiesPosition.end(); ++it, ++i)
		{
			const Vector3D<double> nextPosition = 2. * it->m_Position - it->m_PrevPosition + accelerations[i] * timestep2;
			it->m_PrevPosition = it->m_Position;
			it->m_Position = nextPosition;
		}
	}

#else


	inline void ComputationThread::VelocityVerletStep(BodyPositionList& BodiesPosition, double timestep, double timestep2)
	{
		for (auto& body : BodiesPosition)
			body.m_Position += body.m_Velocity * timestep + 0.5 * body.m_Acceleration * timestep2;

		for (auto it = BodiesPosition.begin(); it != BodiesPosition.end(); ++it)
		{
			it->m_PrevAcceleration = it->m_Acceleration;
			it->m_Acceleration = CalculateAcceleration(it, BodiesPosition);

			it->m_Velocity += (it->m_Acceleration + it->m_PrevAcceleration) * timestep * 0.5;
		}
	}

#endif


	inline void ComputationThread::CalculateRotations(BodyPositionList& BodiesPosition, double timestep)
	{
		static const double maxLimit = 100 * TWO_M_PI;

		int i = 0;
		for (auto& body : BodiesPosition)
		{
			const double angular_speed = TWO_M_PI / m_BodyList[i].rotationPeriod;
			body.rotation += angular_speed * timestep;

			if (body.rotation >= maxLimit) body.rotation -= maxLimit;
			else if (body.rotation < 0) body.rotation += maxLimit;

			++i;
		}
	}


	void ComputationThread::Initialize(BodyPositionList& BodiesPosition)
	{
#ifdef USE_VERLET
		const double timestep = m_timestep;
		const double timestep2 = timestep * timestep;

		accelerations.resize(BodiesPosition.size());
		int i = 0;
		for (auto it = BodiesPosition.cbegin(); it != BodiesPosition.cend(); ++it, ++i)
			accelerations[i] = CalculateAcceleration(it, BodiesPosition);

		i = 0;
		for (auto it = BodiesPosition.begin(); it != BodiesPosition.end(); ++it, ++i)
		{
			it->m_PrevPosition = it->m_Position;
			it->m_Position += it->m_Velocity * timestep + 0.5 * accelerations[i] * timestep2;
		}
#else // VelocityVerlet
		for (auto it = BodiesPosition.begin(); it != BodiesPosition.end(); ++it)
			it->m_Acceleration = CalculateAcceleration(it, BodiesPosition);
#endif	
	}



	void ComputationThread::Compute()
	{
		const double timestep = m_timestep;
		const double timestep2 = timestep * timestep;

		BodyPositionList BodiesPosition(m_BodiesPositions.front().first);
		Initialize(BodiesPosition);


		do
		{
			const unsigned int local_nrsteps = nrsteps;

			for (int n = 0; n < maxQueueSize; ++n)
			{
				// do computations
				for (unsigned int i = 0; i < local_nrsteps; ++i)
				{
#ifdef USE_VERLET
					VerletStep(BodiesPosition, timestep, timestep2);
#else
					VelocityVerletStep(BodiesPosition, timestep, timestep2);
#endif
				}

				const double simulatedTime = timestep * local_nrsteps;

				CalculateRotations(BodiesPosition, simulatedTime);

				// give result to the main thread
				if (SetBodiesPosition(BodiesPosition, simulationTime + simulatedTime)) break; // break the for if the results queue is filled
			}
		} while (!Wait()); // is signaled to kill? also waits for a signal to do more work
	}

	bool ComputationThread::Wait()
	{
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [this] { return an_event > 0; });
		if (an_event > 1) return true;
		an_event = 0;

		return false;
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


	bool ComputationThread::SetBodiesPosition(const BodyPositionList& bodiesPosition, double curSimulationTime)
	{
		bool result = false;
		{
			std::lock_guard<std::mutex> lock(m_DataSection);

			m_BodiesPositions.push(std::make_pair(bodiesPosition, curSimulationTime));
			simulationTime = curSimulationTime;
			
			if (m_BodiesPositions.size() >= maxQueueSize) result = true;
		}

		newData = true;

		return result;
	}


	std::pair<BodyPositionList,double> ComputationThread::GetBodies()
	{
		std::pair<BodyPositionList, double> result;

		{
			std::lock_guard<std::mutex> lock(m_DataSection);

			result.first.swap(m_BodiesPositions.front().first);
			result.second = m_BodiesPositions.front().second;

			m_BodiesPositions.pop();

			if (m_BodiesPositions.empty()) 
				newData = false;
		}

		return result;
	}

}

