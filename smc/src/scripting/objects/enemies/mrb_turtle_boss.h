// -*- c++ -*-
#ifndef SMC_SCRIPTING_TURTLE_BOSS_H
#define SMC_SCRIPTING_TURTLE_BOSS_H
#include "../../scripting.h"

namespace SMC {
	namespace Scripting {
		extern struct RClass* p_rcTurtleBoss;
		void Init_TurtleBoss(mrb_state* p_state);
	}
}
#endif