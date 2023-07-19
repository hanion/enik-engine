#pragma once

namespace Enik {

class GraphicsContext {
   public:
	GraphicsContext();
	~GraphicsContext();

	virtual void Init() = 0;
	virtual void SwapBuffers() = 0;
};

}