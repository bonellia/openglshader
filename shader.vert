#version 330 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TextureCoordinate;

uniform float HeightFactor;
uniform float TextureHorizontalShift;
// Texture-related data
uniform sampler2D HeightMap;
uniform sampler2D rgbTexture;
uniform int TextureHeight;
uniform int TextureWidth;

// Data from CPU
uniform mat4 MVPMatrix; // ModelViewProjection Matrix
// It was stated in the forum that we don't really need MV.
uniform vec3 CameraPosition;
uniform vec3 LightPosition;

// Output to Fragment Shader
out vec2 VertexTextureCoordinate; // For texture-color
out vec3 ToCamera; // Vector from Vertex to Camera;
out vec3 ToLight; // Vector from Vertex to Light;
out vec3 VertexNormal; // For Lighting computation

vec3 ComputeNeighbor(in vec3 Offset)
{
    // compute normal vector using also the heights of neighbor vertices
    vec3 NeighborPosition = Position + Offset;
    vec2 NeighborTextureCoordinate = vec2(-NeighborPosition.x + TextureHorizontalShift / TextureWidth,
                                          -NeighborPosition.z);
    vec4 NeighborHeightMapColor = texture(HeightMap, NeighborTextureCoordinate);
    float Height = HeightFactor * NeighborHeightMapColor.r;
    NeighborPosition.y = Height;
    return NeighborPosition;
}

vec3 ComputeNormal(in float Height)
{
    vec3 Current = vec3(Position.x, Height, Position.z);
    vec3 Left = ComputeNeighbor(vec3(-1, 0, 0));
    vec3 Right = ComputeNeighbor(vec3(1, 0, 0));
    vec3 Top = ComputeNeighbor(vec3(0, 0, 1));
    vec3 Bottom = ComputeNeighbor(vec3(0, 0, -1));
    vec3 TopRight = ComputeNeighbor(vec3(1, 0, 1));
    vec3 BottomLeft = ComputeNeighbor(vec3(-1, 0, -1));

    vec3 ToLeft = Left - Current;
    vec3 ToRight = Right - Current;
    vec3 ToTop = Top - Current;
    vec3 ToBottom = Bottom - Current;
    vec3 ToTopRight = TopRight - Current;
    vec3 ToBottomLeft = BottomLeft - Current;

    vec3 Norm1 = -cross(ToTop, ToLeft);
    vec3 Norm2 = -cross(ToTopRight, ToTop);
    vec3 Norm3 = -cross(ToRight, ToTopRight);
    vec3 Norm4 = -cross(ToBottom, ToRight);
    vec3 Norm5 = -cross(ToBottomLeft, ToBottom);
    vec3 Norm6 = -cross(ToLeft, ToBottomLeft);

    vec3 Norm = (Norm1 + Norm2 + Norm3 + Norm4 + Norm5 + Norm6) / 6.0f;
    return normalize(Norm);
}

void main()
{
    // get texture value, compute height
    VertexTextureCoordinate = vec2(TextureCoordinate.x + (TextureHorizontalShift / TextureWidth),
                                   TextureCoordinate.y);

    vec4 TextureColor = texture(rgbTexture, VertexTextureCoordinate);
    vec4 HeightMapColor = texture(HeightMap, VertexTextureCoordinate);
    float Height = HeightFactor * HeightMapColor.r;
    vec4 CalculatedPosition = vec4(Position.x, Height, Position.z, 1.0f);

    VertexNormal = ComputeNormal(Height);
    // compute toLight vector vertex coordinate in VCS
    ToLight = normalize(LightPosition - CalculatedPosition.xyz);
    ToCamera = normalize(CameraPosition - CalculatedPosition.xyz);

    // set gl_Position variable correctly to give the transformed vertex position
    gl_Position = MVPMatrix * CalculatedPosition;
}