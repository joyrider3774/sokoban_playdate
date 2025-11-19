#ifndef GAMEFUNCS_H
#define GAMEFUNCS_H


#include <stdbool.h>

void LoadSettings();
void SaveSettings();
void SearchForLevelPacks();
void LoadUnlockData();
void SaveUnlockData();
bool AskQuestionUpdate(int* Id, bool* Answer, bool MustBeAButton);
void AskQuestion(int Id, char* Msg);
char* GetString(int Id, int x, int y,char* Msg, size_t MaxLen);
void getStringDraw(char* StringBuffer, bool ErasingPrevious);
bool getStringUpdate(int *Id, bool* Answered, char* StringBuffer);
void LoadGraphics();
void FindLevels();
void UnLoadGraphics();
void printTitleInfo();
void LoadNormalCreatorName();
char chr(int ascii);
int ord(char chr);
#endif