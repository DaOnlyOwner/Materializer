#include "TextureWindow.h"
#include "imgui.h"
#include "Texture.h"

//https://stackoverflow.com/questions/45507041/how-to-check-if-weak-ptr-is-empty-non-assigned
template <typename T>
bool is_uninitialized(std::weak_ptr<T> const& weak) {
	using wt = std::weak_ptr<T>;
	return !weak.owner_before(wt{}) && !wt{}.owner_before(weak);
}

void TextureWindow::Show()
{
	ImGui::Begin("Texture Window");
	if( is_uninitialized(image_to_show) )
	{
		ImGui::LabelText("Texture out of cache","");
		ImGui::End();
		return;
	}
	
	Texture& img = *image_to_show.lock();
	ImGui::Image((void*)img.GetGPUPointer(), ImVec2{ (float)img.GetImgBuffer().width(), (float)img.GetImgBuffer().height() });

	ImGui::End();
}

void TextureWindow::SetUpdateEvent(SharedEvent<const WeakTexture&>& event)
{
	DELEGATE_FROM_THIS(const WeakTexture&, onTextureUpdate, f);
	update_event_guard = DelegateGuard<const WeakTexture&>(event,std::move(f));
}
