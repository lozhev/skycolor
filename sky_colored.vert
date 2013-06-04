// Attributes
attribute vec4 a_position;									// Vertex Position							(x, y, z, w)
attribute vec3 a_color;										// Vertex Color								(r, g, b)

// Uniforms
uniform mat4 u_worldViewProjectionMatrix;					// Matrix to transform a position to clip space.

uniform vec3 u_sunPos;
uniform float term_x, Ax, Bx, Cx, Dx, Ex;
uniform float term_y, Ay, By, Cy, Dy, Ey;
uniform float term_Y, AY, BY, CY, DY, EY;

const float pi = 3.1415926535897931;
const float alphaWaOverAlphaDa = 1.5051953;
const float oneOverGamma = 0.45000452;
const float term2TimesOneOverMaxdLpOneOverGamma = 0.23319134;

// Varyings
varying vec3 v_color;										// Output Vertex color						(r, g, b)

void main()
{
    // Get the vertex position
    vec4 position = a_position;
    
    // Transform position to clip space.a
    gl_Position = u_worldViewProjectionMatrix *  position;
	position=normalize(position);
	//position*=0.985;//1.0825;
	float cosDistSun = dot(u_sunPos,position);//u_sunPos[0]*position[0] + u_sunPos[1]*position[1] + u_sunPos[2]*position[2];
	float distSun = acos(cosDistSun);
	float cosDistSun_q = cosDistSun*cosDistSun;

	vec3 tcol;
	float oneOverCosZenithAngle = (position[1]==0.) ? 9999999999999. : 1. / position[1];
	tcol[0] = term_x * (1. + Ax * exp(Bx*oneOverCosZenithAngle))
			* (1. + Cx * exp(Dx*distSun) + Ex * cosDistSun_q);
	tcol[1] = term_y * (1. + Ay * exp(By*oneOverCosZenithAngle))
			* (1. + Cy * exp(Dy*distSun) + Ey * cosDistSun_q);
 	tcol[2] = term_Y * (1. + AY * exp(BY*oneOverCosZenithAngle))
 			* (1. + CY * exp(DY*distSun) + EY * cosDistSun_q);	

	if (tcol[2] < 0. || tcol[0] < 0. || tcol[1] < 0.)
	{
		tcol[0] = 0.25;
		tcol[1] = 0.25;
		tcol[2] = 0.;
	}

	tcol[2] = pow(tcol[2]*pi*0.0001, alphaWaOverAlphaDa*oneOverGamma)* term2TimesOneOverMaxdLpOneOverGamma;

	vec3 tmp = vec3(tcol[0] * tcol[2] / tcol[1], tcol[2], (1. - tcol[0] - tcol[1]) * tcol[2] / tcol[1]);
	const	vec3	rCoeffs = vec3 ( 2.04148,  -0.564977,-0.344713  );
	const	vec3	gCoeffs = vec3 (-0.969258,  1.875991, 0.0415557 );
	const	vec3	bCoeffs = vec3 ( 0.0134455,-0.118373, 1.01527 );
	//vec3 resultSkyColor = vec3(2.04148*tmp.x-0.564977*tmp.y-0.344713*tmp.z, -0.969258*tmp.x+1.87599*tmp.y+0.0415557*tmp.z, 0.0134455*tmp.x-0.118373*tmp.y+1.01527*tmp.z);//, 1.);
	vec3 resultSkyColor = vec3(dot(rCoeffs,tmp),dot(gCoeffs,tmp),dot(bCoeffs,tmp));	
    
	v_color = resultSkyColor;//color;//a_color;
}
