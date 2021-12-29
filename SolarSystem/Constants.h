#pragma once

const double M_PI = 3.1415926535897932384626433832795028841971693993751;
const double nearPlaneDistance = 0.1;
const double farPlaneDistance = 4000.;
const double rotateAngleStep = 0.5 * M_PI / 180.;
const double translateDistStep = 2.;
const double cameraAngle = 45. * M_PI / 180.;
const double skyboxAngle = 45. * M_PI / 180.; // can be different than for the scene, for some 'interesting' effects
const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

// from AU = astronomical unit = 149597870700 m

const double AGLU = (149597870700. / 100.);


const unsigned int msFrame = 17;




