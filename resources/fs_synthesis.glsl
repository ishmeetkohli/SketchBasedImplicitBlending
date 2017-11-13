#version 330

//Input data
in vec3 fragNormal;
in vec3 fragVert;

//Output data
out vec4 fColor;

uniform vec3 lightPosition_worldspace;
uniform vec3 color;

void main()
{
    vec3 LightColor = vec3(1,1,1);
    vec3 surfaceToLight = lightPosition_worldspace - fragVert;
    float brightness = dot(fragNormal, surfaceToLight) / (length(surfaceToLight) * length(fragNormal));
    brightness = clamp(brightness, 0, 1);

    fColor = vec4(brightness * LightColor * color, 1.0);


//    vec3 LightColor = vec3(1,1,1);


//    vec3 MaterialDiffuseColor = color;
//    vec3 MaterialAmbientColor = vec3(0.3,0.3,0.3) * MaterialDiffuseColor;
//    vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

//    float distance = length( lightPosition_worldspace - position_worldspace );

//    vec3 n = normalize(normal_cameraspace);
//    vec3 l = normalize(lightDirection_cameraspace);
//    float cosTheta = clamp( dot( n,-l ), 0,1 );

//    // Eye vector (towards the camera)
//    vec3 E = normalize(eyeDirection_cameraspace);
//    // Direction in which the triangle reflects the light
//    vec3 R = normalize(reflect(-l,n));

//    float cosAlpha = clamp( dot( E,R ), 0,1 );

//    vec3 tempColor =
//            // Ambient Color
//            MaterialAmbientColor +
//            // Diffuse Color
//            MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance * distance) +
//            // Specular : reflective highlight, like a mirror
//            MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance * distance);

//    fColor = vec4(color,1.0);
}
