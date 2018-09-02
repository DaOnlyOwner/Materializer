#pragma once
#include "Cache.h"
#include "Definitions.h"
#include <glm/vec2.hpp>
#include <memory>
#include <vector>

class InputPort;
class BaseNode;

class OutputPort
{
public:
	const std::vector<PortInputHandle>& GetConnections() const { return connections; }
	friend class BaseNode;

private:
	std::vector<PortInputHandle> connections;
	std::shared_ptr<Cache> cache;
	BaseNode* embedded_node;
	handle data_handle = 0;
	unsigned int index;
	SharedTexture stored_image;

public:
	OutputPort()
	{
	} 
	OutputPort(const std::shared_ptr<Cache>& cache_, BaseNode& embedded_node_, unsigned int index_) : cache(cache_),
	                                                                                                  embedded_node(
		                                                                                                  &embedded_node_),
	                                                                                                  index(index_)
	{
	}

	BaseNode& GetEmbeddedNode() const { return *embedded_node; }
	glm::vec2 GetPos() const;

	void StoreData(const SharedTexture& img) { stored_image = img; }

	WeakTexture GetData() const
	{
		return WeakTexture(stored_image);
	}

	void RegisterConnectionTo(const PortInputHandle& port);

	void UnregisterConnection(const PortInputHandle& port);

	void ConnectTo(const PortInputHandle& port);

	void Disconnect(const PortInputHandle& port);

	PortOutputHandle ToPortHandle() const;
};
