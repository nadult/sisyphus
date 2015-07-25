/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#include "base.h"
#include <mutex>

namespace res {

namespace {

	ResourceManager<Model, XMLLoader<Model>> s_models("data/models/", ".model", "model");
	ResourceManager<DTexture> s_textures("data/textures/", "");
	ResourceManager<Font> s_fonts("data/", ".fnt");
}

ResourceManager<Model, XMLLoader<Model>> &models() { return s_models; }
ResourceManager<DTexture> &textures() { return s_textures; }
ResourceManager<Font> &fonts() { return s_fonts; }
}

namespace {
vector<pair<string, LogType>> s_buffer;
std::mutex s_buffer_mutex;
}

void logText(const string &text, LogType log_type) {
	std::lock_guard<std::mutex> lock(s_buffer_mutex);
	s_buffer.emplace_back(text, log_type);
}

vector<pair<string, LogType>> getLogs() {
	std::lock_guard<std::mutex> lock(s_buffer_mutex);
	auto out = std::move(s_buffer);
	s_buffer.clear();
	return out;
}

bool isDebugging() { return true; }

void findPerpendicular(const float3 &v1, float3 &v2, float3 &v3) {
	DASSERT(lengthSq(v1) > constant::epsilon);

	v2 = float3(-v1.y, v1.z, v1.x);
	v3 = cross(v1, v2);
}

float3 perturbVector(const float3 &v1, float rand1, float rand2, float strength) {
	float3 v2, v3;
	findPerpendicular(v1, v2, v3);

	float3 dir =
		normalize(float3(1.0f, (2.0f * rand1 - 1.0f) * strength, (2.0f * rand2 - 1.0f) * strength));
	return v1 * dir.x + v2 * dir.y + v3 * dir.z;
}
