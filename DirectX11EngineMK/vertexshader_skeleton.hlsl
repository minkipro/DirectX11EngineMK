#pragma pack_matrix(row_major)

cbuffer mycBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 bone_transforms[100];
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float4 inBoneIdA : BONEIDA;
    float4 inBoneIdB : BONEIDB;
    float4 inBoneWeightsA : BONEWEIGHTA;
    float4 inBoneWeightsB : BONEWEIGHTB;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WORLD_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    float4x4 boneTransform = { 1.0f, 0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f };
    if (input.inBoneWeightsA.x != 0)
    {
        boneTransform = float4x4(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        boneTransform += mul(bone_transforms[(int) (input.inBoneIdA.x)], input.inBoneWeightsA.x);
        boneTransform += mul(bone_transforms[(int) (input.inBoneIdA.y)], input.inBoneWeightsA.y);
        boneTransform += mul(bone_transforms[(int) (input.inBoneIdA.z)], input.inBoneWeightsA.z);
        boneTransform += mul(bone_transforms[(int) (input.inBoneIdA.w)], input.inBoneWeightsA.w);
        boneTransform += mul(bone_transforms[(int) (input.inBoneIdB.x)], input.inBoneWeightsB.x);
        boneTransform += mul(bone_transforms[(int) (input.inBoneIdB.y)], input.inBoneWeightsB.y);
        boneTransform += mul(bone_transforms[(int) (input.inBoneIdB.z)], input.inBoneWeightsB.z);
        boneTransform += mul(bone_transforms[(int) (input.inBoneIdB.w)], input.inBoneWeightsB.w);
    }
   
    
    float4 pos = mul(float4(input.inPos, 1.0f), boneTransform);
    output.outPosition = mul(pos, wvpMatrix);
    output.outTexCoord = input.inTexCoord;
    output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), mul(worldMatrix, boneTransform)));
    output.outWorldPos = mul(pos, worldMatrix);
    return output;
}