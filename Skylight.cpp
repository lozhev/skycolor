#include "Skylight.hpp"

#include <cmath>
#include <cstdlib>

Skylight::Skylight() : thetas(0.f), T(0.f)
{
}

Skylight::~Skylight()
{
}


void Skylight::setParams(float _sunZenithAngle, float _turbidity)
{
	thetas = _sunZenithAngle;
	T = _turbidity;

	computeZenithLuminance();
	computeZenithColor();
	computeLuminanceDistributionCoefs();
	computeColorDistributionCoefs();

	float cos_thetas = std::cos(thetas);
	term_x = zenithColorX   / ((1.f + Ax * std::exp(Bx)) * (1.f + Cx * std::exp(Dx*thetas) + Ex * cos_thetas * cos_thetas));
	term_y = zenithColorY   / ((1.f + Ay * std::exp(By)) * (1.f + Cy * std::exp(Dy*thetas) + Ey * cos_thetas * cos_thetas));
	term_Y = zenithLuminance / ((1.f + AY * std::exp(BY)) * (1.f + CY * std::exp(DY*thetas) + EY * cos_thetas * cos_thetas));

}

void Skylight::setParamsv(const float * _sunPos, float _turbidity)
{
	sunPos[0] = _sunPos[0];
	sunPos[1] = _sunPos[1];
	sunPos[2] = _sunPos[2];

	thetas = M_PI_2 - std::asin((float)sunPos[1]);
	T = _turbidity;

	computeZenithLuminance();
	computeZenithColor();
	computeLuminanceDistributionCoefs();
	computeColorDistributionCoefs();

	float cos_thetas = sunPos[1];
	term_x = zenithColorX   / ((1.f + Ax * std::exp(Bx)) * (1.f + Cx * std::exp(Dx*thetas) + Ex * cos_thetas * cos_thetas));
	term_y = zenithColorY   / ((1.f + Ay * std::exp(By)) * (1.f + Cy * std::exp(Dy*thetas) + Ey * cos_thetas * cos_thetas));
	term_Y = zenithLuminance / ((1.f + AY * std::exp(BY)) * (1.f + CY * std::exp(DY*thetas) + EY * cos_thetas * cos_thetas));
}

