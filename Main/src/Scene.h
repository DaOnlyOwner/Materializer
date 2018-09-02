#pragma once

#include "Graph.h"
#include "imgui.h"
#include <glm/vec2.hpp>
#include <algorithm>
#include <set>
#include <string>
#include "CommandMenu.h"
#include "TextureWindow.h"

class InputPort;
class OutputPort;
class BaseNode;

struct Colors
{
	static ImU32 FromHex(std::string rrggbb, float a = 1)
	{
		if (rrggbb.size() != 8)
			throw std::runtime_error("Color doesn't match format (rr gg bb)");
		int r, g, b;
		char* end;
		r = strtol(rrggbb.c_str(), &end, 16);
		g = strtol(end, &end, 16);
		b = strtol(end, &end, 16);
		return ImColor(r, g, b, (int)(a * 255));
	}

	ImU32 BackgroundColor = FromHex("2C 35 31");
	ImU32 GridColor = FromHex("11 11 00", 0.4f);
	ImU32 GridColorSmall = FromHex("11 11 00", 0.3f);
	//ImU32
	//ImU32
};

class Scene
{
private:
	struct FrameInformation
	{
		glm::vec2 pos, screen_pos, node_size, node_min, node_max;
		bool is_node_dragged, is_drawn, is_focused;
	};

	FrameInformation gen_frame_info(const BaseNode& node) const;

	void calc_frame_info(const BaseNode& node, FrameInformation& finfo) const;

private:
	glm::vec2 camera_pos = {0, 0};
	float scaling_ = 7;
	glm::vec2 canvas_pos;
	glm::vec2 canvas_size;

	float grid_size = 64.f;
	Colors graph_color;

	Graph graph;
	CommandMenu menu;
	TextureWindow window;

	glm::vec2 last_mouse_pos;

	std::set<NodeHandle> focus_set;
	
	NodeHandle manually_dragged_node = 0; // The node that is dragged manually by hovering over it and moving the mouse
	glm::vec2 selection_min, selection_max; // In Screen coords
	bool is_dragging_any_conn = false;
	bool is_selection_window_active = false;
	OutputPort* dragged_output = nullptr;
	InputPort* dragged_input = nullptr;
	unsigned int not_clicked_on_node_counter = 0;
public:
	Scene();
	void Update();
	void clear_focus_set();

private:

	void set_scaling(float scaling_new)
	{
		scaling_ = std::max(std::min(scaling_new, 50.f), .5f);
	}

	void handle_global_user_events();
	void render_selection_window(ImDrawList* dl);
	void render_node(ImDrawList* dl, BaseNode& node, const FrameInformation& finfo) const;
	void render_grid(ImDrawList* dl) const;
	void render_grid_part(ImDrawList* dl, const glm::vec2& offset, ImU32 color, float actual_grid_size) const;
	void render_connections(ImDrawList* dl, BaseNode& node) const;
	void render_global(ImDrawList* dl) const;
	void handle_slot_event(BaseNode& node, FrameInformation& finfo);
	void handle_node_dragging(BaseNode& node, FrameInformation& finfo);
	void handle_user_events(BaseNode& node, FrameInformation& finfo);


	void get_camera_rect(glm::vec2& min, glm::vec2& max) const
	{
		min = canvas_pos;
		max = canvas_size + canvas_pos;
	}

	glm::vec2 screen_to_abs(const glm::vec2& screen) const
	{
		return (screen - canvas_pos - camera_pos) / scaling_;
	}

	glm::vec2 abs_to_screen(const glm::vec2& relative) const
	{
		return scaling_ * relative + camera_pos + canvas_pos;
	}

	float abs_to_screen(float relative) const
	{
		return (glm::vec2{relative, relative} * scaling_ + camera_pos + canvas_pos).x;
	}
};
