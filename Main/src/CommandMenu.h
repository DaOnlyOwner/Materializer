#pragma once

#include "imgui.h"
#include <vector>
#include <algorithm>

constexpr int BUFFER_SIZE = 1024;

struct item_selection_container
{
	item_selection_container(std::vector<const char*>&& its) : items_to_search(std::move(its)) {}
	std::vector<const char*> items_to_search;
	std::vector<const char*> items_chosen;
};

class CommandMenu
{
	bool cursor_pos_set = false;
	glm::vec2 cursor_pos = {0,0};

	int current = 0;
	bool active = false;
	item_selection_container container;
	char buffer[BUFFER_SIZE] = "Search commands...";

public:
	CommandMenu(std::vector<const char*>&& items_to_search) : container(std::move(items_to_search)){}
	bool Show(std::string& out);
	void Reset() { cursor_pos_set = false; active = false; }
	bool IsActive() const { return active; }
	void Activate() { active = true; }
	glm::vec2 GetPos() const { return cursor_pos; }
};
