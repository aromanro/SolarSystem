#include "stdafx.h"
#include "ComputationThread.h"

#include "Constants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const double G = 6.67408E-11;
const double TWO_M_PI = 2.*M_PI;

#define EPS 0.01

inline void ComputationThread::CalculateAcceleration(BodyList::iterator& it, BodyList& Bodies)
{
	static const double EPS2 = EPS*EPS;

	Vector3D<double> r21;
	double length;

	it->m_PrevAcceleration = it->m_Acceleration;
	it->m_Acceleration = Vector3D<double>(0., 0., 0.);

	for (auto cit = Bodies.begin(); cit != Bodies.end(); ++cit)
	{
		if (cit == it) continue;

		r21 = cit->m_Position - it->m_Position;
		length = r21.Length();

		it->m_Acceleration += r21 * cit->m_Mass / ((length*length + EPS2) * length);
	}

	it->m_Acceleration *= G;
}



inline void ComputationThread::VelocityVerletStep(BodyList& Bodies, double timestep, double timestep2)
{
	for (auto &body : Bodies)
		body.m_Position += body.m_Velocity * timestep + 0.5 * body.m_Acceleration * timestep2;

	for (auto it = Bodies.begin();it != Bodies.end(); ++it)
	{
		CalculateAcceleration(it, Bodies);

		it->m_Velocity += (it->m_Acceleration + it->m_PrevAcceleration) * timestep * 0.5;
	}
}

inline void ComputationThread::CalculateRotations(BodyList& Bodies, double timestep)
{
	for (auto &body : Bodies)
	{
		double angular_speed = TWO_M_PI / body.rotationPeriod;
		body.rotation += angular_speed * timestep;
		if (body.rotation >= TWO_M_PI) body.rotation -= TWO_M_PI;
		else if (body.rotation < 0) body.rotation += TWO_M_PI;
	}
}


void ComputationThread::Compute()
{
	unsigned int local_nrsteps;
	const double timestep = m_timestep;
	const double timestep2 = timestep*timestep;

	BodyList m_Bodies;
	GetBodies(m_Bodies);

	for (auto it = m_Bodies.begin(); it != m_Bodies.end(); ++it)
		CalculateAcceleration(it, m_Bodies);

	for (;;)
	{
		local_nrsteps = nrsteps;

		// do computations
		for (unsigned int i = 0; i < local_nrsteps; ++i)
			VelocityVerletStep(m_Bodies, timestep, timestep2);

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


ComputationThread::ComputationThread()
	: nrsteps(1), m_timestep(300), an_event(0)
{
}


ComputationThread::~ComputationThread()
{
	EndThread();
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

	Thread = std::thread(&ComputationThread::Compute,this);
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


void ComputationThread::GetBodies(BodyList& bodies)
{
	std::lock_guard<std::mutex> lock(m_DataSection);

	bodies = m_SharedBodies;
}



