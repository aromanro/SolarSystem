#include "stdafx.h"
#include "SpaceshipOrientation.h"


void SpaceshipOrientation::Tick()
{
	ComputeRotations();

	if (Rotated())
	{
		const auto curTime = std::chrono::system_clock::now();

		if (RotatedVertically())
		{
			const double rotTime = std::chrono::duration<double>(curTime - startRotationXtime).count();
			if (TimeToRotate(startRotationX, targetRotationX) + delayBeforeUndoingRotation < rotTime)
			{
				startRotationX = rotationX;
				targetRotationX = 0;
				startRotationXtime = std::chrono::system_clock::now();
			}
		}

		if (RotatedHorizontally())
		{
			const double rotTime = std::chrono::duration<double>(curTime - startRotationYtime).count();
			if (TimeToRotate(startRotationY, targetRotationY) + delayBeforeUndoingRotation < rotTime)
			{
				startRotationY = rotationY;
				targetRotationY = 0;
				startRotationYtime = std::chrono::system_clock::now();
			}
		}

		if (Rolled())
		{
			const double rotTime = std::chrono::duration<double>(curTime - startRotationZtime).count();
			if (TimeToRotate(startRotationZ, targetRotationZ) + delayBeforeUndoingRotation < rotTime)
			{
				startRotationZ = rotationZ;
				targetRotationZ = 0;
				startRotationZtime = std::chrono::system_clock::now();
			}
		}
	}
}


void SpaceshipOrientation::RotateUpDown(double angle)
{
	ComputeRotations();

	startRotationX = rotationX;
	//targetRotationX = (angle > 0 ? 1 : -1) * min(abs(angle), rotationAngleMax);
	targetRotationX = angle;
	startRotationXtime = std::chrono::system_clock::now();
}


void SpaceshipOrientation::RotateUp()
{
	RotateUpDown(rotationAngleMax);
}

void SpaceshipOrientation::RotateDown()
{
	RotateUpDown(-rotationAngleMax);
}

void SpaceshipOrientation::RotateLeftRight(double angle)
{
	ComputeRotations();

	startRotationY = rotationY;
	//targetRotationY = (angle > 0 ? 1 : -1) * min(abs(angle), rotationAngleMax);
	targetRotationY = angle;
	startRotationYtime = std::chrono::system_clock::now();
}

void SpaceshipOrientation::RotateLeft()
{
	RotateLeftRight(rotationAngleMax);
}

void SpaceshipOrientation::RotateRight()
{
	RotateLeftRight(-rotationAngleMax);
}

void SpaceshipOrientation::RollLeftRight(double angle)
{
	ComputeRotations();

	startRotationZ = rotationZ;
	targetRotationZ = (angle > 0 ? 1 : -1) * min(abs(angle), rotationAngleMax);
	//targetRotationZ = angle;
	startRotationZtime = std::chrono::system_clock::now();
}


void SpaceshipOrientation::RollLeft()
{
	RollLeftRight(rotationAngleMax);
}

void SpaceshipOrientation::RollRight()
{
	RollLeftRight(-rotationAngleMax);
}


void SpaceshipOrientation::ComputeRotations()
{
	if (Rotating())
	{
		const auto curTime = std::chrono::system_clock::now();

		ComputeVerticalRotation(curTime);
		ComputeHorizontalRotation(curTime);
		ComputeRollRotation(curTime);
	}
}

double SpaceshipOrientation::RotationAngle(double rotTime, double start, double position, double target) const
{
	// this should do for now, it changed to accelerate/decellerate, modify TimeToRotate as well
	//return rotTime * rotationSpeed;
	const double halfway = abs(target - start) / 2.;
	const double angleAccel = min(rotationAngleAccel, halfway);
	const double timeToAccelerate = 2. * angleAccel / rotationSpeed;
	const double acceleration = rotationSpeed / timeToAccelerate;

	double timeConstantVelocity = 0;
	if (angleAccel < halfway)
		timeConstantVelocity = 2. * (halfway - angleAccel) / rotationSpeed;

	if (rotTime < timeToAccelerate)
	{
		return acceleration * rotTime * rotTime / 2.;
	}
	else if (rotTime > timeToAccelerate + timeConstantVelocity)
	{
		const double leftTime = rotTime - (timeToAccelerate + timeConstantVelocity);
		double deccelerationAngle = leftTime * rotationSpeed - acceleration * leftTime * leftTime / 2.;
		if (deccelerationAngle < 0) deccelerationAngle = 0;

		return 0.1 + angleAccel + timeConstantVelocity * rotationSpeed + deccelerationAngle;
	}
	
	return 0.1 + angleAccel + (rotTime - timeToAccelerate) * rotationSpeed;
}

void SpaceshipOrientation::ComputeVerticalRotation(const std::chrono::system_clock::time_point& curTime)
{
	if (RotatingVertically())
	{
		const double rotTime = std::chrono::duration<double>(curTime - startRotationXtime).count();
		if (RotatingUp())
		{
			rotationX = startRotationX + RotationAngle(rotTime, startRotationX, rotationX, targetRotationX);
			if (rotationX > targetRotationX) rotationX = targetRotationX;
		}
		else
		{
			rotationX = startRotationX - RotationAngle(rotTime, startRotationX, rotationX, targetRotationX);
			if (rotationX < targetRotationX) rotationX = targetRotationX;
		}
	}
}

void SpaceshipOrientation::ComputeHorizontalRotation(const std::chrono::system_clock::time_point& curTime)
{
	if (RotatingHorizontally())
	{
		const double rotTime = std::chrono::duration<double>(curTime - startRotationYtime).count();
		if (RotatingLeft())
		{
			rotationY = startRotationY - RotationAngle(rotTime, startRotationY, rotationY, targetRotationY);
			if (rotationY < targetRotationY) rotationY = targetRotationY;
		}
		else
		{
			rotationY = startRotationY + RotationAngle(rotTime, startRotationY, rotationY, targetRotationY);
			if (rotationY > targetRotationY) rotationY = targetRotationY;
		}
	}
}

void SpaceshipOrientation::ComputeRollRotation(const std::chrono::system_clock::time_point& curTime)
{
	if (Rolling())
	{
		const double rotTime = std::chrono::duration<double>(curTime - startRotationZtime).count();
		if (RollingLeft())
		{
			rotationZ = startRotationZ - RotationAngle(rotTime, startRotationZ, rotationZ, targetRotationZ);
			if (rotationZ < targetRotationZ) rotationZ = targetRotationZ;
		}
		else
		{
			rotationZ = startRotationZ + RotationAngle(rotTime, startRotationZ, rotationZ, targetRotationZ);
			if (rotationZ > targetRotationZ) rotationZ = targetRotationZ;
		}
	}
}

