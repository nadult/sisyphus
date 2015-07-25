/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#include "io/camera_controller.h"
#include "game/world.h"
#include <algorithm>

using namespace game;

namespace io {

CameraController::CameraController(World *world, const IRect &viewport)
	: m_viewport(viewport), m_world(world) {
	DASSERT(m_world);
	m_camera.setAspectRatio(float(viewport.width()) / viewport.height());
}

void CameraController::setCamera(const float3 &source, const float3 &target) {
	m_target_pos = target;
	m_target_camera_pos = source;
	//fixCameraTargets();
	m_camera.setPos(m_target_camera_pos);
	m_camera.setTarget(m_target_pos);
}

void CameraController::handleInput(const GfxDevice &device, float time_diff) {
	float3 move_dirs[3] = {m_camera.right(), m_camera.forward(), m_camera.targetUp()};
	for(int i = 0; i < 2; i++) {
		move_dirs[i].y = 0.0f;
		move_dirs[i] = normalize(move_dirs[i]);
	}

	float camera_rot = 0.0f;
	float3 camera_move;

	for(const auto &event : device.inputEvents()) {
		if(event.keyPressed('a'))
			camera_move -= move_dirs[0];
		if(event.keyPressed('d'))
			camera_move += move_dirs[0];
		if(event.keyPressed('s'))
			camera_move -= move_dirs[1];
		if(event.keyPressed('w'))
			camera_move += move_dirs[1];
		if(event.keyPressed('r'))
			camera_move += move_dirs[2];
		if(event.keyPressed('f'))
			camera_move -= move_dirs[2];
		if(event.keyPressed('q'))
			camera_rot += 1.0f;
		if(event.keyPressed('e'))
			camera_rot -= 1.0f;
	}

	camera_move *= 15.0f * time_diff;
	camera_rot *= 1.5f * time_diff;

	m_target_camera_pos += camera_move;
	m_target_pos += camera_move;

	if(fabs(camera_rot) > constant::epsilon) {
		float3 rot_vec =
			rotateVector(m_target_camera_pos - m_target_pos, float3(0, 1, 0), camera_rot);
		m_target_camera_pos = m_target_pos + rot_vec;
	}
	//fixCameraTargets();
}

/*
float CameraController::probeHeightMap(float3 pos) const {
	FBox bbox = m_world->heightMap().boundingBox();
	pos.x = clamp(pos.x, bbox.min.x, bbox.max.x - 0.1f);
	pos.z = clamp(pos.z, bbox.min.z, bbox.max.z - 0.1f);
	return m_world->heightMap().getHeight(pos.xz());
}

void CameraController::fixCameraTargets() {
	float cam_height = probeHeightMap(m_target_camera_pos);
	float3 vec = normalize(m_target_pos - m_target_camera_pos);
	m_target_pos = m_target_camera_pos + vec * (m_target_camera_pos.y - cam_height);
	float tgt_height = probeHeightMap(m_target_pos);

	m_target_camera_pos.y -= cam_height;
	m_target_camera_pos.y = max(2.0f, m_target_camera_pos.y);
	m_target_camera_pos.y += cam_height;
	m_target_pos.y = max(tgt_height, cam_height + (m_target_camera_pos.y - cam_height) * 0.5f);

	float dist = distance(m_target_pos, m_target_camera_pos);
	float min_dist = 5.0f;
	if(dist < min_dist)
		m_target_camera_pos -= m_camera.forward() * (min_dist - dist);
}*/

template <class T> static T niceBlend(const T &src, const T &dst) { return lerp(src, dst, 0.05f); }

void CameraController::tick(double time_diff) {
	m_camera.setPos(niceBlend(m_camera.pos(), m_target_camera_pos));
	m_camera.setTarget(niceBlend(m_camera.target(), m_target_pos));
}

Segment CameraController::screenRay(const float2 &screen_pos) const {
	float2 pos = screen_pos - float2(m_viewport.center());
	pos.x = pos.x * 2.0f / m_viewport.width();
	pos.y = -pos.y * 2.0f / m_viewport.height();
	return m_camera.makeRay(pos);
}

Frustum CameraController::screenFrustum(const FRect &rect) const {
	float2 corners[4] = {{rect.min.x, rect.max.y},
						 {rect.min.x, rect.min.y},
						 {rect.max.x, rect.min.y},
						 {rect.max.x, rect.max.y}};
	float3 points[2][4];
	for(int n = 0; n < 4; n++) {
		auto ray = screenRay(float2(corners[n]));
		points[0][n] = ray.at(1.0f);
		points[1][n] = ray.at(10.0f);
	}

	Plane planes[4];
	planes[Frustum::id_left] = Plane(points[0][0], points[1][0], points[0][1]);
	planes[Frustum::id_up] = Plane(points[0][1], points[1][1], points[0][2]);
	planes[Frustum::id_right] = Plane(points[0][2], points[1][2], points[0][3]);
	planes[Frustum::id_down] = Plane(points[0][3], points[1][3], points[0][0]);
	return Frustum(planes);
}
}
