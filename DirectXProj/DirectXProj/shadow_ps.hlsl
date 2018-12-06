
Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);
Texture2D depthMapTexture2 : register(t2);
Texture2D depthMapTexture3 : register(t3);

SamplerState diffuseSampler  : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
	float4 ambient[3];
	float4 diffuse[3];
	float4 direction[3];
	float4 spotPosition;
	float spotLightAngle;
	float3 padding;
};

cbuffer AttenuationBuffer : register(b1)
{
	float constantFactor;
	float linearFactor;
	float quadraticFactor;
	float pad;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float4 lightViewPos[3] : TEXCOORD1;
	float3 worldPosition : TEXCOORD4;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

// Calculate the cone for the spotlight depending on defined angle
float calculateSpotlightCone(float4 dir, float3 lVector, float spotAngle_)
{
	float minCos = cos(radians(spotAngle_));
	float maxCos = (minCos + 1.0f) / 2.0f;
	float cosAngle = dot(direction[2].xyz, -lVector);
	return smoothstep(minCos, maxCos, cosAngle);
}

float4 main(InputType input) : SV_TARGET
{
    float depthValue;
    float lightDepthValue;
    float shadowMapBias = 0.01f;
    float4 colour = float4(0.f, 0.f, 0.f, 1.f);
	float4 spotLightColour;
    float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);
    bool isLit = false;

	// Calculate the projected texture coordinates.
    for (int i = 0; i < 3; i++)
    {
        float2 pTexCoord = input.lightViewPos[i].xy / input.lightViewPos[i].w;
        pTexCoord *= float2(0.5, -0.5);
        pTexCoord += float2(0.5f, 0.5f);
		saturate(pTexCoord);

        // Determine if the projected coordinates are in the 0 to 1 range.  If not don't do lighting.
        if (!(pTexCoord.x < 0.f || pTexCoord.x > 1.f || pTexCoord.y < 0.f || pTexCoord.y > 1.f))
        {
            // Sample the shadow map (get depth of geometry)
            if (i == 0)
            {
                depthValue = depthMapTexture.Sample(shadowSampler, pTexCoord).r;
            }
            else if (i == 1)
            {
                depthValue = depthMapTexture2.Sample(shadowSampler, pTexCoord).r;
            }
			else if (i == 2)
			{
				depthValue = depthMapTexture3.Sample(shadowSampler, pTexCoord).r;
			}
	        // Calculate the depth from the light.
            lightDepthValue = input.lightViewPos[i].z / input.lightViewPos[i].w;
            lightDepthValue -= shadowMapBias;

	        // Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
            if (lightDepthValue < depthValue)
            {
				if (i < 2)
				{
					colour += calculateLighting(-direction[i].xyz, input.normal, diffuse[i]);
				}
				else
				{
					// Light calculations for spot light
					float3 spotLightVector = spotPosition.xyz - input.worldPosition;
					float dist = length(spotLightVector);
					float spotLightAttenuation = 1 / (constantFactor + (linearFactor * dist) + (quadraticFactor * pow(dist, 2)));

					spotLightVector = normalize(spotLightVector);
					spotLightColour = (calculateLighting(spotLightVector, input.normal, diffuse[2]) * spotLightAttenuation);
					float spotIntensity = calculateSpotlightCone(direction[2], spotLightVector, spotLightAngle);
					spotLightColour *= spotIntensity;
				}
            }
            isLit = true;
        }
    }

	if (!isLit)
	{
		for (int i = 0; i < 3; i++)
		{
			if (i < 2)
			{
				colour += calculateLighting(-direction[i].xyz, input.normal, diffuse[i]);
			}
			else
			{
				// Light calculations for spot light
				float3 spotLightVector = spotPosition.xyz - input.worldPosition;
				float dist = length(spotLightVector);
				float spotLightAttenuation = 1 / (constantFactor + (linearFactor * dist) + (quadraticFactor * pow(dist, 2)));

				spotLightVector = normalize(spotLightVector);
				spotLightColour = (calculateLighting(spotLightVector, input.normal, diffuse[2]) * spotLightAttenuation);
				float spotIntensity = calculateSpotlightCone(direction[2], spotLightVector, spotLightAngle);
				spotLightColour *= spotIntensity;
			}
		}
	}

	//colour = saturate(colour + ambient[0]);
	return saturate((colour + spotLightColour) * textureColour);
}