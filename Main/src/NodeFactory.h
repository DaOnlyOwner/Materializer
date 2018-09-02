#pragma once
#include <string>
#include <unordered_map>
#include "IOperation.h"
#include "NodeInformation.h"

class Graph;

struct operation_information
{
	std::shared_ptr<NodeInformation> info;
	std::unique_ptr<IOperation> operation;
};

class NodeFactory
{
	std::unordered_map<std::string, operation_information> operation_map;
public:
	NodeFactory(const NodeFactory&) = delete;
	NodeFactory& operator= (const NodeFactory&) = delete;
	
	static NodeFactory& Instance()
	{
		static NodeFactory instance_;
		return instance_;
	}
	void Create(const std::string& key, Graph& embedded_graph, const glm::vec2& pos);

	template<typename TOperationType>
	void Register(NodeInformation&& information, const std::string& key)
	{
		static_assert(std::is_base_of<IOperation, TOperationType>::value, "Make sure that IOperation is a base class of the template parameter");
		operation_map[key] = operation_information{ std::make_shared<NodeInformation>(std::move(information)), std::make_unique<TOperationType>() };
	}

private:
	~NodeFactory() = default;
	NodeFactory() = default;
};
