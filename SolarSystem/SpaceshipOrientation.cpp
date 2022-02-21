#include "stdafx.h"
#include "SpaceshipOrientation.h"


void SpaceshipOrientation::Tick()
{
	ComputeRotations();

	if (Rotated())
	{
		const auto curTime = std::chrono::system_clock::now();

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
	}
}


void SpaceshipOrientation::RotateUp()
{
	ComputeRotations();

	startRotationX = rotationX;
	targetRotationX = rotationAngleMax;
	startRotationXtime = std::chrono::system_clock::now();
}

void SpaceshipOrientation::RotateDown()
{
	ComputeRotations();

	startRotationX = rotationX;
	targetRotationX = -rotationAngleMax;
	startRotationXtime = std::chrono::system_clock::now();
}

void SpaceshipOrientation::RotateLeft()
{
	ComputeRotations();

	startRotationY = rotationY;
	targetRotationY = rotationAngleMax;
	startRotationYtime = std::chrono::system_clock::now();
}

void SpaceshipOrientation::RotateRight()
{
	ComputeRotations();

	startRotationY = rotationY;
	targetRotationY = -rotationAngleMax;
	startRotationYtime = std::chrono::system_clock::now();
}


void SpaceshipOrientation::ComputeRotations()
{
	if (targetRotationX != rotationX || targetRotationY != rotationY)
	{
		const auto curTime = std::chrono::system_clock::now();

		if (RotatingHorizontally())
		{
			const double rotTime = std::chrono::duration<double>(curTime - startRotationYtime).count();
			if (RotatingLeft())
			{
				rotationY = startRotationY - rotationSpeed * rotTime;
				if (rotationY < targetRotationY) rotationY = targetRotationY;
			}
			else
			{
				rotationY = startRotationY + rotationSpeed * rotTime;
				if (rotationY > targetRotationY) rotationY = targetRotationY;
			}
		}

		if (RotatingVertically())
		{
			const double rotTime = std::chrono::duration<double>(curTime - startRotationXtime).count();
			if (RotatingUp())
			{
				rotationX = startRotationX + rotationSpeed * rotTime;
				if (rotationX > targetRotationX) rotationX = targetRotationX;
			}
			else
			{
				rotationX = startRotationX - rotationSpeed * rotTime;
				if (rotationX < targetRotationX) rotationX = targetRotationX;
			}
		}
	}
}
