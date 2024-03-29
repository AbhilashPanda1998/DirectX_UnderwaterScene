cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

struct VertexShaderInput
{
    float3 pos : POSITION;
    float3 color : COLOR0;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR0;
};

cbuffer timeConstantBuffer : register(b1)
{
    float time;
    float3 padding;
}

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    float3 inPos = input.pos;
    float4 pos = float4(inPos, 1.0f);

    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);

    output.pos = pos;   
    output.color = input.color;
    output.pos.xyz *= 0.20f;
    output.pos.x -= 5;
    output.pos.x += time * 0.25f;
    return output;
}
