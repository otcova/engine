#include <cmath>
#include <iostream>
#include "board.h"


StoreWireChunk::StoreWireChunk(const WireChunk& wCh) : wiresActive(wCh.wiresActive), wiresNode(wCh.wiresNode) {}

std::bitset<WireChunkSize * WireChunkSize * 2>::reference Board::getWire(int x, int y, int d) {
    return wiresMap[getWireChunkCoords(x, y).hash()].global(x, y, d);
}

std::bitset<WireChunkSize * WireChunkSize>::reference Board::getWireNode(int x, int y) {
    return wiresMap[getWireChunkCoords(x, y).hash()].globalNode(x, y);
}

int& Board::getWireGrup(int x, int y, int d) {
    return wiresMap[getWireChunkCoords(x, y).hash()].globalGrup(x, y, d);
}

Pos Board::getWireChunkCoords(int x, int y)
{
    return Pos((int)std::floor((double)x / (double)WireChunkSize), (int)std::floor((double)y / (double)WireChunkSize));
}

Pos Board::getObjChunkCoords(int x, int y)
{
    return Pos((int)std::floor((double)x / (double)ObjChunkSize), (int)std::floor((double)y / (double)ObjChunkSize));
}

Obj* Board::getObjAt(int x, int y)
{
    std::map<int64_t, ObjChunk>::iterator wchIt = objsMap.find(getObjChunkCoords(x, y).hash());
    if (wchIt != objsMap.end()) {
        for (int i = 0; i < wchIt->second.objs.size(); i++) {
            if (checkCollide(x, y, 0, 0, wchIt->second.objs[i].x, wchIt->second.objs[i].y,
                wchIt->second.objs[i].getWidth(), wchIt->second.objs[i].getHeight())) {
                return &wchIt->second.objs[i];
            }
        }
    }

    return nullptr;
}