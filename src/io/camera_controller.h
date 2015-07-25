/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#ifndef IO_CAMERA_CONTROLLER_H
#define IO_CAMERA_CONTROLLER_H

#include "camera.h"

namespace game {
class World;
}

namespace io {

class CameraController {
  public:
	CameraController(game::World *world, const IRect &viewport);
	void handleInput(const GfxDevice &device, float time_diff);
	void tick(double time_diff);

	const Camera &camera() const { return m_camera; }
	Segment screenRay(const float2 &screen_pos) const;
	Frustum screenFrustum(const FRect &rect) const;

	void setCamera(const float3 &source, const float3 &target);

  private:
	float probeHeightMap(float3 pos) const;
	void fixCameraTargets();

	Segment m_cursor;

	IRect m_viewport;
	game::World *m_world;
	Camera m_camera;
	float3 m_target_camera_pos;
	float3 m_target_pos;
};
}

#endif
