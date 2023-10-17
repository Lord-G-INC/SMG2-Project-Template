#ifdef NOGLE
#pragma once

#include "syati.h"
	namespace WarpAreaStageTable {

		void readTable(s32 selectedindex, bool useErrors);

		void selectWipeClose(s32 type, s32 fadeTime);
		void selectWipeOpen(s32 type, s32 fadeTime);
	};
#endif