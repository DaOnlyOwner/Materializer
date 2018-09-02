#include "Scene.h"

#include "BaseNode.h"
#include "GeomUtils.h"
#include "InputPort.h"
#include "OutputPort.h"
#include "NodeInformation.h"
#include "Texture.h"
#include "CommandMenu.h"
#include "NodeFactory.h"
#include <GLFW/glfw3.h>

Scene::FrameInformation Scene::gen_frame_info(const BaseNode& node) const
{
	FrameInformation finfo;
	calc_frame_info(node, finfo);
	finfo.is_focused = false;
	finfo.is_node_dragged = false;
	return finfo;
}

void Scene::calc_frame_info(const BaseNode& node, FrameInformation& finfo) const
{
	finfo.pos = node.Pos;
	finfo.screen_pos = abs_to_screen(node.Pos);
	finfo.node_min = finfo.screen_pos;
	finfo.node_max = abs_to_screen(finfo.pos + node.GetNodeInformation()->Size);
	finfo.node_size = node.GetNodeInformation()->Size;
	glm::vec2 cam_min, cam_max;
	glm::vec2 node_size_with_slot = {finfo.node_size.x + SLOT_RADIUS, finfo.node_size.y};
	get_camera_rect(cam_min, cam_max);
	finfo.is_drawn = (IntersectionAABB_AABB(finfo.node_min - glm::vec2{SLOT_RADIUS, 0},
	                                        abs_to_screen(finfo.pos + node_size_with_slot),
	                                        cam_min, cam_max));
}

Scene::Scene() : menu({"Window: Texture View", "Operation: Test"})
{

}

void Scene::Update()
{
	ImGui::ShowMetricsWindow();
	ImGui::Begin("Nodegraph");
	//scaling += 0.01f;
	canvas_pos = ImGui::GetCursorScreenPos();
	canvas_size = ImGui::GetWindowSize();
	//printf("Canvas Size: %f, %f\n", canvas_size.x, canvas_size.y);
	//dl->PushClipRect(canvas_pos, ImVec2(canvas_pos.x+canvas_size.x, canvas_pos.y+canvas_size.y), true);
	if(menu.IsActive())
	{
		std::string out;
		if(menu.Show(out))
		{
			NodeFactory::Instance().Create(out, graph, screen_to_abs(menu.GetPos()));
			menu.Reset();
		}
	}
	window.Show();

	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, graph_color.BackgroundColor);
	ImGui::BeginChild("canvas", ImVec2(0, 0), false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	ImDrawList* dl = ImGui::GetWindowDrawList();

	render_grid(dl);
	render_selection_window(dl);
	//dl->ChannelsSplit(graph.GetNodes().size() + 1); // first channel (background) for nodes, second for connections, third for text
	for (size_t i = 0; i < graph.GetNodes().size(); i++)
	{
		auto& node = graph.GetNodes()[i];
		ImGui::PushID(node.GetId());
		FrameInformation finfo = gen_frame_info(node);
		handle_user_events(node, finfo);
		if (finfo.is_drawn)
			render_node(dl, node, finfo);

		render_connections(dl, node);
		ImGui::PopID();
	}

	//dl->ChannelsMerge();
	render_global(dl);
	handle_global_user_events();

	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::End();
}

void Scene::clear_focus_set()
{
	focus_set.clear();
}

void Scene::handle_global_user_events()
{

	if(ImGui::IsKeyDown(GLFW_KEY_R))
	{
		if(!focus_set.empty())
		{
			graph.ComputeFrom((*focus_set.begin()));
		}
	}

	if(ImGui::IsKeyDown(GLFW_KEY_SPACE) && ImGui::IsWindowFocused())
	{
		menu.Activate();
	}

	if(ImGui::IsKeyDown(GLFW_KEY_ESCAPE))
	{
		menu.Reset();
	}



	if (not_clicked_on_node_counter == graph.GetNodes().size() && !focus_set.empty())
	{
		clear_focus_set();
	}



	not_clicked_on_node_counter = 0;

	if (!focus_set.empty())
	{
		NodeHandle handle = *focus_set.begin();	
		auto& node = graph.Get(handle);
		if (window.GetUpdateEvent() != node.ReprTextureUpdateEvent)
		{
			window.SetUpdateEvent(node.ReprTextureUpdateEvent);
		}
	}

	

	if (ImGui::IsWindowFocused() && ImGui::IsMouseDragging() &&
		!is_dragging_any_conn &&
		!manually_dragged_node &&
		!is_selection_window_active)
	{
		camera_pos += glm::vec2(ImGui::GetMousePos()) - last_mouse_pos;
	}

	if (ImGui::IsMouseReleased(GLFW_MOUSE_BUTTON_LEFT))
	{
		is_dragging_any_conn = false;
		manually_dragged_node = 0;
		dragged_input = nullptr;
		dragged_output = nullptr;
		menu.Reset();
	}

	if (ImGui::GetIO().MouseWheel != 0)
	{
		float amount = ImGui::GetIO().MouseWheel / 2.f;
		auto anchor_point = screen_to_abs(ImGui::GetMousePos());
		set_scaling(scaling_ + amount);
		auto anchor_point_after = screen_to_abs(ImGui::GetMousePos());
		auto delta = (anchor_point_after - anchor_point) * scaling_;
		camera_pos += delta;
	}

	last_mouse_pos = ImGui::GetMousePos();
}

