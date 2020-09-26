#pragma once

#include "board.h"
/*
#include <vector>

void runLogic(int typeID, std::vector<bool&>& v);*/

void initTypes();
const ObjType& getObjType(int typeID);

void logic(Obj& obj, Board& board, std::vector<bool>& grupsGet, std::vector<bool>& grupsSet, bool draw);