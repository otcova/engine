#pragma once

#include <bitset>
#include "board.h"

struct StoreWireChunk;
struct Obj;

bool canAddWire(int boardID, int ax, int ay, int bx, int by, int cx, int cy);
void setWire(int boardID, bool set, int ax, int ay, int bx, int by, int cx, int cy);
void client_swapNode(int x, int y);
void drawWires(int boardID, double scale, double rectX, double rectY, double rectW, double rectH);


bool canAddObj(int boardID, int x, int y, int typeID, int rotate);
void addObj(int boardID, int x, int y, int objName, int rotate, bool makeUndo = true);
bool swapObjNot(int x, int y, int d, bool makeUndo = true);
void removeObj(int boardID, int x, int y, bool makeUndo = true);
void drawObjs(int boardID, double scale, double rectX, double rectY, double rectW, double rectH);
void drawObj(const Obj& obj);

void initRun(int boardID);
void runStep(int boardID, bool autoRefresh);
void stopRun(int boardID);
bool clickEvent(int boardID, int x, int y, bool make_undo = true);

bool selectRect(int rectX, int rectY, int rectW, int rectH);
void moveSelected(int dx, int dy);
void rotateSelected(int r);
void deleteSelected();
void unSelect();
void drawSelect(double scale, double boardX, double boardY);
void duplicateSelected();
void copySelected();
bool pasteSelected();


void undoAction();
void reverseUndo();

void removeEmptyWireChunks();

void LoadBoard(void* data);
void* SaveBoard();