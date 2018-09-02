#include "GeomUtils.h"

// Can also be used for bezier_rect intersection because the curves I use are nearly a straight line.
// TODO
bool IntersectionLine_AABB(const glm::vec2& pos0, const glm::vec2& pos1, const glm::vec2& min, const glm::vec2& max)
{
	return true; // For now until i found a super fast algo
}


bool IntersectionAABB_AABB(const glm::vec2& min0, const glm::vec2& max0, const glm::vec2& min1, const glm::vec2& max1)
{
	/*    
	 * -----------
	 * |1------  |  Here we have to check if max0.y >= min1.y and max1.y <= min0.y, lets check that it doesn't lay on the right: 
	 * | |    |  |  min0.x <= max1.x && max0.x >= min1.x
	 * | | 0  |  |
	 * | |    |  |
	 * --|----|---
	 *   ------
	 */
	return max0.y >= min1.y && min0.y <= max1.y && min0.x <= max1.x && max0.x >= min1.x;
}
