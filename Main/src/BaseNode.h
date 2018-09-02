#pragma once
#include "Definitions.h"
#include "IOperation.h" // Required by unique_ptr;
#include "glm/vec2.hpp"
#include <memory>
#include <vector>
#include "Event.h"
//#include "InputPort.h"
//#include "OutputPort.h"

class InputPort;
class OutputPort;
class Texture;
class Cache;
class Graph;
struct NodeInformation;

class BaseNode
{
public:
	const SharedNodeInformation& GetNodeInformation() const { return info; }
	uint64_t GetId() const { return id; }
	std::vector<OutputPort>& GetOutputPorts() { return output_ports; }
	std::vector<InputPort>& GetInputPorts() { return input_ports; }
	SharedTexture& GetReprTexture() { return representing_image; }
	
	glm::vec2 Pos = {0,0};
	std::shared_ptr<Event<const WeakTexture&>> ReprTextureUpdateEvent = std::make_shared<Event<const WeakTexture&>>();


private:
	std::vector<InputPort> input_ports;
	std::vector<OutputPort> output_ports;
	SharedNodeInformation info;
	std::unique_ptr<IOperation> operation;
	uint64_t id;
	Graph* embedded_graph;
	SharedTexture representing_image;

public:
	BaseNode(const SharedNodeInformation& info,
	         const std::shared_ptr<Cache>& cache,
	         std::unique_ptr<IOperation>&& _operation,
	         uint64_t id_,
	         Graph* embedded_graph_);

	BaseNode()
	{
	}

	BaseNode(BaseNode&&);

	//void Compute();
	//void ComputeAndNotify();

	void ConnectToInput(unsigned int output_port_num, NodeHandle conn_to, unsigned int input_port_num);

	void DisconnectFrom(unsigned int output_port_num, NodeHandle conn_to, unsigned int input_port_num);

	OutputPort& Lookup(const PortOutputHandle& handle) const;	
	InputPort& Lookup(const PortInputHandle& handle) const;

	bool IsReadyToCompute() const;
	
	glm::vec2 CalcInputSlotPos(int index) const;
	glm::vec2 CalcOutputSlotPos(int index) const;

	bool Compute();
};
