#pragma once

#include <map>
#include <string>
#include <vector>
#include <stdint.h>
#include <iostream>
#include <bitset>

#define WireChunkSize 8
#define ObjChunkSize 64

inline int mod(int x, int m)
{
    return (x % m + m) % m;
}

struct Pos
{
    int32_t x, y;
    Pos(int x, int y) : x(x), y(y) {}
    inline int64_t hash() const
    {
        return *((int64_t *)(void *)this); // ((int64_t)x) << 32 | y;
    }
};

struct WireChunk;

struct StoreWireChunk
{
    std::bitset<WireChunkSize * WireChunkSize * 2> wiresActive;
    std::bitset<WireChunkSize * WireChunkSize> wiresNode;
    StoreWireChunk() {}
    StoreWireChunk(const WireChunk &wCh);

    inline std::bitset<WireChunkSize * WireChunkSize * 2>::reference global(int x, int y, int d) { return wiresActive[mod(x, WireChunkSize) * 2 * WireChunkSize + mod(y, WireChunkSize) * 2 + d]; }
};

struct WireChunk
{
    //Wire wires[WireChunkSize * WireChunkSize * 2];
    std::bitset<WireChunkSize * WireChunkSize * 2> wiresActive;
    std::bitset<WireChunkSize * WireChunkSize> wiresNode;
    int wiresGrup[WireChunkSize * WireChunkSize * 2];

    WireChunk()
    {
        for (int i = 0; i < WireChunkSize * WireChunkSize * 2; i++)
        {
            wiresGrup[i] = -1;
        }
    }
    WireChunk(const StoreWireChunk &swCh) : wiresActive(swCh.wiresActive), wiresNode(swCh.wiresNode)
    {
        for (int i = 0; i < WireChunkSize * WireChunkSize * 2; i++)
        {
            wiresGrup[i] = -1;
        }
    }

    inline bool is_empty() const {
        return wiresActive.none();
    }
    
    inline std::bitset<WireChunkSize * WireChunkSize * 2>::reference local(int x, int y, int d) { return wiresActive[x * 2 * WireChunkSize + y * 2 + d]; }
    inline std::bitset<WireChunkSize * WireChunkSize * 2>::reference global(int x, int y, int d) { return wiresActive[mod(x, WireChunkSize) * 2 * WireChunkSize + mod(y, WireChunkSize) * 2 + d]; }
    inline std::bitset<WireChunkSize * WireChunkSize>::reference localNode(int x, int y) { return wiresNode[x * WireChunkSize + y]; }
    inline std::bitset<WireChunkSize * WireChunkSize>::reference globalNode(int x, int y) { return wiresNode[mod(x, WireChunkSize) * WireChunkSize + mod(y, WireChunkSize)]; }
    inline int &localGrup(int x, int y, int d) { return wiresGrup[x * 2 * WireChunkSize + y * 2 + d]; }
    inline int &globalGrup(int x, int y, int d) { return wiresGrup[mod(x, WireChunkSize) * 2 * WireChunkSize + mod(y, WireChunkSize) * 2 + d]; }
};

enum DIR : char
{
    RIGHT = 0,
    DOWN,
    LEFT,
    UP
};
struct ObjType;

struct WireConectionType
{
    int x, y, d;
    bool defNot, hide;

    inline void rotate(const ObjType &objt, int r);
};

struct ObjType
{
    int w, h;
    std::vector<WireConectionType> c; // wire conections inputs / outputs
};

struct Action
{
    enum Type : char
    {
        SetSegWire,
        SwapNode,
        SetObj,
        SwapObjNot,
        Select
    };
    Type type;

    bool set;
    int x, y, typeID;

    int d;
    int rotate;

    std::vector<char> wireNot;

    Action(const Type &type)
        : type(type) {}

    Action(bool set, int x, int y, int d)
        : type(Type::SetSegWire), set(set), x(x), y(y), d(d) {}

    Action(int x, int y)
        : type(Type::SwapNode), x(x), y(y) {}

    Action(bool set, int x, int y, int typeID, int rotate)
        : type(Type::SetObj), set(set), x(x), y(y), typeID(typeID), rotate(rotate) {}

    Action(bool set, int x, int y, int typeID, int rotate, const std::vector<char> &wireNot)
        : type(Type::SetObj), set(set), x(x), y(y), typeID(typeID), rotate(rotate), wireNot(wireNot) {}

    Action(int x, int y, int d)
        : type(Type::SwapObjNot), x(x), y(y), d(d) {}
};

struct sWire
{
    int x, y, d;
    sWire(int x, int y, int d) : x(x), y(y), d(d) {}
};
struct sNode
{
    int x, y;
    sNode(int x, int y) : x(x), y(y) {}
};

inline void WireConectionType::rotate(const ObjType &objt, int r)
{
    if (x != -1 && y != -1)
    {
        x += 1 - d;
        y += d;
    }

    int rx;
    int ry;
    if (r == 0)
    {
        rx = x;
        ry = y;
    }
    else if (r == 1)
    {
        rx = y;
        ry = objt.w - x;
        d = !d;
    }
    else if (r == 2)
    {
        rx = objt.w - x;
        ry = objt.h - y;
    }
    else
    {
        rx = objt.h - y;
        ry = objt.w - objt.w + x;
        d = !d;
    }

    x = rx;
    y = ry;

    if (x != -1 && y != -1)
    {
        x -= 1 - d;
        y -= d;
    }
}

const ObjType &getObjType(int typeID);

struct Obj
{
    int x, y, typeID, rotate;
    std::vector<char> memory;
    std::vector<char> wireNot;

    Obj(int x, int y, int typeID, int rotate)
        : x(x), y(y), typeID(typeID), rotate(rotate)
    {
        const ObjType &objt = getObjType(typeID);
        wireNot.reserve(objt.c.size());
        for (int i = 0; i < objt.c.size(); i++)
            wireNot.push_back(objt.c[i].defNot);
    }

    inline int getWidth() const
    {
        return rotate % 2 == 0 ? getObjType(typeID).w : getObjType(typeID).h;
    }
    inline int getHeight() const
    {
        return rotate % 2 == 0 ? getObjType(typeID).h : getObjType(typeID).w;
    }
};

inline bool checkCollide(const int &x1, const int &y1, const int &w1, const int &h1, const int &x2, const int &y2, const int &w2, const int &h2)
{
    if (y1 + h1 < y2 || y1 > y2 + h2 || x1 + w1 < x2 || x1 > x2 + w2)
        return false;
    return true;
}

struct CopyPasteData
{
    std::vector<sWire> wires;
    std::vector<sNode> nodes;
    std::vector<Obj> objs;
    int x, y, w, h;
};

struct ObjChunk
{
    std::vector<Obj> objs;
};

struct Board
{
    bool needRefresh = false;
    std::map<int64_t, WireChunk> wiresMap;
    std::map<int64_t, ObjChunk> objsMap;

    Pos getWireChunkCoords(int x, int y);
    Pos getObjChunkCoords(int x, int y);
    std::bitset<WireChunkSize * WireChunkSize * 2>::reference getWire(int x, int y, int d);
    std::bitset<WireChunkSize * WireChunkSize>::reference getWireNode(int x, int y);
    int &getWireGrup(int x, int y, int d);
    Obj *getObjAt(int x, int y);
};