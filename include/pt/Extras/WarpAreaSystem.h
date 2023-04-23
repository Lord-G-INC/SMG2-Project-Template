#pragma once

#include "syati.h"

#ifdef NOGLE
	class ErrorLayout : public LayoutActor {
	public:
		ErrorLayout();

		virtual void init (const JMapInfoIter& rIter);

		void printf(bool canPrint, const char* format, ...);
	};

	namespace WarpAreaStageTable {

		void readTable(s32 selectedindex, bool useErrors);

		void selectWipeClose(s32 type, s32 fadeTime);
		void selectWipeOpen(s32 type, s32 fadeTime);
	};
#endif