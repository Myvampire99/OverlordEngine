float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.01f;

Texture2D gDiffuseMap;
Texture2D gShadowMap;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;// or Mirror or Clamp or Border
	AddressV = Wrap;// or Mirror or Clamp or Border
};

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

SamplerComparisonState cmpSampler
{
   // sampler state
   Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
   AddressU = MIRROR;
   AddressV = MIRROR;
 
   // sampler comparison state
   ComparisonFunc = LESS_EQUAL;
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 lPos : TEXCOORD1;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

    output.pos = mul(float4(input.pos, 1), gWorldViewProj);
    output.normal = normalize(mul(input.normal, (float3x3) gWorld));
    output.texCoord = input.texCoord;
    output.lPos = mul(float4(input.pos, 1), gWorldViewProj_Light);
	
	//TODO: complete Vertex Shader
	//Hint: Don't forget to project our position to light clip space and store it in lPos
	
	return output;
}

int width = 1280;
int height = 720;

float2 texOffset(int u, int v)
{
	//TODO: return offseted value (our shadow map has the following dimensions: 1280 * 720)
    return float2(u * 1.0f / width, v * 1.0f / height);
}

float EvaluateShadowMap(float4 lpos)
{
    //re-homogenize position after interpolation
    lpos.xyz /= lpos.w;


 
    //if position is not visible to the light - dont illuminate it
    //results in hard light frustum
    if (lpos.x < -1.0f || lpos.x > 1.0f || lpos.y < -1.0f || lpos.y > 1.0f || lpos.z < 0.0f || lpos.z > 1.0f)
        return 0;

    //float2 pos = texOffset(lpos.x, lpos.y);
 
    //transform clip space coords to texture space coords (-1:1 to 0:1)
    lpos.x = lpos.x / 2 + 0.5;
    lpos.y = lpos.y / -2 + 0.5;

    lpos.z -= gShadowMapBias;

    //sample shadow map - point sampler
    //float shadowMapDepth = gShadowMap.Sample(samLinear, lpos.xy).r;
    //float shadowMapDepth = gShadowMap.SampleCmpLevelZero(cmpSampler, lpos.xy, lpos.z);

	//perform PCF filtering on a 4 x 4 texel neighborhood
    int taps = 16;
    float pixels = sqrt(taps); //4
    float blocks = pixels / 2; // 2

    float sum = 0;
    float x, y;
    for (y = -floor(blocks / 2); y <= ceil(blocks / 2); y += 1.0f)
    {
        for (x = -floor(blocks / 2); x <= ceil(blocks / 2); x += 1.0f)
        {
            sum += gShadowMap.SampleCmpLevelZero(cmpSampler, lpos.xy + texOffset(x, y), lpos.z);
        }
    }

    float shadowMapDepth = sum / (float)taps;

    if (shadowMapDepth < lpos.z)
        return shadowMapDepth;

    return 1;
    //if clip space z value greater than shadow map value then pixel is in shadow

 
    //otherwise calculate ilumination at fragmentg
    //float3 lightpos = gWorldViewProj_Light[3].xyz;
    //float3 L = normalize(lightpos - lpos.xyz);
    //float ndotl = dot(normalize(float3(0, 1, 0)), L);

    //return ndotl;

}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float shadowValue = EvaluateShadowMap(input.lPos);

	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	float3 color_rgb= diffuseColor.rgb;
	float color_a = diffuseColor.a;
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;

    return float4(color_rgb * shadowValue, color_a);

}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

