/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
*/

#include "game/model_entity.h"

namespace game {

PProgram meshProgram() {
	static PProgram out;
	if(!out) {
		Shader vertex_shader(Shader::tVertex), fragment_shader(Shader::tFragment);
		Loader("data/mesh_shader.vsh") >> vertex_shader;
		Loader("data/mesh_shader.fsh") >> fragment_shader;
		out = make_cow<Program>(vertex_shader, fragment_shader);
	}

	return out;
}

static PModel makeModel(const ModelEntityDesc &desc) {
	PModel model = res::models()[desc.mesh_name];
	return model;
}

static MaterialSet makeMaterials(PModel model) {
	std::map<string, PMaterial> mats;
	for(const auto &def : model->materialDefs())
		mats[def.name] = make_cow<Material>(meshProgram(), def.diffuse);

	return MaterialSet(make_cow<Material>(), std::move(mats));
}

ModelEntity::ModelEntity(const ModelEntityDesc &desc)
	: m_desc(desc), m_model(makeModel(m_desc)), m_materials(makeMaterials(m_model)) {
	updateAnimState(m_model->animatePose(-1, 0));
	//	XMLDocument doc;
	//	m_model->saveToXML(doc.addChild("mesh"));
	//	Saver(m_desc.mesh_name + ".mesh") << doc;
}

void ModelEntity::updateAnimState(const Pose &new_pose) {
	m_model_pose = new_pose;
	m_bounding_box = m_model->boundingBox(m_model_pose);
	DASSERT(m_model_pose.size() == m_model->nodes().size());
}

void ModelEntity::updateAnimVerts() const {
	/*	if(m_anim_state.need_update) {
			m_anim_state.positions.resize(m_model->vertexCount());
			//	m_anim_state.normals.resize(m_model->vertexCount());
			//	TODO: animate normals too
			m_model->animateVertices(m_anim_state.joints.data(), m_anim_state.positions.data(),
	   nullptr);
			m_anim_state.need_update = false;
		}*/
}

Intersection ModelEntity::intersect(const Segment &segment) const {
	float isect = m_model->intersect(invMeshTrans() * (invTrans() * segment), m_model_pose);
	return isect < constant::inf ? Intersection(this, isect * m_desc.visual_scale)
								 : Intersection();
}

bool ModelEntity::isIntersecting(const Frustum &frustum) const {
	return (invMeshTrans() * (invTrans() * frustum)).isIntersecting(m_bounding_box);
}

FBox ModelEntity::localBBox() const { return meshTrans() * m_bounding_box; }

pair<FBox, Matrix4> ModelEntity::currentBBox() const {
	return make_pair(m_bounding_box, trans() * meshTrans());
}

Matrix4 ModelEntity::meshTrans() const {
	return translation(m_desc.visual_offset) * scaling(m_desc.visual_scale) *
		   ::rotation(float3(0, 1, 0), m_desc.visual_angle);
}

Matrix4 ModelEntity::invMeshTrans() const {
	return ::rotation(float3(0, 1, 0), -m_desc.visual_angle) * scaling(1.0f / m_desc.visual_scale) *
		   translation(-m_desc.visual_offset);
}

void ModelEntity::draw(Renderer &out) const {
	Matrix4 matrix = trans() * meshTrans();

	updateAnimVerts();

	m_model->draw(out, m_model_pose, m_materials, matrix);
	if(isDebugging()) {
		Color color = isDebugging() ? Color(255, 255, 255, 80) : Color::white;
		// out.addBBoxLines(m_model->boundingBox(m_model_pose), Color::yellow, matrix);
		// out.addBBoxLines(bbox(), Color::yellow);
	}
}

int ModelEntity::findAnim(const string &name) const {
	for(int n = 0, count = m_model->animCount(); n < count; n++)
		if(m_model->anim(n).name() == name)
			return n;
	return -1;
}

Matrix4 ModelEntity::nodeTrans(const string &name) const {
	return meshTrans() * m_model->nodeTrans(name, m_model_pose);
}

Mesh ModelEntity::toMesh() const {
	return Mesh::transform(meshTrans(), m_model->toMesh(m_model_pose));
}
}
