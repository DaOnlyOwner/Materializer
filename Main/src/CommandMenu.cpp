#include "CommandMenu.h"
#include <ctype.h>
#include "GLFW/glfw3.h"


bool contains(const char* pattern, const char* text)
{
	for (int i = 0; text[i] != '\0'; i++)
	{
		bool match = true;
		for (int j = 0; pattern[j] != NULL; j++)
		{
			if (text + i + j == '\0' || tolower(text[i + j]) != tolower(pattern[j])) {
				match = false; break;
			}
		}
		if (match) return true;
	}
	return false;
}

int handle_edit_event(ImGuiTextEditCallbackData *data)
{
	item_selection_container* container = static_cast<item_selection_container*>(data->UserData);
	container->items_chosen.clear();
	for (const char* item : container->items_to_search) {
		if (contains(data->Buf, item))
		{
			container->items_chosen.push_back(item);
		}
	}
	return 1;
}

bool CommandMenu::Show(std::string& out)
{
	bool enter = false;

	if (!cursor_pos_set) {
		cursor_pos = ImGui::GetMousePos(); cursor_pos_set = true;
	}

	ImGui::SetNextWindowPos(cursor_pos);
	ImGui::SetNextWindowFocus();
	ImGui::SetNextWindowSize({ 200, 300 });
	ImGui::Begin("Command Menu");
	if (ImGui::IsKeyPressed(GLFW_KEY_UP))
	{
		current = std::max(0, current - 1);
	}

	else if (ImGui::IsKeyPressed(GLFW_KEY_DOWN))
	{
		current = std::min((int)container.items_chosen.size() - 1, current + 1);
	}

	else ImGui::SetKeyboardFocusHere();
	if (ImGui::ListBox("##choicelb", &current, container.items_chosen.data(), container.items_chosen.size(), 10) || ImGui::IsKeyDown(GLFW_KEY_ENTER))
	{
		out = container.items_chosen[current];
		enter = true;
	}
	ImGui::Separator();
	ImGui::InputText("##comsearch", buffer, BUFFER_SIZE, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CallbackAlways, handle_edit_event, &container);
	ImGui::End();
	return enter;
}