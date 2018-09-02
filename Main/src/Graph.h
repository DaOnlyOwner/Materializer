#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include "Definitions.h"
#include <set>
#include <glm/detail/type_vec2.hpp>

class Cache;
class BaseNode;
class IOperation;

typedef uint64_t NodeHandle;

class Graph
{
private:
	std::vector<BaseNode> nodes;
	std::unordered_map<NodeHandle, size_t> name_obj_mapper;
	NodeHandle name_counter = 0;

public:
	NodeHandle Create(const SharedNodeInformation& info, const std::shared_ptr<Cache>& cache,
	                  std::unique_ptr<IOperation>&& operation_, const glm::vec2& pos);

	BaseNode& Get(NodeHandle handle);
	void compute_from(NodeHandle handle, std::set<NodeHandle>& computed_nodes);

	void ComputeFrom(NodeHandle handle);

	std::vector<BaseNode>& GetNodes() { return nodes; }
};
