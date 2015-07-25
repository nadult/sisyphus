/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#ifndef GAME_MODEL_ENTITY_H
#define GAME_MODEL_ENTITY_H

#include "game/entity.h"

namespace game {

struct ModelEntityDesc {
	string name;
	string mesh_name;
	float3 visual_offset;
	float visual_scale;
	float visual_angle;
	bool is_blended;
};

class ModelEntity : public Entity {
  public:
	ModelEntity(const ModelEntityDesc&);
	ModelEntity(const ModelEntity &) = default;

	const string &meshName() const { return m_desc.mesh_name; }
	Matrix4 meshTrans() const;
	Matrix4 invMeshTrans() const;

	FBox localBBox() const override;
	pair<FBox, Matrix4> currentBBox() const;

	// If no joint positions information are present, then
	// skinned mesh will be rendered as simple mesh
	virtual void draw(Renderer &) const override;

	int animCount() const { return m_model->animCount(); }
	float animLength(int id) const { return m_model->anim(id).length(); }
	const string &animName(int id) const { return m_model->anim(id).name(); }
	int findAnim(const string &) const;

	Intersection intersect(const Segment &segment) const override;
	bool isIntersecting(const Frustum &) const override;
	Matrix4 nodeTrans(const string &name) const;

	Mesh toMesh() const;

  protected:
	void updateAnimState(const Pose &);
	void updateAnimVerts() const;

	ModelEntityDesc m_desc;
	PModel m_model;
	MaterialSet m_materials;
	PTexture m_texture;

  private:
	Pose m_model_pose;
	FBox m_bounding_box;
	bool m_is_looped;
	bool m_is_finished;
};

}

#endif
