/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#include "camera.h"

Camera::Camera(const float3 &pos, const float3 &target_up_vector, const float3 &target, float fov,
			   float aspect_ratio, float min_z, float max_z)
	: m_pos(pos), m_target(target), m_target_up(target_up_vector), m_fov(fov),
	  m_aspect_ratio(aspect_ratio), m_min_z(min_z), m_max_z(max_z) {}

Camera::Camera()
	: Camera(float3(0, 0, 0), float3(0, 1, 0), float3(1, 0, 0), degToRad(60.0f), 16.0f / 9.0f, 1.0f,
			 10000.0f) {}

const Matrix4 Camera::viewMatrix() const { return fwk::lookAt(m_pos, m_target, up()); }
const Matrix4 Camera::projectionMatrix() const {
	return fwk::perspective(m_fov, m_aspect_ratio, m_min_z, m_max_z);
}
const Matrix4 Camera::matrix() const { return projectionMatrix() * viewMatrix(); }
const Frustum Camera::frustum() const { return Frustum(matrix()); }

float3 Camera::up() const {
	// TODO: what if dir is close to target_up?
	float3 forward = this->forward();
	float3 right = cross(forward, m_target_up);
	//	printf("right: %.2f %.2f %.2f\n", right.x, right.y, right.z);
	//	printf("forw:  %.2f %.2f %.2f\n", forward.x, forward.y, forward.z);
	return cross(right, forward);
}

float3 Camera::forward() const { return normalize(m_target - m_pos); }

float3 Camera::right() const { return cross(forward(), up()); }

Segment Camera::makeRay(const float2 &screen_pos) const {
	Matrix4 inv_proj_mat = inverse(projectionMatrix());
	float3 target = mulPoint(inv_proj_mat, float3(screen_pos, 1.0f));
	Segment out = inverse(viewMatrix()) * Segment(float3(), target);
	return Segment(out, 0.0f, constant::inf);
}
