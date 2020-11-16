struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    if (sampleColor.a < 1.0f)
    {
        sampleColor.a = 0.0f;
    }
    return sampleColor;
}