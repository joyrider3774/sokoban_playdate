#ifndef DEFINES_H
#define DEFINES_H
#define ORIG_WINDOW_WIDTH 400
#define ORIG_WINDOW_HEIGHT 240 
#define SCREEN_BPP 32
#define FPS 50
#define UI_WIDTH_SCALE 1.25
#define UI_HEIGHT_SCALE 1.10
#define NrOfRows 15
#define NrOfCols 25
#define MaxHistory 1000
#define TileWidth 16
#define TileHeight 16
#define ZEmpty 6
#define ZPlayer 5
#define ZBox 4
#define ZWall 3
#define ZSpot 2
#define ZFloor 1
#define IDPlayer 1
#define IDBox 2
#define IDWall 3
#define IDSpot 4
#define IDEmpty 5
#define IDFloor 6
#define MaxLevelPacks 1000
#define MaxMusicFiles 26
#define NrOfSounds 6
#define SND_MENU 0
#define SND_SELECT 1
#define SND_ERROR 2
#define SND_STAGEEND 3
#define SND_BACK 4
#define SND_MOVE 5
#define InputDelay 16
#define MaxLevelPackNameLength 100

#define GSTitleScreen 1 
#define GSCredits 2
#define GSGame 3 
#define GSStageSelect 4
#define GSLevelEditor 5 
#define GSLevelEditorMenu 6


#define GSTitleScreenInit 51
#define GSCreditsInit 52
#define GSGameInit 53
#define GSStageSelectInit 54
#define GSLevelEditorInit 55 
#define GSLevelEditorMenuInit 56

#define IDSolvedLevelLevelEditorMode 1
#define IDSolvedLevelNextUnlocked 2
#define IDSolvedLastLevel 3
#define IDSolvedEarlierLevel 4
#define IDNoPlayer 5
#define IDMoreSpotsThanBoxes 6
#define IDNoLevelsInPack 7
#define IDCurrentLevelNotSaved 8
#define IDRestartLevel 9
#define IDDeleteAllParts 10
#define IDDeleteLevelPack 11
#define IDDeleteLevel 12
#define IDLevelNotUnlocked 13
#define IDLevelInfo 14

#define MenuUpdateTicks 10
#define LevelEditorUpdateTicks 5

#endif