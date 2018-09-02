#include "InputPort.h"

#include "BaseNode.h"
#include "Texture.h"
#include "OutputPort.h"
#include "NodeInformation.h"

bool InputPort::IsInputOptional() const
{
	return embedded_node->GetNodeInformation()->InputOptional[index];
}

void InputPort::RegisterConnectionFrom(const PortOutputHandle& port)
{
	UnregisterConnection();
	connection = port;
}

void InputPort::UnregisterConnection()
{
	if (connection.validate())
		embedded_node->Lookup(connection).UnregisterConnection(ToPortHandle());
	connection.first = 0;
	connection.second = 0;
}

void InputPort::ConnectFrom(const PortOutputHandle& port)
{
	RegisterConnectionFrom(port);
	embedded_node->Lookup(port).RegisterConnectionTo(ToPortHandle());
}

void InputPort::Disconnect()
{
	embedded_node->Lookup(connection).UnregisterConnection(ToPortHandle());
	UnregisterConnection();
}

glm::vec2 InputPort::GetPos() const
{
	return embedded_node->CalcInputSlotPos(index);
}

PortInputHandle InputPort::ToPortHandle() const
{
	return PortInputHandle(embedded_node->GetId(), index);
}
