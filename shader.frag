#version 330 core

// Output Color
out vec4 FragmentColor;

// Data from Vertex Shader
in vec2 VertexTextureCoordinate;
in vec3 ToCamera; // Vector from Vertex to Camera;
in vec3 ToLight; // Vector from Vertex to Light;
in vec3 VertexNormal; // For Lighting computation

uniform float HeightFactor;
uniform float TextureHorizontalShift;
uniform int TextureHeight;
uniform int TextureWidth;
uniform mat4 MVPMatrix; // ModelViewProjection Matrix
// Texture-related data;
uniform sampler2D HeightMap;
uniform sampler2D Texture;
uniform vec3 CameraPosition;
uniform vec3 LightPosition;

void main()
{
    // Assignment Constants below
    vec4 TextureColor = texture(Texture, VertexTextureCoordinate);

    vec4 ka = vec4(0.25, 0.25, 0.25, 1.00); // reflectance coeff. for ambient
    vec4 Ia = vec4(0.3, 0.3, 0.3, 1.0); // light color for ambient
    vec4 Id = vec4(1.0, 1.0, 1.0, 1.0); // light color for diffuse
    vec4 kd = vec4(1.0, 1.0, 1.0, 1.0); // reflectance coeff. for diffuse
    vec4 Is = vec4(1.0, 1.0, 1.0, 1.0); // light color for specular
    vec4 ks = vec4(1.0, 1.0, 1.0, 1.0); // reflectance coeff. for specular
    float specExp = 100; // specular exponent

    // Light components
    // Ambient
    vec3 AmbientComponent = (ka * Ia).xyz;
    float DiffuseCoeff = max(dot(VertexNormal, ToLight), 0.0);
    // Diffuse
    vec3 DiffuseComponent = DiffuseCoeff * (Id * kd).xyz;
    // Reflection
    vec3 ReflectedLight = reflect(-ToLight, VertexNormal);
    float SpecularCoeff = pow(max(dot(ReflectedLight, ToCamera), 0.0), specExp);
    // Specular
    vec3 SpecularComponent = SpecularCoeff * (Is * ks).xyz;

    vec3 TotalComponent = AmbientComponent + DiffuseComponent + SpecularComponent;
    vec3 UnclampedFragmentColor = TextureColor.xyz * TotalComponent;
    // compute the color using the following equation
    FragmentColor = vec4(clamp(UnclampedFragmentColor, 0.0, 1.0), 1.0);
}