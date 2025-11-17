#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include "Common.h"
#include "Defines.h"
#include "CWorldParts.h"
#include "CLevelPackFile.h"
#include "pd_api.h"

extern LCDBitmap *IMGBackground,*IMGFloor,*IMGPlayer,*IMGBox,*IMGSpot,*IMGEmpty,*IMGWall,*IMGTitleScreen;
extern LCDFont* font,* BigFont, *RobotoMedium;
extern int GameState;
extern int Selection,MusicCount,SelectedMusic,InstalledLevelPacksCount,InstalledLevels,SelectedLevel,SelectedLevelPack,UnlockedLevels,GetStringId,GetStringX,GetStringY;
extern bool LevelEditorMode,LevelHasChanged,StageReload,ReloadMusic;
extern CWorldParts* WorldParts;
extern CLevelPackFile *LevelPackFile;
extern char *LevelPackName;
extern char InstalledLevelPacks[MaxLevelPacks][MaxLevelPackNameLength];
extern bool isCustomnTitleScreen;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern char *FileName;
extern int AskingQuestionID;
extern size_t MaxLenGetString;
extern bool AskingQuestion;
extern bool AskingGetString;
extern bool showFPS;
extern bool paused;
extern char NormalCreateName[21];
extern PlaydateAPI* pd;
extern PDButtons currButtons, prevButtons;
extern unsigned long framecount, frameUpStart, frameDownStart, frameLeftStart, frameRightStart, frameAStart;
#endif