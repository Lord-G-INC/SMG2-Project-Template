#include "pt/MapObj/RedCoinSystem/RedCoinLayouts.h"

RedCoinCounter::RedCoinCounter(const char* pName) : LayoutActor(pName, false) {
    mPaneRumbler = 0;
    mRedCoinCount = 0;
    mHasAllRedCoins = 0;
}

void RedCoinCounter::init(const JMapInfoIter& rIter) {
    initLayoutManager("RedCoinCounter", 2);
    initEffectKeeper(0, 0, 0);

    MR::registerDemoSimpleCastAll(this);
    MR::connectToSceneLayout(this);
    MR::createAndAddPaneCtrl(this, "Counter", 1);
    MR::setTextBoxNumberRecursive(this, "Counter", 0);

    mPaneRumbler = new CountUpPaneRumbler(this, "Counter");
    mPaneRumbler->mRumbleCalculator->mRumbleStrength = 8.0f;
    mPaneRumbler->reset();

    initNerve(&NrvRedCoinCounter::NrvHide::sInstance);
}

void RedCoinCounter::control() {
    mPaneRumbler->update();
}

void RedCoinCounter::setStarIcon(s32 starID, s32 iconID) {
    wchar_t str;
    MR::addPictureFontCode(&str, MR::hasPowerStarInCurrentStage(starID) ? iconID : 0x52);
    MR::setTextBoxFormatRecursive(this, "TxtStar", &str);
}

void RedCoinCounter::startCountUp(s32 count, bool hasAllCoins) {
    mRedCoinCount = count;
    mHasAllRedCoins = hasAllCoins;

    if (mRedCoinCount == 1) {
        appear();
        setNerve(&NrvRedCoinCounter::NrvAppearWithUpdate::sInstance);
    }
    else 
        setNerve(&NrvRedCoinCounter::NrvCountUp::sInstance);
}

void RedCoinCounter::exeAppear() {
    if (MR::isFirstStep(this)) {
        MR::startAnim(this, "Appear", 0);
        MR::startAnim(this, "Wait", 1);
    }
}

void RedCoinCounter::exeAppearWithUpdate() {
    if (MR::isFirstStep(this)) {
        MR::startAnim(this, "Appear", 0);
        MR::startAnim(this, "Wait", 1);
        OSReport("Yes\n");
    }

    if (MR::isStep(this, 30))
        setNerve(&NrvRedCoinCounter::NrvCountUp::sInstance);
}

void RedCoinCounter::exeDisappear() {
    if (MR::isFirstStep(this))
        MR::startAnim(this, "End", 0);

    if (MR::isStep(this, 60))
        kill();
}

void RedCoinCounter::exeCountUp() {
    if (MR::isFirstStep(this)) {
        MR::setTextBoxNumberRecursive(this, "Counter", mRedCoinCount);
        MR::emitEffect(this, "RedCoinCounterLight");
        mPaneRumbler->start();

    if (mHasAllRedCoins)
        setNerve(&NrvRedCoinCounter::NrvComplete::sInstance);
    else 
        MR::startPaneAnim(this, "Counter", "Flash", 0);
    }
}

void RedCoinCounter::exeComplete() {
    if (MR::isFirstStep(this))
        MR::startPaneAnim(this, "Counter", "FlashLoop", 0);

    if (MR::isStep(this, 120))
        setNerve(&NrvRedCoinCounter::NrvDisappear::sInstance);
}

namespace NrvRedCoinCounter {
    void NrvAppear::execute(Spine* pSpine) const {
        ((RedCoinCounter*)pSpine->mExecutor)->exeAppear();
    }

    void NrvAppearWithUpdate::execute(Spine* pSpine) const {
        ((RedCoinCounter*)pSpine->mExecutor)->exeAppearWithUpdate();
    }

    void NrvDisappear::execute(Spine* pSpine) const {
        ((RedCoinCounter*)pSpine->mExecutor)->exeDisappear();
    }

    void NrvCountUp::execute(Spine* pSpine) const {
        ((RedCoinCounter*)pSpine->mExecutor)->exeCountUp();
    }

    void NrvComplete::execute(Spine* pSpine) const {
        ((RedCoinCounter*)pSpine->mExecutor)->exeComplete();
    }

    void NrvHide::execute(Spine* pSpine) const {
    }

    NrvAppear(NrvAppear::sInstance);
    NrvAppearWithUpdate(NrvAppearWithUpdate::sInstance);
    NrvDisappear(NrvDisappear::sInstance);
    NrvCountUp(NrvCountUp::sInstance);
    NrvComplete(NrvComplete::sInstance);
    NrvHide(NrvHide::sInstance);
}