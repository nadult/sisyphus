#include "sprite.h"

static std::map<string, SpriteDesc> s_descs;

static void loadDescs() {
	if(!s_descs.empty())
		return;

	XMLDocument doc;
	Loader("data/sprites.xml") >> doc;
	XMLNode node = doc.child("sprite");
	while(node) {
		SpriteDesc desc;
		desc.texture_name = node.attrib("texture");
		desc.grid_size = node.attrib<int2>("grid_size");
		desc.anim_length = node.attrib<float>("anim_length");
		ASSERT(desc.grid_size.x >= 1 && desc.grid_size.y >= 1);
		ASSERT(desc.anim_length > 0.0f);

		s_descs[(string)node.attrib("name")] = desc;
		node.next();
	}
}

static const SpriteDesc &findDesc(const string &name) {
	loadDescs();

	auto it = s_descs.find(name);
	if(it == s_descs.end())
		THROW("Cannot find sprite definition: '%s'", name.c_str());

	return it->second;
}

Sprite::Sprite(const string &name) : m_desc(findDesc(name)) {
	PTexture texture = res::textures()[m_desc.texture_name];
	m_material = make_immutable<Material>(texture);
}

void Sprite::draw(Renderer &out, const float3 &pos, const float2 &size, float anim_pos) const {
	int num_frames = m_desc.grid_size.x * m_desc.grid_size.y;
	int frame_id = int((anim_pos / m_desc.anim_length) * (float)num_frames) % num_frames;

	const Matrix4 &view_matrix = inverse(out.viewMatrix());
	float3 right_vec = mulNormal(view_matrix, float3(1, 0, 0));
	float3 up_vec = mulNormal(view_matrix, float3(0, 1, 0));

	float3 corner = pos - (right_vec * size.x + up_vec * size.y) * 0.5f;
	float3 points[4] = {corner, corner + right_vec * size.x, corner + up_vec * size.y,
						corner + right_vec * size.x + up_vec * size.y};

	int tx = frame_id % m_desc.grid_size.x, ty = frame_id / m_desc.grid_size.x;
	float2 inv_tex_size(1.0f / m_desc.grid_size.x, 1.0f / m_desc.grid_size.y);

	float2 tex_coords[4] = {inv_tex_size * float2(tx, ty), inv_tex_size * float2(tx + 1, ty),
							inv_tex_size * float2(tx, ty + 1),
							inv_tex_size * float2(tx + 1, ty + 1)};

	out.addSprite(points, tex_coords, m_material);
}
