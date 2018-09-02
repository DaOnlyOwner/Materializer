#pragma once
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include <string>
#include <vector>

constexpr float SLOT_RADIUS = 1.3f;
constexpr float SLOT_TOL = .5f;
constexpr float NODE_BODY_BASE_SIZE = 20.f;


struct NodeInformation
{
	NodeInformation();
	NodeInformation(std::string&& name, std::string&& tooltip, std::vector<std::string>&& input_descr, std::vector<std::string>&& output_descr, std::vector<bool>&& input_optional);

	std::string Name;
	std::string Tooltip;
	std::vector<std::string> InputDescr;
	std::vector<std::string> OutputDescr;	
	glm::vec2 Size;

	std::vector<bool> InputOptional;

	int GetInputNum() { return InputDescr.size(); }
	int GetOutputNum() { return OutputDescr.size(); }
	glm::vec2 GetInputSlotPos(int index) const;
	glm::vec2 GetOutputSlotPos(int index) const;
};