void Scene::render_selection_window(ImDrawList* dl)
{
	// Start choice process
	if (ImGui::IsMouseDown(GLFW_MOUSE_BUTTON_RIGHT) && ImGui::IsMouseDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		if (selection_min == selection_max)
		{
			selection_min = ImGui::GetMousePos();
			is_selection_window_active = true;
		}

		else if (selection_min != selection_max)
		{
			selection_max = ImGui::GetMousePos();
			dl->AddRectFilled(selection_min, selection_max, ImColor(150, 150, 150, 80));
			dl->AddRect(selection_min, selection_max, ImColor(150, 150, 150, 255));
		}
	}
	else
	{
		selection_min = {0, 0};
		selection_max = {0, 0};
		is_selection_window_active = false;
	}
}

void Scene::render_node(ImDrawList* dl, BaseNode& node, const FrameInformation& finfo) const
{
	auto info = node.GetNodeInformation();
	if (finfo.is_focused)
		dl->AddRectFilled(abs_to_screen(finfo.pos - glm::vec2(.5, .5)),
		                  abs_to_screen(finfo.pos + finfo.node_size + glm::vec2(.5, .5)), Colors::FromHex("AA AA AA", 0.5f),
		                  4.0f);
	dl->AddRectFilled(finfo.screen_pos, abs_to_screen(finfo.pos + finfo.node_size), Colors::FromHex("11 64 66", 1), 4.0f);
	dl->AddRect(finfo.screen_pos, abs_to_screen(finfo.pos + finfo.node_size), Colors::FromHex("AA AA AA", 1.f), 4.0f);
	
	SharedTexture& img = node.GetReprTexture();
	if(img != nullptr)
	{
		auto gpu_ptr = img->GetGPUPointer();
		dl->AddImage((void*)gpu_ptr, finfo.screen_pos, abs_to_screen(finfo.pos + glm::vec2{ img->GetImgBuffer().width(),img->GetImgBuffer().height() }));
	}


	if (ImGui::IsMouseHoveringRect(finfo.node_min, finfo.node_max) || finfo.is_node_dragged)
	{
		// Add descriptions of ports when hovering
		for (size_t i = 0; i < info->InputDescr.size(); i++)
		{
			auto& descr = info->InputDescr[i];
			auto pos_ = node.CalcInputSlotPos(i);
			glm::vec2 text_size = ImGui::CalcTextSize(descr.c_str());
			ImGui::SetCursorScreenPos(abs_to_screen(pos_ - glm::vec2(2.f, -SLOT_RADIUS / 1.6f)) - text_size);
			ImGui::TextUnformatted(descr.c_str());
		}
		for (size_t i = 0; i < info->OutputDescr.size(); i++)
		{
			auto& descr = info->OutputDescr[i];
			auto pos_ = node.CalcOutputSlotPos(i);
			ImGui::SetCursorScreenPos(abs_to_screen(pos_ + glm::vec2(2.f, -SLOT_RADIUS / 1.6f)));
			ImGui::TextUnformatted(descr.c_str());
		}
	}

	// Add visual feedback, when user hovers output / input slot
	for (int i = 0; i < info->GetInputNum(); i++)
	{
		glm::vec2 min = abs_to_screen(node.CalcInputSlotPos(i) - SLOT_RADIUS);
		glm::vec2 max = abs_to_screen(node.CalcInputSlotPos(i) + SLOT_RADIUS);
		if (ImGui::IsMouseHoveringRect(min, max))
		{
			dl->AddCircleFilled(abs_to_screen(node.CalcInputSlotPos(i)), SLOT_RADIUS * scaling_,
			                    Colors::FromHex("D9 B0 8C", 1.f));
		}
		else
			dl->AddCircleFilled(abs_to_screen(node.CalcInputSlotPos(i)), SLOT_RADIUS * scaling_,
			                    Colors::FromHex("D9 B0 8C", 0.5f));
	}


	for (int i = 0; i < info->GetOutputNum(); i++)
	{
		glm::vec2 min = abs_to_screen(node.CalcOutputSlotPos(i) - SLOT_RADIUS);
		glm::vec2 max = abs_to_screen(node.CalcOutputSlotPos(i) + SLOT_RADIUS);
		if (ImGui::IsMouseHoveringRect(min, max))
		{
			dl->AddCircleFilled(abs_to_screen(node.CalcOutputSlotPos(i)), SLOT_RADIUS * scaling_,
			                    Colors::FromHex("D9 B0 8C", 1.f));
		}
		else
			dl->AddCircleFilled(abs_to_screen(node.CalcOutputSlotPos(i)), SLOT_RADIUS * scaling_,
			                    Colors::FromHex("D9 B0 8C", 0.5f));
	}

	// Add name to the node, always.
	glm::vec2 text_size = glm::vec2(ImGui::CalcTextSize(info->Name.c_str())) / scaling_;
	ImGui::SetCursorScreenPos(
		abs_to_screen(finfo.pos - glm::vec2(-info->Size.x / 2.f + text_size.x / 2.f, text_size.y + 1.f)));
	ImGui::TextUnformatted(info->Name.c_str());
}

