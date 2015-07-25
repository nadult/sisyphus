/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#ifndef BASE_H
#define BASE_H

#include "fwk.h"

using namespace fwk;

template <class T> using optional = unique_ptr<T>;

namespace res {

ResourceManager<Model, XMLLoader<Model>> &models();
ResourceManager<DTexture> &textures();
ResourceManager<Font> &fonts();
}

enum class LogType {
	per_frame,
	single,
};

void logText(const string &text, LogType log_type = LogType::single);
vector<pair<string, LogType>> getLogs();
bool isDebugging();

class GameRenderer;

void findPerpendicular(const float3 &v1, float3 &v2, float3 &v3);
float3 perturbVector(const float3 &v1, float rand1, float rand2, float strength);

#endif
