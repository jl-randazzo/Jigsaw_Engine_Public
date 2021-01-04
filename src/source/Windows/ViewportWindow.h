
#ifndef _VIEWPORT_WINDOW_H_
#define _VIEWPORT_WINDOW_H_
#include "window.h"
#include "Ref.h"
#include "Graphics/RenderContext.h"

/**
 * This class exists to encapsulate low-level data for the window and program context. 
*/
class ViewportWindow: public Window {
public:
	// Constructors
	ViewportWindow(float wh_ratio, const Jigsaw::Ref<Jigsaw::Graphics::RenderContext>& render_context) : wh_ratio(wh_ratio), render_context(render_context) { }

	ViewportWindow(const ViewportWindow& other, float wh_ratio, const Jigsaw::Ref<Jigsaw::Graphics::RenderContext>& render_context) 
		: wh_ratio(wh_ratio), render_context(render_context) {}

	float GetWHRatio() const {
		return wh_ratio;
	};

	void PopulateWindowClass(WNDCLASS *wndClass, HINSTANCE hInstance);

protected:
	LRESULT Awake(const HWND hWnd) throw(HRESULT);
	
private:

	const Jigsaw::Ref<Jigsaw::Graphics::RenderContext> const render_context;

	// private variables
	float wh_ratio;
};
#endif