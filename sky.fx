matrix matWorld;
matrix matViewProj;

/// SkyBox_t 
float3 sunPos;
float term_x, Ax, Bx, Cx, Dx, Ex;
float term_y, Ay, By, Cy, Dy, Ey;
float term_Y, AY, BY, CY, DY, EY;

static const float pi = 3.1415926535897931;
static const float alphaWaOverAlphaDa = 1.5051953;
static const float oneOverGamma = 0.45000452;
static const float term2TimesOneOverMaxdLpOneOverGamma = 0.23319134;

struct VS_OUT_SKYBOX_t
{
	float4 Position   : SV_POSITION; 
	float3 color      : COLOR0;    
};

//#define CALC_IN_VERTEX

float3 color_calc(float3 pos)
{
	float3 position = pos;
	position=normalize(position);
	position[1]=clamp(position[1],0,1);
	float cosDistSun = dot(sunPos,position);//sunPos[0]*position[0] + sunPos[1]*position[1] + sunPos[2]*position[2];//dot??
	float distSun = acos(cosDistSun);
	float cosDistSun_q = cosDistSun*cosDistSun;

	float3 tcol;

	float oneOverCosZenithAngle = (position[1]==0.) ? 9999999999999. : 1. / position[1];
	tcol[0] = term_x * (1. + Ax * exp(Bx*oneOverCosZenithAngle))
			* (1. + Cx * exp(Dx*distSun) + Ex * cosDistSun_q);

	tcol[1] = term_y * (1. + Ay * exp(By*oneOverCosZenithAngle))
			* (1. + Cy * exp(Dy*distSun) + Ey * cosDistSun_q);

 	tcol[2] = term_Y * (1. + AY * exp(BY*oneOverCosZenithAngle))
 			* (1. + CY * exp(DY*distSun) + EY * cosDistSun_q);

	tcol[2] = pow(tcol[2]*pi*0.0001, alphaWaOverAlphaDa*oneOverGamma)* term2TimesOneOverMaxdLpOneOverGamma;

	float3 tmp = float3(tcol[0] * tcol[2] / tcol[1], tcol[2], (1. - tcol[0] - tcol[1]) * tcol[2] / tcol[1]);
	const	float3	rCoeffs = float3 ( 2.04148,  -0.564977,-0.344713  );
	const	float3	gCoeffs = float3 (-0.969258,  1.875991, 0.0415557 );
	const	float3	bCoeffs = float3 ( 0.0134455,-0.118373, 1.01527 );
	//vec3 resultSkyColor = vec3(2.04148*tmp.x-0.564977*tmp.y-0.344713*tmp.z, -0.969258*tmp.x+1.87599*tmp.y+0.0415557*tmp.z, 0.0134455*tmp.x-0.118373*tmp.y+1.01527*tmp.z);//, 1.);
	float3 resultSkyColor = float3(dot(rCoeffs,tmp),dot(gCoeffs,tmp),dot(bCoeffs,tmp));
	
	if (cosDistSun>=0.9925f){
		resultSkyColor+=35*(cosDistSun-0.9925f);	
	}
    
	return resultSkyColor;
}

VS_OUT_SKYBOX_t VS_SKYBOX_t(VS_IN IN)
{
	VS_OUT_SKYBOX_t Out=(VS_OUT_SKYBOX_t)0;  
	
	float4x4 wvp = mul(matWorld,matViewProj);
	Out.Position = mul(float4(IN.Position, 1.0f), wvp).xyzw;
#ifdef CALC_IN_VERTEX
	Out.color=color_calc(IN.Position);
#else
    Out.color = IN.Position;
#endif
	        	
	return Out;       
}

PS_Color_Depth PS_SKYBOX_t(VS_OUT_SKYBOX_t IN)
{
	PS_Color_Depth PS_Out;
	
#ifdef CALC_IN_VERTEX
	PS_Out.Color   = float4(IN.color,1);
#else
	PS_Out.Color   = float4(color_calc(IN.color),1);
#endif
	PS_Out.Depth.x = 10000000;	

	return PS_Out;
}

RasterizerState rsCullNone 
{ 
	CullMode = None; 
};

DepthStencilState dssLessEqual   
{ 
	DepthFunc      = LESS_EQUAL;
};

technique10 SkyTech
{
    pass Pass0
    {   
        SetVertexShader( CompileShader( vs_4_0, VS_SKYBOX_t() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_SKYBOX_t() ) );

		SetRasterizerState(rsCullNone);		
		//SetDepthStencilState(dssLessEqual,0);
    } 
}