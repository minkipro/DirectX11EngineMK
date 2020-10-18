#pragma pack_matrix(row_major)

cbuffer mycBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float3 cameraPos;
}

struct VS_INPUT
{
    float3 inPos : POSITION;
    float4 inColor : COLOR;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_Position;
    float4 outColor : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    float defX = input.inPos[0] - cameraPos[0];
    float defZ = input.inPos[2] - cameraPos[2];
    
    output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix);
    float xzLengthSqure = defX * defX + defZ * defZ;
    if (xzLengthSqure > 500000)
    {
        output.outColor[3] = 0.0f;
    }
    else
    {
        output.outColor = input.inColor;
    }
    return output;
}