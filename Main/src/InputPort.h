#pragma once

#include "Definitions.h"
#include <glm/vec2.hpp>

class OutputPort;
class BaseNode;
class Texture;

class InputPort
{
public:
	friend class BaseNode;
	glm::vec2 GetPos() const;

private:
	PortOutputHandle connection;
	BaseNode* embedded_node;
	unsigned int index;
	WeakTexture inputData;

public:
	InputPort()
	{
	} // TODO: Required from std::vector, is there anything better?
	InputPort(BaseNode& node, int index_) : embedded_node(&node), index(index_)
	{
	}

	void InputPort::StoreData(const WeakTexture& data)
	{
		inputData = data;
	}
	
	BaseNode& GetEmbeddedNode() const { return *embedded_node; }

	bool IsInputOptional() const;

	void RegisterConnectionFrom(const PortOutputHandle& port);

	void UnregisterConnection();

	void ConnectFrom(const PortOutputHandle& port);
	void Disconnect();

	PortOutputHandle GetConnection() const { return connection; } 

	PortInputHandle ToPortHandle() const;

	WeakTexture GetInputData() const { return inputData; }

	bool IsInputDataValid() const { return inputData.expired(); }


private:
	bool validate() { return connection.validate(); }
};