void Scene::render_connections(ImDrawList* dl, BaseNode& node) const
{
	glm::vec2 cam_min, cam_max;
	get_camera_rect(cam_min, cam_max);
	for (auto& port : node.GetOutputPorts())
	{
		for (auto con_node : port.GetConnections())
		{
			glm::vec2 screen_node_pos = abs_to_screen(port.GetPos());
			glm::vec2 screen_conn_pos = abs_to_screen(node.Lookup(con_node).GetPos());
			if (IntersectionLine_AABB(screen_node_pos, screen_conn_pos, cam_min, cam_max))
				dl->AddBezierCurve(screen_node_pos, abs_to_screen(port.GetPos() + glm::vec2(5, 0)),
				                   abs_to_screen(node.Lookup(con_node).GetPos() + glm::vec2(-5, 0)),
				                   screen_conn_pos, IM_COL32(200, 200, 100, 255), 0.65f * scaling_);
		}
	}
}

void Scene::render_global(ImDrawList* dl) const
{
	if (dragged_input != nullptr)
	{
		glm::vec2 screen_port_pos = abs_to_screen(dragged_input->GetPos());
		glm::vec2 screen_mouse_pos = ImGui::GetMousePos();
		dl->AddBezierCurve(screen_port_pos,
		                   abs_to_screen(dragged_input->GetPos() + glm::vec2(-5, 0)),
		                   abs_to_screen(screen_to_abs(screen_mouse_pos) + glm::vec2(+5, 0)),
		                   screen_mouse_pos, IM_COL32(200, 200, 100, 255), 0.65 * scaling_);
	}

	else if (dragged_output != nullptr)
	{
		glm::vec2 screen_port_pos = abs_to_screen(dragged_output->GetPos());
		glm::vec2 screen_mouse_pos = ImGui::GetMousePos();
		dl->AddBezierCurve(screen_port_pos,
		                   abs_to_screen(dragged_output->GetPos() + glm::vec2(5, 0)),
		                   abs_to_screen(screen_to_abs(screen_mouse_pos) + glm::vec2(-5, 0)),
		                   screen_mouse_pos, IM_COL32(200, 200, 100, 255), 0.65 * scaling_);
	}
}

