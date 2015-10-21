#pragma once

#include <vector>

#include "..\logic\level.hpp"
#include "..\graphics\layer\ui\uilayer.hpp"
#include "..\graphics\framebuffer.hpp"
#include "..\graphics\layer\ui\elements\uitexture.hpp"
#include "..\graphics\layer\ui\elements\uilabel.hpp"

namespace lys
{

	class Arena : public Level
	{
	private:
		UILayer _ui;
		UITexture _test2;
		UITexture _test3;
		UITexture _test4;
		UILabel _label;
		FrameBuffer _fb;
	public:
		Arena(EngineInternals &internals, const EngineLoadingArgs &args);

		void update(EngineInternals &internals, EngineArgs &args) override;
		void draw(EngineInternals &internals, EngineArgs &args) override;

		void resize(EngineInternals &internals) override;
	};

}