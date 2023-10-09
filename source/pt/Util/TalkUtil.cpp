#include "pt/Util/TalkUtil.h"

/*
* Author: Aurum
*/

namespace pt {
	void setMessageArg(TalkMessageCtrl *pTalkCtrl, const wchar_t *pText) {
		CustomTagArg tagArg;
		tagArg.mType = 1;
		tagArg.mStringArg = pText;
		pTalkCtrl->setMessageArg(tagArg);
	}

	void setTextBoxArgNumberNumberFontRecursive(LayoutActor* pLayout, const char* paneName, s32 num, s32 index) {
		s32 digits = log10(num)+1;
		const wchar_t* str = new wchar_t[12];
		swprintf((wchar_t*)str, 12, L"\u000E\u000A\u0001%lc%lc%d", 2+digits*2, digits*2, num);
        MR::setTextBoxArgStringRecursive(pLayout, paneName, str, index);
		delete str;
	}
}
