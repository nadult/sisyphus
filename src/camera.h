/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#ifndef CAMERA_H
#define CAMERA_H

#include "base.h"

class Camera {
  public:
	Camera(const float3 &pos, const float3 &target_up_vector, const float3 &target, float fov_in_radians,
		   float aspect_ratio, float min_z, float max_z);
	Camera();

	const Matrix4 viewMatrix() const;
	const Matrix4 projectionMatrix() const;
	const Matrix4 matrix() const;
	const Frustum frustum() const;

	const float3 &pos() const { return m_pos; }
	void setPos(const float3 &new_pos) { m_pos = new_pos; }
	
	const float3 &targetUp() const { return m_target_up; }
	void setTargetUp(const float3 &new_up) { m_target_up = new_up; }

	const float3 &target() const { return m_target; }
	void setTarget(const float3 &new_target) { m_target = new_target; }

	float fov() const { return m_fov; }
	void setFov(float new_fov) { m_fov = new_fov; }

	float aspectRatio() const { return m_aspect_ratio; }
	void setAspectRatio(float new_ratio) { m_aspect_ratio = new_ratio; }

	float minZ() const { return m_min_z; }
	float maxZ() const { return m_max_z; }
	void setZRange(float min, float max) {
		m_min_z = min;
		m_max_z = max;
	}

	float3 up() const;
	float3 forward() const;
	float3 right() const;

	Segment makeRay(const float2 &screen_pos) const;

  private:
	float3 m_pos, m_target;
	float3 m_target_up;

	float m_fov, m_aspect_ratio;
	float m_min_z, m_max_z;
};

#endif
