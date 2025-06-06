#ifndef MXPALETTE_H
#define MXPALETTE_H

#include "mxcore.h"
#include "mxtypes.h"

#ifdef MINIWIN
#include "miniwin/ddraw.h"
#else
#include <ddraw.h>
#endif

// VTABLE: LEGO1 0x100dc848
// VTABLE: BETA10 0x101c2300
// SIZE 0x414
class MxPalette : public MxCore {
public:
	MxBool operator==(MxPalette& p_other);
	void Detach();

	MxPalette();
	MxPalette(const RGBQUAD*);
	~MxPalette() override;

	void ApplySystemEntriesToPalette(LPPALETTEENTRY p_entries);
	MxPalette* Clone();
	void GetDefaultPalette(LPPALETTEENTRY p_entries);
	MxResult GetEntries(LPPALETTEENTRY p_entries);
	MxResult SetEntries(LPPALETTEENTRY p_palette);
	MxResult SetSkyColor(LPPALETTEENTRY p_skyColor);
	void Reset(MxBool p_ignoreSkyColor);
	LPDIRECTDRAWPALETTE CreateNativePalette();

	void SetPalette(LPDIRECTDRAWPALETTE p_palette);

	// FUNCTION: BETA10 0x100d92c0
	void SetOverrideSkyColor(MxBool p_value) { m_overrideSkyColor = p_value; }

	// SYNTHETIC: LEGO1 0x100beeb0
	// SYNTHETIC: BETA10 0x10144640
	// MxPalette::`scalar deleting destructor'

private:
	LPDIRECTDRAWPALETTE m_palette;
	PALETTEENTRY m_entries[256]; // 0x0c
	MxBool m_overrideSkyColor;   // 0x40c
	PALETTEENTRY m_skyColor;     // 0x40d
};

#endif // MXPALETTE_H
