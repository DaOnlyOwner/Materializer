#include "OutputPort.h"
#include "BaseNode.h"
#include "Texture.h"
#include "InputPort.h"
#include <algorithm>


glm::vec2 OutputPort::GetPos() const
{
	return embedded_node->CalcOutputSlotPos(index);
}


void OutputPort::RegisterConnectionTo(const PortInputHandle& port)
{
	if (find(connections.begin(), connections.end(), port) == connections.end())
		connections.push_back(port);
}

void OutputPort::UnregisterConnection(const PortInputHandle& port)
{
	connections.erase(remove(connections.begin(), connections.end(), port), connections.end());
}

void OutputPort::ConnectTo(const PortInputHandle& port)
{
	RegisterConnectionTo(port);
	embedded_node->Lookup(port).RegisterConnectionFrom(ToPortHandle());
}

void OutputPort::Disconnect(const PortInputHandle& port)
{
	UnregisterConnection(port);
	embedded_node->Lookup(port).UnregisterConnection();
}

PortOutputHandle OutputPort::ToPortHandle() const
{
	return PortOutputHandle(embedded_node->GetId(), index);
}
