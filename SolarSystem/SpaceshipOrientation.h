#pragma once

// this is for adjusting the spaceship rotation while rotating the camera or moving sideways
// not implemented yet

class SpaceshipOrientation
{
public:
	double rotationX = 0;
	double rotationY = 0;
	double rotationZ = 0;

	double targetRotationX = 0;
	double targetRotationY = 0;
	double targetRotationZ = 0;

	void Tick();

	void RotateUp();
	void RotateDown();
	void RotateLeft();
	void RotateRight();
};

