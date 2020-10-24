#pragma once

#include "board.h"

void initTypes();
const ObjType& getObjType(int typeID);

void logic(Obj& obj, Board& board, std::vector<bool>& grupsGet, std::vector<bool>& grupsSet);