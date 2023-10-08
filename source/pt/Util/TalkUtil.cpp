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

	// if this were possible through MSBT, I'd do it that way
	// this code also sucks. i need to figure out how to handle this better
	void setTextBoxArgNumberNumberFontRecursive(LayoutActor* pLayout, const char* paneName, s32 num, s32 index) {
		s32 charnum = log10(num)+1;
		const char* str2;

			if (charnum == 1)
				str2 = "\u0004\u0002";
			else if (charnum == 2)
				str2 = "\u0006\u0004";
			else if (charnum == 3)
				str2 = "\u0008\u0006";

		const wchar_t* str = new wchar_t[9];
		swprintf((wchar_t*)str, 9, L"\u000E\u000A\u0001%s%d", str2, num);
        MR::setTextBoxArgStringRecursive(pLayout, paneName, str, index);
		delete str;
	}
}
