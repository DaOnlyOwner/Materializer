#include "NodeInformation.h"
#include "math.h"

glm::vec2 calc_size(unsigned int inputslot_size, unsigned int outputslot_size)
{
	int num_slots = inputslot_size >= outputslot_size ? inputslot_size : outputslot_size;
	float spacing = 1.f / (num_slots + 1);
	float pre = (SLOT_RADIUS + SLOT_TOL) * num_slots;
	pre += num_slots * pre * spacing;
	float height = fmax(NODE_BODY_BASE_SIZE, pre);


	return { NODE_BODY_BASE_SIZE, height };
}

NodeInformation::NodeInformation()
{
	Size = calc_size(InputDescr.size(), OutputDescr.size());
}

NodeInformation::NodeInformation(std::string&& name, std::string&& tooltip, std::vector<std::string>&& input_descr,
	std::vector<std::string>&& output_descr, std::vector<bool>&& input_optional) :
	Name(std::move(name)), Tooltip(std::move(tooltip)), InputDescr(std::move(input_descr)), OutputDescr(std::move(output_descr)), InputOptional(std::move(input_optional))
{
	Size = calc_size(InputDescr.size(), OutputDescr.size());
}

glm::vec2 NodeInformation::GetInputSlotPos(int index) const
{
	float y = (Size.y / (float)(InputDescr.size() + 1)) * (index + 1.f);
	return {0, y};
}

glm::vec2 NodeInformation::GetOutputSlotPos(int index) const
{
	float y = (Size.y / (float)(OutputDescr.size() + 1)) * (index + 1.f);
	float x = Size.x;
	return {x, y};
}
