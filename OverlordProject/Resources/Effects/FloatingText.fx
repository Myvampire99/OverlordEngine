float4x4 gWorldViewProj : WorldViewProjection;
float2 gTextureSize;
float4x4 gViewInverse : ViewInverse;
Texture2D gParticleTexture : ParticleTexture;


SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

//STATES
//******
BlendState AlphaBlending 
{     
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
	BlendOp = ADD;
	SrcBlendAlpha = ONE;
	DestBlendAlpha = ZERO;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0f;
};

DepthStencilState DisableDepthWriting
{
	//Enable Depth Rendering
    DepthEnable = TRUE;
	//Disable Depth Writing
    DepthWriteMask = ZERO;
};

RasterizerState BackCulling
{
	CullMode = BACK;
};


//SHADER STRUCTS
//**************
struct VS_DATA
{
    int Channel : TEXCOORD2; //Texture Channel
	float3 Position : POSITION;
	float4 Color: COLOR;
	//float Size: TEXCOORD0;
	//float Rotation: TEXCOORD1;
    float2 TexCoord : TEXCOORD0; //Left-Top Character Texture Coordinate on Texture
    float2 CharSize : TEXCOORD1; //Size of the character (in screenspace)
};

struct GS_DATA
{
	float4 Position : SV_POSITION;
	float2 TexCoord: TEXCOORD0;
	float4 Color : COLOR;
    int Channel : TEXCOORD1; //Channel of the vertex
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
	return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float2 texCoord, float4 col, float2x2 uvRotation,int channel)
{
	//Step 1. Create a GS_DATA object
 //   GS_DATA GSData = (GS_DATA) 0;
	////Step 2. Transform the position using the WVP Matrix and assign it to (GS_DATA object).Position (Keep in mind: float3 -> float4, Homogeneous Coordinates)
 //   GSData.Position = mul(float4(pos, 1.0f), gWorldViewProj);
	////Step 3. Assign texCoord to (GS_DATA object).TexCoord
	//	//This is a little formula to do texture rotation by transforming the texture coordinates (Can cause artifacts)
	//	//texCoord -= float2(0.5f,0.5f);
	//	//texCoord = mul(texCoord, uvRotation);
	//	//texCoord += float2(0.5f,0.5f);
 //   GSData.TexCoord = texCoord;
	////Step 4. Assign color to (GS_DATA object).Color
 //   GSData.Color = col;
 //   GSData.Channel = channel;
	////Step 5. Append (GS_DATA object) to the TriangleStream parameter (TriangleStream::Append(...))
 //   triStream.Append(GSData);




    GS_DATA geomData = (GS_DATA) 0;
    geomData.Position = mul(float4(pos, 1.0f), gWorldViewProj);
    geomData.Color = col;
    geomData.TexCoord = texCoord;
    geomData.Channel = channel;
    triStream.Append(geomData);



	
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	//Use these variable names
	float3 topLeft, topRight, bottomLeft, bottomRight;
	//float size = vertex[0].Size;
	float3 origin = vertex[0].Position;

    float sizeX = vertex[0].CharSize.x* 1.f;
    float sizeY = vertex[0].CharSize.y *1.f;

	//Vertices (Keep in mind that 'origin' contains the center of the quad
	//topLeft = ...
    topLeft = origin + float3(-sizeX / 2.f, sizeY / 2.f, 0);
	//topRight = ...
    topRight = origin + float3(sizeX / 2.f, sizeY / 2.f, 0);
	//bottomLeft = ...
    bottomLeft = origin + float3(-sizeX / 2.f, -sizeY / 2.f, 0);
	//bottomRight = ...
    bottomRight = origin + float3(sizeX / 2.f, -sizeY / 2.f, 0);

	//Transform the vertices using the ViewInverse (Rotational Part Only!!! (~ normal transformation)), this will force them to always point towards the camera (cfr. BillBoarding)
	//topLeft = ...
    topLeft = mul(topLeft, (float3x3) gViewInverse);
	//topRight = ...
    topRight =  mul(topRight, (float3x3) gViewInverse);
	//bottomLeft = ...
    bottomLeft = mul(bottomLeft, (float3x3) gViewInverse);
	//bottomRight = ...
    bottomRight = mul(bottomRight, (float3x3) gViewInverse);

	//This is the 2x2 rotation matrix we need to transform our TextureCoordinates (Texture Rotation)
    float2x2 uvRotation = { 0.0f,0.0f,0.0f,0.0f}; //{cos(vertex[0].Rotation), - sin(vertex[0].Rotation), sin(vertex[0].Rotation), cos(vertex[0].Rotation)};
	
	//Create Geometry (Trianglestrip)

	//float2(vertex[0].TexCoord.x, vertex[0].TexCoord.y)
	//float2(vertex[0].TexCoord.x + vertex[0].CharSize.x / gTextureSize.x, vertex[0].TexCoord.y)
	//float2(vertex[0].TexCoord.x, vertex[0].TexCoord.y +  vertex[0].CharSize.y / gTextureSize.y)
	//float2(vertex[0].TexCoord.x + vertex[0].CharSize.x / gTextureSize.x, vertex[0].TexCoord.y + vertex[0].CharSize.y / gTextureSize.y)

    //CreateVertex(triStream, bottomLeft, float2(vertex[0].TexCoord.x, vertex[0].TexCoord.y), vertex[0].Color, uvRotation, vertex[0].Channel);
    //CreateVertex(triStream, topLeft, float2(vertex[0].TexCoord.x + vertex[0].CharSize.x / gTextureSize.x, vertex[0].TexCoord.y), vertex[0].Color, uvRotation, vertex[0].Channel);
    //CreateVertex(triStream, bottomRight, float2(vertex[0].TexCoord.x, vertex[0].TexCoord.y + vertex[0].CharSize.y / gTextureSize.y), vertex[0].Color, uvRotation, vertex[0].Channel);
    //CreateVertex(triStream, topRight, float2(vertex[0].TexCoord.x + vertex[0].CharSize.x / gTextureSize.x, vertex[0].TexCoord.y + vertex[0].CharSize.y / gTextureSize.y), vertex[0].Color, uvRotation, vertex[0].Channel);

    CreateVertex(triStream, bottomLeft, float2(vertex[0].TexCoord.x, vertex[0].TexCoord.y + vertex[0].CharSize.y / gTextureSize.y), vertex[0].Color, uvRotation, vertex[0].Channel);
    CreateVertex(triStream, topLeft, float2(vertex[0].TexCoord.x, vertex[0].TexCoord.y), vertex[0].Color, uvRotation, vertex[0].Channel);
    CreateVertex(triStream, bottomRight, float2(vertex[0].TexCoord.x + vertex[0].CharSize.x / gTextureSize.x, vertex[0].TexCoord.y + vertex[0].CharSize.y / gTextureSize.y), vertex[0].Color, uvRotation, vertex[0].Channel);
    CreateVertex(triStream, topRight, float2(vertex[0].TexCoord.x + vertex[0].CharSize.x / gTextureSize.x, vertex[0].TexCoord.y), vertex[0].Color, uvRotation, vertex[0].Channel);

    //CreateVertex(triStream, bottomLeft, float2(0, 1), vertex[0].Color, uvRotation, vertex[0].Channel);
    //CreateVertex(triStream, topLeft, float2(0, 0), vertex[0].Color, uvRotation, vertex[0].Channel);
    //CreateVertex(triStream, bottomRight, float2(1, 1), vertex[0].Color, uvRotation, vertex[0].Channel);
    //CreateVertex(triStream, topRight, float2(1, 0), vertex[0].Color, uvRotation, vertex[0].Channel);
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET {
	
	//Simple Texture Sampling
    return gParticleTexture.Sample(samPoint, input.TexCoord)[input.Channel] * input.Color;
}

// Default Technique
technique10 Default {

	pass p0 {
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
		
		SetRasterizerState(BackCulling);       
		SetDepthStencilState(DisableDepthWriting, 0);
        SetBlendState(AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
	}
}
