#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TileScale;
layout(location = 5) in int a_EntityID;

uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_TexCoord;
out flat float v_TexIndex;
out float v_TileScale;
out flat int v_EntityID;


void main() {
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_TileScale = a_TileScale;
	v_EntityID = a_EntityID;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}


#type fragment
#version 450

layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIndex;
in float v_TileScale;
in flat int v_EntityID;

uniform sampler2D u_Textures[16];

void main() {
	vec4 texture_color = v_Color;

	switch (int(v_TexIndex)) {
		case 0:  texture_color *= texture(u_Textures[0],  v_TexCoord * v_TileScale); break;
		case 1:  texture_color *= texture(u_Textures[1],  v_TexCoord * v_TileScale); break;
		case 2:  texture_color *= texture(u_Textures[2],  v_TexCoord * v_TileScale); break;
		case 3:  texture_color *= texture(u_Textures[3],  v_TexCoord * v_TileScale); break;
		case 4:  texture_color *= texture(u_Textures[4],  v_TexCoord * v_TileScale); break;
		case 5:  texture_color *= texture(u_Textures[5],  v_TexCoord * v_TileScale); break;
		case 6:  texture_color *= texture(u_Textures[6],  v_TexCoord * v_TileScale); break;
		case 7:  texture_color *= texture(u_Textures[7],  v_TexCoord * v_TileScale); break;
		case 8:  texture_color *= texture(u_Textures[8],  v_TexCoord * v_TileScale); break;
		case 9:  texture_color *= texture(u_Textures[9],  v_TexCoord * v_TileScale); break;
		case 10: texture_color *= texture(u_Textures[10], v_TexCoord * v_TileScale); break;
		case 11: texture_color *= texture(u_Textures[11], v_TexCoord * v_TileScale); break;
		case 12: texture_color *= texture(u_Textures[12], v_TexCoord * v_TileScale); break;
		case 13: texture_color *= texture(u_Textures[13], v_TexCoord * v_TileScale); break;
		case 14: texture_color *= texture(u_Textures[14], v_TexCoord * v_TileScale); break;
		case 15: texture_color *= texture(u_Textures[15], v_TexCoord * v_TileScale); break;
	}

	if (texture_color.a == 0.0) {
		discard;
	}

	color = texture_color;
	entityID = v_EntityID;
}