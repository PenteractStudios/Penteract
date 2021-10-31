--- vertVarCommon

#define MAX_BONES 100
#define MAX_CASCADES 4

in layout(location=0) vec3 pos;
in layout(location=1) vec3 norm;
in layout(location=2) vec3 tangent;
in layout(location=3) vec2 uvs;
in layout(location=4) uvec4 boneIndices;
in layout(location=5) vec4 boneWeitghts;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform unsigned int shadowStaticCascadesCounter;
uniform unsigned int shadowDynamicCascadesCounter;
uniform unsigned int shadowMainEntitiesCascadesCounter;

uniform mat4 viewOrtoLightsStatic[MAX_CASCADES];
uniform mat4 projOrtoLightsStatic[MAX_CASCADES];

uniform mat4 viewOrtoLightsDynamic[MAX_CASCADES];
uniform mat4 projOrtoLightsDynamic[MAX_CASCADES];

uniform mat4 viewOrtoLightsMainEntities[MAX_CASCADES];
uniform mat4 projOrtoLightsMainEntities[MAX_CASCADES];

out vec3 fragNormal;
out mat3 TBN;
out vec3 fragPos;
out vec2 uv;

out vec4 fragPosLightStatic[MAX_CASCADES];
out vec4 fragPosLightDynamic[MAX_CASCADES];
out vec4 fragPosLightMainEntities[MAX_CASCADES];

out vec3 viewFragPosStatic[MAX_CASCADES];
out vec3 viewFragPosDynamic[MAX_CASCADES];
out vec3 viewFragPosMainEntities[MAX_CASCADES];


uniform mat4 palette[MAX_BONES];
uniform bool hasBones;

--- vertMainCommon

void main()
{
    vec4 position = vec4(pos, 1.0);
    vec4 normal = vec4(norm, 0.0);

    if (hasBones)
    { 
        mat4 skinT = palette[boneIndices[0]] * boneWeitghts[0] + palette[boneIndices[1]] * boneWeitghts[1]
        + palette[boneIndices[2]] * boneWeitghts[2] + palette[boneIndices[3]] * boneWeitghts[3];

        position = skinT * vec4(pos, 1.0);
        normal = skinT * vec4(norm, 0.0);
    }

    gl_Position = proj * view * model * position;
    fragPos = vec3(model * position);
    fragNormal = normalize(transpose(inverse(mat3(model))) * normal.xyz);
    uv = uvs;
    
    for (unsigned int i = 0; i < shadowStaticCascadesCounter; ++i) {
        // Static 
        viewFragPosStatic[i] = (viewOrtoLightsStatic[i] * model * position).xyz;

        fragPosLightStatic[i] = projOrtoLightsStatic[i] * viewOrtoLightsStatic[i] * model * position;
        fragPosLightStatic[i] /= fragPosLightStatic[i].w;
        fragPosLightStatic[i].xyz = fragPosLightStatic[i].xyz * 0.5 + 0.5;
    }

    for (unsigned int i = 0; i < shadowDynamicCascadesCounter; ++i) {
        // Dynamic
        viewFragPosDynamic[i] = (viewOrtoLightsDynamic[i] * model * position).xyz;

        fragPosLightDynamic[i] = projOrtoLightsDynamic[i] * viewOrtoLightsDynamic[i] * model * position;
        fragPosLightDynamic[i] /= fragPosLightDynamic[i].w;
        fragPosLightDynamic[i].xyz = fragPosLightDynamic[i].xyz * 0.5 + 0.5;
    }

    for (unsigned int i = 0; i < shadowMainEntitiesCascadesCounter; ++i) {
        // Main Entities
        viewFragPosMainEntities[i] = (viewOrtoLightsMainEntities[i] * model * position).xyz;

        fragPosLightMainEntities[i] = projOrtoLightsMainEntities[i] * viewOrtoLightsMainEntities[i] * model * position;
        fragPosLightMainEntities[i] /= fragPosLightMainEntities[i].w;
        fragPosLightMainEntities[i].xyz = fragPosLightMainEntities[i].xyz * 0.5 + 0.5;
    }
}

--- vertMainNormal

void main()
{
    vec4 position = vec4(pos, 1.0);
    vec4 normal = vec4(norm, 0.0);

    if (hasBones)
    { 
        mat4 skinT = palette[boneIndices[0]] * boneWeitghts[0] + palette[boneIndices[1]] * boneWeitghts[1]
        + palette[boneIndices[2]] * boneWeitghts[2] + palette[boneIndices[3]] * boneWeitghts[3];

        position = skinT * vec4(pos, 1.0);
        normal = skinT * vec4(norm, 0.0);
    }

    vec3 T = normalize(transpose(inverse(mat3(model))) * tangent.xyz);
    vec3 N = normalize(transpose(inverse(mat3(model))) * normal.xyz);
    T = normalize(T - dot(T, N) * N);  // Re-orthogonalize T with respect to N
    vec3 B = cross(T, N); // Cross swapped to point +z 

    gl_Position = proj * view * model * position;
    fragPos = vec3(model * position);
    fragNormal = N;
    TBN = mat3(T, B, N);
    uv = uvs;

    for (unsigned int i = 0; i < shadowStaticCascadesCounter; ++i) {
        // Static 
        viewFragPosStatic[i] = (viewOrtoLightsStatic[i] * model * position).xyz;

        fragPosLightStatic[i] = projOrtoLightsStatic[i] * viewOrtoLightsStatic[i] * model * position;
        fragPosLightStatic[i] /= fragPosLightStatic[i].w;
        fragPosLightStatic[i].xyz = fragPosLightStatic[i].xyz * 0.5 + 0.5;
    }

    for (unsigned int i = 0; i < shadowDynamicCascadesCounter; ++i) {
        // Dynamic
        viewFragPosDynamic[i] = (viewOrtoLightsDynamic[i] * model * position).xyz;

        fragPosLightDynamic[i] = projOrtoLightsDynamic[i] * viewOrtoLightsDynamic[i] * model * position;
        fragPosLightDynamic[i] /= fragPosLightDynamic[i].w;
        fragPosLightDynamic[i].xyz = fragPosLightDynamic[i].xyz * 0.5 + 0.5;
    }

    for (unsigned int i = 0; i < shadowMainEntitiesCascadesCounter; ++i) {
        // Main Entities
        viewFragPosMainEntities[i] = (viewOrtoLightsMainEntities[i] * model * position).xyz;

        fragPosLightMainEntities[i] = projOrtoLightsMainEntities[i] * viewOrtoLightsMainEntities[i] * model * position;
        fragPosLightMainEntities[i] /= fragPosLightMainEntities[i].w;
        fragPosLightMainEntities[i].xyz = fragPosLightMainEntities[i].xyz * 0.5 + 0.5;
    }
}