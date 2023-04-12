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
    output.pos.xyz *= 0.32f;
    output.pos.y -= 2;
    output.pos.z = output.pos.z - 1;
    output.pos.x = output.pos.x - 8;
    output.pos.x += time * 0.15f;
    return output;
}
