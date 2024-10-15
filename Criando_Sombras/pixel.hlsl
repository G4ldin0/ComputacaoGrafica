cbuffer cbPerObject : register(b0)
{
    float4x4 world;
    float4x4 viewProj;
    //float3 cameraPos;
    //float _blank;
    float3 lightDirection;
    float4 lightAmbient;
    float lightStrenght;
    float3 eyePos;
}


struct pixelIn
{
    float4 posH : SV_POSITION;
    float4 posW : POSITION;
    float4 Color : COLOR;
    float3 normal : NORMAL;
};


float3 fresnel(float3 R0, float theta)
{
    float cosIncidentAngle = saturate(theta);
    float t = 1.0f - cosIncidentAngle;
    float3 reflectance = R0 + (1.0f - R0) * (t * t * t * t * t);
    
    return reflectance;
}


float4 main(pixelIn pIn) : SV_TARGET
{
    // renormalização de acordo com o mundo para calculo de direções
    float3 normal = normalize(pIn.normal);
    
    // intensidade da luz
    float3 lambert = max(dot(lightDirection, normal), 0.1f);
    float4 light = float4(lightStrenght * lambert, 1.0f);
    
    float4 DIFUSE = pIn.Color * light;
    
    
    
    
    //float3 material = float3(1.0f, 0.71f, 0.29f);
    float3 material = float3(2.0f, 2.0f, 2.0f);
    
    
    const float m = 16.0f;
    
    // vetor normal entre luz e camera
    float3 v = normalize(eyePos - pIn.posW.xyz);
    float3 h = normalize(v + lightDirection);
    
    float3 fresnelFactor = fresnel(material, dot(h, lightDirection));
    float roughtness = (m + 8.0f) * pow(max(dot(normal, h), 0.0f), m) / 8.0f;
    
    float4 SPECULAR = float4(roughtness * fresnelFactor, 1.0f) * light;
    //SPECULAR = SPECULAR / (SPECULAR + 1.0f);
    
    return (lightAmbient * pIn.Color) + DIFUSE + SPECULAR;
}