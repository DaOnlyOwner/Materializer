#pragma once
#include "IOperation.h"
#include "InputPort.h"
#include "OutputPort.h"
#include "BaseNode.h"

class TextureWindow
{
public:
	TextureWindow() = default;
	void Show();
	void SetUpdateEvent( SharedEvent<const WeakTexture&>& event );
	SharedEvent<const WeakTexture&> GetUpdateEvent() { return update_event_guard.GetEvent().lock(); }

private:
	WeakTexture image_to_show;
	DelegateGuard<const WeakTexture&> update_event_guard;
	void onTextureUpdate(const WeakTexture& image) { image_to_show = image; }
};

