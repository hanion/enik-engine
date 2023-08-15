#pragma once

namespace Enik {

class GraphicsContext {
   public:
	GraphicsContext();
	virtual ~GraphicsContext() = default;

	virtual void Init() = 0;
	virtual void SwapBuffers() = 0;
};

}