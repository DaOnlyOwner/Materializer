#include "Graph.h"
#include <cassert>
#include "Cache.h"
#include "BaseNode.h"
#include <functional>
#include "OutputPort.h"
#include "InputPort.h"
#include <set>
#include <vector>

NodeHandle Graph::Create(const SharedNodeInformation& info, const std::shared_ptr<Cache>& cache,
                         std::unique_ptr<IOperation>&& operation_, const glm::vec2& pos)
{
	BaseNode p = { info, cache, move(operation_), ++name_counter, this };
	p.Pos = pos; // TODO: Add to constructor
	nodes.push_back(std::move(p));
	name_obj_mapper[name_counter] = nodes.size() - 1;
	return name_counter;
}

BaseNode& Graph::Get(NodeHandle handle)
{
	auto it = name_obj_mapper.find(handle);
	if (it != name_obj_mapper.end())
	{
		return nodes[it->second];
	}
	assert(false && "This shouldn't happen!");
}

void Graph::ComputeFrom(NodeHandle handle)
{
	BaseNode& root = Get(handle);
	std::vector<NodeHandle> dfsStack;
	dfsStack.push_back(handle);
	std::set<NodeHandle> computed_nodes;

	while (!dfsStack.empty())
	{
		auto& node = Get(dfsStack.back());
		/*
		* if(node.IsReadyToCompute()):
		*      compute node;
		*      forward computed data to outputs;
		*      push outputs into the stack if they aren't processed already
		*
		* else
		*      for all inputs i:
		*          if nodeRef(i.providerName) is not being processed and i is required and value not -1: push i.providerName into stack and mark as being processed
		*          if i is -1 and not optional: error ---> return after resetting processed nodes
		*/

		if (node.Compute())
		{
			for (auto& op : node.GetOutputPorts())
			{
				for (auto& ip : op.GetConnections())
				{
					InputPort& receiverPort = node.Lookup(ip);
					receiverPort.StoreData(op.GetData());
					if (computed_nodes.find(receiverPort.GetEmbeddedNode().GetId()) == computed_nodes.end())
					{
						dfsStack.push_back(receiverPort.GetEmbeddedNode().GetId());
					}
				}
			}
		}

		else
		{
			for (InputPort& ip : node.GetInputPorts())
			{ 
				auto& providerNode = node.Lookup(ip.GetConnection());

				if (computed_nodes.find(providerNode.GetEmbeddedNode().GetId()) != computed_nodes.end() 
					&& !ip.IsInputOptional() && ip.GetConnection().validate())
				{
					dfsStack.push_back(ip.GetEmbeddedNode().GetId());
					computed_nodes.insert(ip.GetEmbeddedNode().GetId());
				}

				if (!ip.GetConnection().validate() && !ip.IsInputOptional())
				{
					return; // Error, cannot process node because required input is missing.
				}
			}
		}
		dfsStack.pop_back();
	}
}
