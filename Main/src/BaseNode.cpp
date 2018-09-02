#include "BaseNode.h"
#include "Cache.h"
#include "Graph.h"
#include "IOperation.h"
#include "Texture.h"
#include "InputPort.h"
#include "NodeInformation.h"
#include "OutputPort.h"
#include <algorithm>

BaseNode::BaseNode(const SharedNodeInformation& info,
                   const std::shared_ptr<Cache>& cache,
                   std::unique_ptr<IOperation>&& _operation,
                   uint64_t id_,
                   Graph* embedded_graph_)
	: input_ports(info->InputDescr.size()),
	  output_ports(info->OutputDescr.size()),
	  info(info),
	  operation(move(_operation)),
	  id(id_),
	  embedded_graph(embedded_graph_)
{
	for (unsigned int i = 0; i < info->OutputDescr.size(); i++)
	{
		output_ports[i] = OutputPort(cache, *this, i);
	}

	for (unsigned int i = 0; i < info->InputDescr.size(); i++)
	{
		input_ports[i] = InputPort(*this, i);
	}
}


BaseNode::BaseNode(BaseNode&& other) : Pos(other.Pos),
                                       input_ports(move(other.input_ports)),
                                       output_ports(move(other.output_ports)),
                                       info(move(other.info)),
                                       operation(other.operation->Clone()),
                                       id(other.id),
                                       embedded_graph(other.embedded_graph)
{
	for (auto& input_port : input_ports)
		input_port.embedded_node = this;
	for (auto& output_port : output_ports)
		output_port.embedded_node = this;
}


glm::vec2 BaseNode::CalcInputSlotPos(int index) const
{
	return info->GetInputSlotPos(index) + Pos;
}

glm::vec2 BaseNode::CalcOutputSlotPos(int index) const
{
	return info->GetOutputSlotPos(index) + Pos;
}

// TODO: Get rid of dynamic memory allocation
bool BaseNode::Compute()
{
	if(IsReadyToCompute())
	{
		// Get the input data:
		std::vector<WeakTexture> input_data;
		for (const InputPort& ip : GetInputPorts()) input_data.push_back(ip.GetInputData());
		auto out = operation->ComputeInternal(input_data);
		for (int i = 0; i<GetOutputPorts().size(); i++)
		{
			GetOutputPorts()[i].StoreData(out.Output[i]);
		}
		ReprTextureUpdateEvent->Fire(out.ShowcaseImg);
		return true;
	}

	return false;
}

void BaseNode::ConnectToInput(unsigned int output_port_num, NodeHandle conn_to, unsigned int input_port_num)
{
	OutputPort& from = output_ports[output_port_num];
	InputPort& to = embedded_graph->Get(conn_to).input_ports[input_port_num];
	auto t = to.ToPortHandle();
	from.ConnectTo(t);
}

void BaseNode::DisconnectFrom(unsigned int output_port_num, NodeHandle conn_to, unsigned int input_port_num)
{
	OutputPort& from = output_ports[output_port_num];
	InputPort& to = embedded_graph->Get(conn_to).input_ports[input_port_num];
	auto t = to.ToPortHandle();
	from.Disconnect(t);
}

OutputPort& BaseNode::Lookup(const PortOutputHandle& handle) const
{
#ifndef NDEBUG
	return embedded_graph->Get(handle.first).GetOutputPorts()[handle.second];
#else
  return embedded_graph->Get(handle.first).GetOutputPorts().at(handle.second);
#endif
}

InputPort& BaseNode::Lookup(const PortInputHandle& handle) const
{
#ifndef NDEBUG
	return embedded_graph->Get(handle.first).GetInputPorts()[handle.second];
#else
  return embedded_graph->Get(handle.first).GetInputPorts().at(handle.second);
#endif
}

bool BaseNode::IsReadyToCompute() const
{
	return std::all_of(input_ports.begin(), input_ports.end(), [&](const InputPort& item) {return item.IsInputDataValid() || item.IsInputOptional(); });
}

