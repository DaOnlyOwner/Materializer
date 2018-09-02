#include "NodeFactory.h"
#include "Graph.h"

void NodeFactory::Create(const std::string& key, Graph& embedded_graph, const glm::vec2& pos)
{
	auto it = operation_map.find(key);
	if(it != operation_map.end())
	{
		embedded_graph.Create(it->second.info, {}, it->second.operation->Clone(), pos);
	}

	else throw std::runtime_error("Couldn't create node, the given key was not registered");

}
