#ifdef SMSS
#include "syati.h"
#include "pt/LayoutActor/FileSelectData.h"
#include "pt/Util/ActorUtil.h"
#include "revolution.h"
#include "kamek/hooks.h"
#include "JSystem/JKernel/JKRArchive.h"
#include "pt/init.h"

/* ---------------------------------------------------------------------
   FileSelectData
   This is in charge of custom file icons.
   Author: AwesomeTMC
*/

/* ---------------------------------------------------------------------
   File Select Table loading and unloading
*/


s32 tableNum;

void loadFileDataTable(LiveActor *pActor)
{
  // This overwrites a call to this, so we must call this.
  MR::invalidateClipping(pActor);

  // Load arc and set variables.
  if (MR::isFileExist("/ObjectData/FileSelectData.arc", false))
  {
    JMapInfo JMapTable = JMapInfo();
    JMapTable.attach(gFileSelectDataTable);
    tableNum = MR::getCsvDataElementNum(&JMapTable);
  }
  else
  {
    u32 fg = 0xFFFFFFFF;
    u32 bg = 0x00000000;
    const char *msg = "Please check PTD Wiki for FileSelectData.arc template";
    OSFatal(&fg, &bg, msg);
  }
}
kmCall(0x8024F3C0, loadFileDataTable);

/* ---------------------------------------------------------------------
   File Icon Messages
*/

// Changes from an array to use the iconID -- MAX LENGTH 12 chars!!!! (it will cut off otherwise)
const wchar_t *getFileIconMessage(int iconID)
{
  char iconMsg[120];
  snprintf(iconMsg, 120, "System_FileSelect_Icon%03d", iconID);
  const wchar_t *newIconMsg = MR::getGameMessageDirect(iconMsg);
  if (newIconMsg == 0)
    return L"InvalidName";
  return newIconMsg;
}

// replacing the original way of getting the message
kmWrite32(0x8024C6AC, 0x60000000);
kmWrite32(0x8024C6B0, 0x60000000);
kmCall(0x8024C6B4, getFileIconMessage);

/* ---------------------------------------------------------------------
   File Icon Models
*/

void createFellows(FileSelectItem item)
{
  JMapInfo JMapTable = JMapInfo();
  JMapTable.attach(gFileSelectDataTable);
  for (int i = 0; i < tableNum; i++)
  {
    // Get model name
    const char *modelName = "";
    MR::getCsvDataStrOrNULL(&modelName, &JMapTable, "ModelName", i);
    if (MR::isNullOrEmptyString(modelName))
      modelName = "FileSelectDataMario"; // fallback if it fails to get the icon

    // Make model
    item.modelArray->models[i] = new FileSelectModel(modelName, (MtxPtr)&item._D8, "キャラフェイス");
  }
}

kmBranch(0x8024D5F0, createFellows);
// adjust slots to allow for 25 icons
kmWrite32(0x8024c9e4, 0x38600064);
kmWrite32(0x80483EF0, 0x38600070);
kmWrite32(0x8024c9ec, 0x38000019);

// 1:1 decompilation if you change "tableNum" to 7.
void killAllModels(FileSelectItem item)
{
  item.partModel->makeActorDead();
  for (int i = 0; i < tableNum; i++)
  {
    item.modelArray->models[i]->makeActorDead();
  }
  item._A0->makeActorDead();
}
kmBranch(0x8024E030, killAllModels);

// Remove value clamping
kmWrite32(0x80250E54, 0x60000000);
kmWrite32(0x80250E58, 0x60000000);

/* ---------------------------------------------------------------------
   File Icon Particles
*/

// 0 - Open; 1 - Vanish; 2 - Complete
void emitFileEffect(FileSelectItem item, const char *effectName, int effectNum)
{
  if (!MR::isDead(item.partModel) && effectNum != 2)
    MR::emitEffect(item.partModel, effectName);
  for (int i = 0; i < tableNum; i++)
  {
    if (!MR::isDead(item.modelArray->models[i]))
    {
      switch (effectNum)
      {
      case 0:
        item.modelArray->models[i]->emitOpen();
        break;
      case 1:
        item.modelArray->models[i]->emitVanish();
        break;
      case 2:
        item.modelArray->models[i]->emitCompleteEffect();
        break;
      }
    }
  }
  if (!MR::isDead(item._A0))
    MR::emitEffect(item._A0, effectName);
}

void emitOpen(FileSelectItem item)
{
  emitFileEffect(item, "Open", 0);
}
kmBranch(0x8024E0C0, emitOpen);

void emitVanish(FileSelectItem item)
{
  emitFileEffect(item, "Vanish", 1);
}
kmBranch(0x8024E170, emitVanish);

void emitCompleteEffect(FileSelectItem item)
{
  if (item.isComplete)
    emitFileEffect(item, "Complete", 2);
}
kmBranch(0x8024E2D0, emitCompleteEffect);

/* ---------------------------------------------------------------------
   File Icon Images
*/

#define ROSALINA_LOCK false // set to true if you want Rosalina's Icon to be locked behind getting 120 stars
void invalidateAllUnusedIcon(MiiSelect pSelect, bool isGet120StarEnding)
{
  // Invalidate rosalina's icon if needed
  if (ROSALINA_LOCK && pSelect.bitFlags->isOn(5) && !isGet120StarEnding)
  {
    pSelect.bitFlags->set(5, false);
    pSelect.iconCount -= 1;
  }

  // Invalidate unused icons
  for (int i = tableNum; i < pSelect.bitFlags->mFlagCount; i++)
  {
    if (pSelect.bitFlags->isOn(i))
    {
      pSelect.bitFlags->set(i, false);
      pSelect.iconCount -= 1;
    }
  }
}

// modify code so it passes the bool onto invalidateAllUsedIcon
kmWrite32(0x8025220c, 0x7c641b78); // mr r4, r3
kmWrite32(0x80252210, 0x60000000); // nop
kmWrite32(0x80252218, 0x60000000); // nop

kmWrite32(0x804831e8, 0x38800019);
kmCall(0x8025221C, invalidateAllUnusedIcon);

float getIconFloat(int fellowID)
{
  JMapInfo JMapTable = JMapInfo();
  JMapTable.attach(gFileSelectDataTable);
  float fellowFloat = 0.0;
  MR::getCsvDataF32(&fellowFloat, &JMapTable, "play_frame", fellowID);
  return fellowFloat;
}
kmCall(0x80484A90, getIconFloat);
kmWrite32(0x80484A94, 0x60000000);
kmWrite32(0x80484A98, 0xD0210008);
kmWrite32(0x80484AA0, 0x60000000);
kmWrite32(0x80484ad0, 0xc0210008);
#endif