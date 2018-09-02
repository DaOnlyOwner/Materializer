#pragma once

#include "glm/vec2.hpp"

bool IntersectionLine_AABB(const glm::vec2& pos0, const glm::vec2& pos1, const glm::vec2& min, const glm::vec2& max);
bool IntersectionAABB_AABB(const glm::vec2& min0, const glm::vec2& max0, const glm::vec2& min1, const glm::vec2& max1);
