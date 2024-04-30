#ifdef SMSS
#pragma once
#include "JSystem/JKernel/JKRArchive.h"
#include "syati.h"
#include "revolution.h"
#include "Game/Util/BitArray.h"
class MiiFaceParts : public LiveActor
{
};

class FileSelectIconID
{
public:
  enum EFellowID
  {

  };

  FileSelectIconID();
  FileSelectIconID(const FileSelectIconID &);

  void set(const FileSelectIconID &);
  void setMiiIndex(u16);
  bool isMii() const;
  u16 getMiiIndex() const;
  void setFellowID(FileSelectIconID::EFellowID);
  bool isFellow() const;
  FileSelectIconID::EFellowID getFellowID() const;

  bool _0;
  u16 _2;
};

class FileSelectModel : public LiveActor
{
public:
  FileSelectModel(const char *modelName, MtxPtr, const char *jpName);

  void emitOpen();
  void emitVanish();
  void emitCompleteEffect();

  char unk[0x4];
};

// This class has been modified! Do not use for documentation purposes!
struct FileSelectModelArray
{
public:
  FileSelectModel *models[25];
};

class FileSelectNumber : public LayoutActor {
public:
  FileSelectNumber(const char *);
  virtual void init(const JMapInfoIter &);
  virtual void appear();
  void disappear();
  void setNumber(s32);
  void onSelectIn();
  void onSelectOut();
  virtual void control();
  u8 _0[0x38];
  NerveExecutor *_38;
};

class FileSelectItem : public LiveActor
{
public:
  void createFellows();
  char _90[0x4];                    // _90
  PartsModel *partModel;            // _94
  const FileSelectIconID &iconID;   // _98
  FileSelectModelArray *modelArray; // _9C
  MiiFaceParts *_A0;
  char _A4[0x4];
  TMtx34f _A8;
  TMtx34f _D8;
  TMtx34f _108;
  char _138[0x13];
  bool isComplete; // _14B
};

class MiiSelectIcon : public LayoutActor
{
public:
  MiiSelectIcon(int, int, int, const char *unk);
  void appearMiiDummy();
  void appear();
};

class MiiSelect : public LayoutActor
{
public:
  u16 getIconNum(); // Icon Count
  u16 getIconID(const FileSelectIconID &, const long);
  char unk[0x8];
  MR::BitArray *bitFlags; //_34
  u16 iconCount;          //_38
};
#endif