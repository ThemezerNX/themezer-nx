#pragma once
#include <JAGL.h>

#define COLOR_WHITE COLOR(255,255,255,255)
#define COLOR_GREY COLOR(66,66,68,255)
#define COLOR_LIGHTGREY COLOR(100,100,100,255)
#define COLOR_AQUA COLOR(0, 203, 255, 255)
#define COLOR_BLUEGREY COLOR(50, 80, 100, 255)
#define COLOR_DARKGREY COLOR(57, 57, 59, 255)
#define COLOR_DARKERGREY COLOR(42, 42, 42, 255)
#define COLOR_LESSBLUEGREY COLOR(55, 58, 62, 255)
#define COLOR_MAGENTA COLOR(170, 0, 105, 255)
#define COLOR_GREYMAGENTA COLOR(99, 55, 90, 255)
#define COLOR_DARKERRED COLOR(63, 0, 0, 255)
#define COLOR_DARKRED COLOR(100, 0, 0, 255)
#define COLOR_DARKERORANGE COLOR(114, 86, 0, 255)
#define COLOR_DARKORANGE COLOR(142, 99, 0, 255)
#define COLOR_DARKERBLUE COLOR(0, 52, 61, 255)
#define COLOR_DARKBLUE COLOR(0, 86, 99, 255)
#define COLOR_RED COLOR(255,0,0,255)
#define COLOR_BLUE COLOR(0,0,255,255)
#define COLOR_GREEN COLOR(0,255,0,255)
#define COLOR_ORANGE COLOR(255,182,0,255)

/*
#define COLOR_TOPBAR COLOR(25, 25, 25, 255)
#define COLOR_TOPBARBUTTONS COLOR(61, 135, 181, 255)
#define COLOR_BUTTONBACK COLOR(255, 0, 0, 255)
#define COLOR_BUTTONPOWER COLOR(255,127,0,255)
#define COLOR_TOPBARSELECTION COLOR(50, 80, 100, 255)

#define COLOR_CENTERLISTBG COLOR(63, 63, 63, 255)
#define COLOR_CENTERLISTSELECTION COLOR(89,89,89,255)
#define COLOR_CENTERLISTPRESS COLOR_TOPBARBUTTONS
*/

#define COLOR_HIGHLIGHT COLOR(0,113,165,255)

#define COLOR_TOPBAR COLOR(0, 87, 127, 255)
#define COLOR_TOPBARBUTTONS COLOR_TOPBAR
#define COLOR_BUTTONBACK COLOR(255, 0, 0, 255)
#define COLOR_BUTTONPOWER COLOR(255,127,0,255)
#define COLOR_BUTTONCREATE COLOR_GREEN
#define COLOR_BUTTONCURDIR COLOR_BLUE //COLOR(255, 229, 0, 255)
#define COLOR_TOPBARSELECTION COLOR_HIGHLIGHT

#define COLOR_CENTERLISTBG COLOR(0,52,76,255)
#define COLOR_CENTERLISTSELECTION COLOR_HIGHLIGHT
#define COLOR_CENTERLISTPRESS COLOR(0,156,229,255)

#define COLOR_BTN1 COLOR(255, 144, 0, 255)
#define COLOR_BTN2 COLOR(0, 135, 22, 255)
#define COLOR_BTN3 COLOR(166, 12, 255, 255)
#define COLOR_BTN4 COLOR(0, 255, 203, 255)
#define COLOR_BTN5 COLOR_BLUE

//xtern SDL_Texture *folderIcon;
//xtern SDL_Texture *folderSIcon;
//xtern SDL_Texture *fileSIcon;

void InitDesign();
void ExitDesign();
int FolderExplorer(Context_t *ctx);
int exitFunc(Context_t *ctx);
int ButtonHandlerBExit(Context_t *ctx);
ShapeLinker_t *CreateFileExplorerMenu(ShapeLinker_t *dirList, char *curPath);
ShapeLinker_t *CreateMainMenu(ShapeLinker_t *listItems);