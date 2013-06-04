// Class which compute the daylight sky color
// Fast implementation of the algorithm from the article
// "A Practical Analytic Model for Daylight" by A. J. Preetham, Peter Shirley and Brian Smits.

#ifndef _SKYLIGHT_HPP_
#define _SKYLIGHT_HPP_

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <math.h>
//#include <QDebug>

inline float fastAcos(float x)
{
	return float(M_PI_2) - (x + x*x*x * (1.f/6.f + x*x * (3.f/40.f + 5.f/112.f * x*x)) );
}

inline float fastExp(float x)
{
	return (x>=0)?
		(1.f + x*(1.f+ x/2.f*(1.f+ x/3.f*(1.f+x/4.f*(1.f+x/5.f))))):
	1.f / (1.f -x*(1.f -x/2.f*(1.f- x/3.f*(1.f-x/4.f*(1.f-x/5.f)))));
}

typedef struct {
	float zenithAngle;
	float distSun;
	float color[3];
} skylightStruct;

typedef struct {
	float pos[3];
	float color[3];
} skylightStruct2;

class Skylight
{
public:
	Skylight();
	virtual ~Skylight();
	
	void setParams(float sunZenithAngle, float turbidity);// not use!!
	void setParamsv(const float * sunPos, float turbidity);
	inline void getZenithColor(float * v) const;
	
	// uncomment for only sky color
	void getxyYValuev(skylightStruct2& p) const
	{
		const float cosDistSun = sunPos[0]*p.pos[0] + sunPos[1]*p.pos[1] + sunPos[2]*p.pos[2];
		const float distSun = fastAcos(cosDistSun);
		const float cosDistSun_q = cosDistSun*cosDistSun;

		const float oneOverCosZenithAngle = (p.pos[1]==0.f) ? (float)1e99 : 1.f / p.pos[1];
		p.color[0] = term_x * (1.f + Ax * fastExp(Bx*oneOverCosZenithAngle))
				* (1.f + Cx * fastExp(Dx*distSun) + Ex * cosDistSun_q);

		p.color[1] = term_y * (1.f + Ay * fastExp(By*oneOverCosZenithAngle))
				* (1.f + Cy * fastExp(Dy*distSun) + Ey * cosDistSun_q);

 		p.color[2] = term_Y * (1.f + AY * fastExp(BY*oneOverCosZenithAngle))
 				* (1.f + CY * fastExp(DY*distSun) + EY * cosDistSun_q);


		if (p.color[2] < 0. || p.color[0] < 0. || p.color[1] < 0.)
		{
			p.color[0] = 0.25;
			p.color[1] = 0.25;
			p.color[2] = 0.;
		}
	}

	void getShadersParams(/*Vec3f&*/float* asunPos, float& aterm_x, float& aAx, float& aBx, float& aCx, float& aDx, float& aEx,
		float& aterm_y, float& aAy, float& aBy, float& aCy, float& aDy, float& aEy) const
	{
		//asunPos=sunPos;
		//memcpy(asunPos,&sunPos,4*3);
		if (asunPos){
			asunPos[0]=sunPos[0];
			asunPos[1]=sunPos[1];
			asunPos[2]=sunPos[2];
		}
		aterm_x=term_x;aAx=Ax;aBx=Bx;aCx=Cx;aDx=Dx;aEx=Ex;
		aterm_y=term_y;aAy=Ay;aBy=By;aCy=Cy;aDy=Dy;aEy=Ey;
	}

	void getShadersParams(float& aterm_x, float& aAx, float& aBx, float& aCx, float& aDx, float& aEx,
		float& aterm_y, float& aAy, float& aBy, float& aCy, float& aDy, float& aEy,
		float& aterm_Y, float& aAY, float& aBY, float& aCY, float& aDY, float& aEY) const
	{		
		aterm_x=term_x;aAx=Ax;aBx=Bx;aCx=Cx;aDx=Dx;aEx=Ex;
		aterm_y=term_y;aAy=Ay;aBy=By;aCy=Cy;aDy=Dy;aEy=Ey;
		aterm_Y=term_Y;aAY=AY;aBY=BY;aCY=CY;aDY=DY;aEY=EY;
	}
	
	
	
private:
	float thetas;
	float T;       // Turbidity : i.e. sky "clarity"
	               //  1 : pure air
	               //  2 : exceptionnally clear
	               //  4 : clear
	               //  8 : light haze
	               // 25 : haze
	               // 64 : thin fog

	float zenithLuminance;
	float zenithColorX;
	float zenithColorY;

	float eyeLumConversion;

	float AY, BY, CY, DY, EY;
	float Ax, Bx, Cx, Dx, Ex;
	float Ay, By, Cy, Dy, Ey;

	float term_x;
	float term_y;
	float term_Y;

	float sunPos[3];

	inline void computeZenithLuminance(void);
	inline void computeZenithColor(void);
	inline void computeLuminanceDistributionCoefs(void);
	inline void computeColorDistributionCoefs(void);
};

inline void Skylight::getZenithColor(float * v) const
{
	v[0] = zenithColorX;
	v[1] = zenithColorY;
	v[2] = zenithLuminance;
}

inline void Skylight::computeZenithLuminance(void)
{
	zenithLuminance = 1000.f * ((4.0453f*T - 4.9710f) * std::tan( (0.4444f - T/120.f) * (float(M_PI)-2.f*thetas) ) -
		0.2155f*T + 2.4192f);
	if (zenithLuminance<=0.f) zenithLuminance=0.00000000001f;
}

inline void Skylight::computeZenithColor(void)
{
	static float thetas2;
	static float thetas3;
	static float T2;

	thetas2 = thetas * thetas;
	thetas3 = thetas2 * thetas;
	T2 = T * T;

	zenithColorX = ( 0.00216f*thetas3 - 0.00375f*thetas2 + 0.00209f*thetas) * T2 +
	               (-0.02903f*thetas3 + 0.06377f*thetas2 - 0.03202f*thetas + 0.00394f) * T +
	               ( 0.10169f*thetas3 - 0.21196f*thetas2 + 0.06052f*thetas + 0.25886f);

	zenithColorY = ( 0.00275f*thetas3 - 0.00610f*thetas2 + 0.00317f*thetas) * T2 +
	               (-0.04214f*thetas3 + 0.08970f*thetas2 - 0.04153f*thetas + 0.00516f) * T +
	               ( 0.14535f*thetas3 - 0.26756f*thetas2 + 0.06670f*thetas + 0.26688f);

}

inline void Skylight::computeLuminanceDistributionCoefs(void)
{
	AY = 0.2787f*T - 1.0630f;
	BY =-0.3554f*T + 0.4275f;
	CY =-0.0227f*T + 6.3251f;
	DY = 0.1206f*T - 2.5771f;
	EY =-0.0670f*T + 0.3703f;
	// with BY>0 the formulas in getxyYValuev make no sense
}

inline void Skylight::computeColorDistributionCoefs(void)
{
	Ax =-0.0148f*T - 0.1703f;
	Bx =-0.0664f*T + 0.0011f;
	Cx =-0.0005f*T + 0.2127f;
	Dx =-0.0641f*T - 0.8992f;
	Ex =-0.0035f*T + 0.0453f;

	Ay =-0.0131f*T - 0.2498f;
	By =-0.0951f*T + 0.0092f;
	Cy =-0.0082f*T + 0.2404f;
	Dy =-0.0438f*T - 1.0539f;
	Ey =-0.0109f*T + 0.0531f;
	// with Bx,By>0 the formulas in getxyYValuev make no sense
}


#endif // _SKYLIGHT_H_