void Scene::handle_slot_event(BaseNode& node, FrameInformation& finfo)
{
	for (int i = 0; i < node.GetNodeInformation()->GetInputNum(); i++)
	{
		glm::vec2 min = abs_to_screen(node.CalcInputSlotPos(i) - SLOT_RADIUS);
		glm::vec2 max = abs_to_screen(node.CalcInputSlotPos(i) + SLOT_RADIUS);
		if (ImGui::IsMouseHoveringRect(min, max))
		{
			if (ImGui::IsMouseDown(0) && !manually_dragged_node && !is_dragging_any_conn && !is_selection_window_active)
			{
				dragged_input = &node.GetInputPorts()[i];
				is_dragging_any_conn = true;
			}
			if (!ImGui::IsMouseDown(0) && is_dragging_any_conn && dragged_output)
			{
				is_dragging_any_conn = false;
				//assert(dragged_output != nullptr);
				dragged_output->ConnectTo(node.GetInputPorts()[i].ToPortHandle());
				dragged_input = nullptr;
				dragged_output = nullptr;
			}
		}
	}
	for (int i = 0; i < node.GetNodeInformation()->GetOutputNum(); i++)
	{
		glm::vec2 min = abs_to_screen(node.CalcOutputSlotPos(i) - SLOT_RADIUS);
		glm::vec2 max = abs_to_screen(node.CalcOutputSlotPos(i) + SLOT_RADIUS);
		if (ImGui::IsMouseHoveringRect(min, max))
		{
			if (ImGui::IsMouseDown(0) && !manually_dragged_node && !is_dragging_any_conn && !is_selection_window_active)
			{
				dragged_output = &node.GetOutputPorts()[i];
				is_dragging_any_conn = true;
			}
			if (!ImGui::IsMouseDown(0) && is_dragging_any_conn && dragged_input)
			{
				is_dragging_any_conn = false;
				assert(dragged_input != nullptr);
				dragged_input->ConnectFrom(node.GetOutputPorts()[i].ToPortHandle());
				dragged_input = nullptr;
				dragged_output = nullptr;
			}
		}
	}
}

void Scene::handle_node_dragging(BaseNode& node, FrameInformation& finfo)
{
	if (ImGui::IsMouseClicked(GLFW_MOUSE_BUTTON_LEFT))
	{
		if (ImGui::IsMouseHoveringRect(finfo.node_min, finfo.node_max))
		{
			finfo.is_node_dragged = true;
			manually_dragged_node = node.GetId();
		}
		else not_clicked_on_node_counter++;
	}

	if (ImGui::IsMouseDragging(GLFW_MOUSE_BUTTON_LEFT) && manually_dragged_node == node.GetId())
	{
		node.Pos -= screen_to_abs(last_mouse_pos) - screen_to_abs(ImGui::GetMousePos());
		finfo.is_node_dragged = true;
		calc_frame_info(node, finfo);
	}
}

void Scene::handle_user_events(BaseNode& node, FrameInformation& finfo)
{
	if (focus_set.find(node.GetId()) != focus_set.end())
		finfo.is_focused = true;

	if (finfo.is_drawn)
	{
		handle_slot_event(node, finfo);
		handle_node_dragging(node, finfo);
		if (ImGui::IsMouseClicked(GLFW_MOUSE_BUTTON_RIGHT) && ImGui::IsMouseHoveringRect(finfo.node_min, finfo.node_max))
		{
			focus_set.clear();
			focus_set.insert(node.GetId());
			finfo.is_focused = true;
		}
	}
}

void Scene::render_grid(ImDrawList* dl) const
{
	glm::vec2 offset_big = {fmodf(camera_pos.x, grid_size), fmodf(camera_pos.y, grid_size)};
	glm::vec2 offset_small = {fmodf(camera_pos.x, grid_size / 3.f), fmodf(camera_pos.y, grid_size / 3.f)};

	ImU32 color_big = graph_color.GridColor;
	ImU32 color_small = graph_color.GridColorSmall;

	render_grid_part(dl, offset_big, color_big, grid_size);
	render_grid_part(dl, offset_small, color_small, grid_size / 3.f);
}

void Scene::render_grid_part(ImDrawList* dl, const glm::vec2& offset, ImU32 color, float actual_grid_size) const
{
	for (glm::vec2 i = offset; i.x < canvas_size.x || i.y < canvas_size.y; i += actual_grid_size)
	{
		dl->AddLine(glm::vec2(i.x, 0.0f) + canvas_pos,
		            glm::vec2(i.x, canvas_size.y) + canvas_pos, color);
		dl->AddLine(glm::vec2(0.0f, i.y) + canvas_pos,
		            glm::vec2(canvas_size.x, i.y) + canvas_pos, color);
	}
}
