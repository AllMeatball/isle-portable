#ifndef LEGOEVENTNOTIFICATIONPARAM_H
#define LEGOEVENTNOTIFICATIONPARAM_H

#include "mxnotificationparam.h"
#include "mxtypes.h"

#include <SDL3/SDL_keycode.h>
#include <stdlib.h>

class LegoROI;

// VTABLE: LEGO1 0x100d6aa0
// SIZE 0x20
class LegoEventNotificationParam : public MxNotificationParam {
public:
	enum {
		c_lButtonState = 1,
		c_rButtonState = 2,
		c_modKey1 = 4,
		c_modKey2 = 8,
	};

	// FUNCTION: LEGO1 0x10028690
	MxNotificationParam* Clone() const override
	{
		LegoEventNotificationParam* clone =
			new LegoEventNotificationParam(m_type, m_sender, m_modifier, m_x, m_y, m_key);
		clone->m_roi = m_roi;
		return clone;
	} // vtable+0x04

	LegoEventNotificationParam() : MxNotificationParam(c_notificationType0, NULL) {}
	LegoEventNotificationParam(
		MxS32 p_type,
		MxCore* p_sender,
		MxU8 p_modifier,
		MxS32 p_x,
		MxS32 p_y,
		SDL_Keycode p_key
	)
		: MxNotificationParam((NotificationId)p_type, p_sender), m_modifier(p_modifier), m_x(p_x), m_y(p_y), m_key(p_key), m_roi(NULL)
	{
	}

	LegoROI* GetROI() { return m_roi; }
	MxU8 GetModifier() { return m_modifier; }
	SDL_Keycode GetKey() const { return m_key; }

	// FUNCTION: LEGO1 0x10012190
	// FUNCTION: BETA10 0x10024210
	MxS32 GetX() const { return m_x; }

	// FUNCTION: LEGO1 0x100121a0
	// FUNCTION: BETA10 0x10024240
	MxS32 GetY() const { return m_y; }

	void SetROI(LegoROI* p_roi) { m_roi = p_roi; }

	// FUNCTION: BETA10 0x1007d620
	void SetModifier(MxU8 p_modifier) { m_modifier = p_modifier; }

	// FUNCTION: BETA10 0x1007d6b0
	void SetKey(SDL_Keycode p_key) { m_key = p_key; }

	// FUNCTION: BETA10 0x1007d650
	void SetX(MxS32 p_x) { m_x = p_x; }

	// FUNCTION: BETA10 0x1007d680
	void SetY(MxS32 p_y) { m_y = p_y; }
#ifdef LEGO1_DLL
	static void expose(ssq::VM& vm) {
		ssq::Class cls = vm.addClass("LegoEventNotificationParam", ssq::Class::Ctor<LegoEventNotificationParam(
			MxS32 p_type,
			MxCore* p_sender,
			MxU8 p_modifier,
			MxS32 p_x,
			MxS32 p_y,
			SDL_Keycode p_key
		)>());

		cls.addFunc("SetNotification", &LegoEventNotificationParam::SetNotification);
		cls.addFunc("SetKey", &LegoEventNotificationParam::SetKey);
		cls.addFunc("SetX", &LegoEventNotificationParam::SetX);
		cls.addFunc("SetY", &LegoEventNotificationParam::SetY);
		cls.addFunc("SetModifier", &LegoEventNotificationParam::SetModifier);

		cls.addFunc("GetNotification", &LegoEventNotificationParam::GetNotification);
		cls.addFunc("GetKey", &LegoEventNotificationParam::GetKey);
		cls.addFunc("GetX", &LegoEventNotificationParam::GetX);
		cls.addFunc("GetY", &LegoEventNotificationParam::GetY);
		cls.addFunc("GetModifier", &LegoEventNotificationParam::GetModifier);
	}
#else
	void expose(ssq::VM& vm);
#endif

protected:
	MxU8 m_modifier;   // 0x0c
	MxS32 m_x;         // 0x10
	MxS32 m_y;         // 0x14
	SDL_Keycode m_key; // 0x18
	LegoROI* m_roi;    // 0x1c
};

// SYNTHETIC: LEGO1 0x10028770
// LegoEventNotificationParam::`scalar deleting destructor'

// SYNTHETIC: LEGO1 0x100287e0
// LegoEventNotificationParam::~LegoEventNotificationParam

#endif // LEGOEVENTNOTIFICATIONPARAM_H
