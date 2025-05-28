#pragma once

#include "miniwin_d3drm_sdl3gpu.h"
#include "miniwin_d3drmobject_sdl3gpu.h"
#include "miniwin_p.h"

struct Direct3DRMLight_SDL3GPUImpl : public Direct3DRMObjectBase_SDL3GPUImpl<IDirect3DRMLight> {
	Direct3DRMLight_SDL3GPUImpl(D3DRMLIGHTTYPE type, float r, float g, float b);
	HRESULT SetColorRGB(float r, float g, float b) override;

	D3DRMLIGHTTYPE m_type;
	D3DCOLOR m_color = 0xFFFFFFFF;
};

struct Direct3DRMLightArray_SDL3GPUImpl
	: public Direct3DRMArrayBase_SDL3GPUImpl<IDirect3DRMLight, Direct3DRMLight_SDL3GPUImpl, IDirect3DRMLightArray> {
	using Direct3DRMArrayBase_SDL3GPUImpl::Direct3DRMArrayBase_SDL3GPUImpl;
};
