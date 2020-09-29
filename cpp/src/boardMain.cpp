#ifdef __INTELLISENSE__
#define EM_ASM_INT()
#define EM_ASM()
#endif

#include <emscripten.h>
#include <iostream>
#include <vector>
#include <map>
#include "board.h"
#include "logic.h"
#include "boardMain.h"

Board board;
std::vector<bool> grups;

std::vector<std::vector<Action>> actionsRegister;
std::vector<std::vector<Action>> undoActionsRegister;

void pushActionGrup(const std::vector<Action>& actionGrup) {
    if (actionGrup.size() > 0) {
    undoActionsRegister.clear();
    actionsRegister.emplace_back(actionGrup);
    if (actionsRegister.size() > 70) 
        actionsRegister.erase(actionsRegister.begin());
    }
}

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

void stopRun(int boardID) {
    for (std::map<int64_t, WireChunk>::iterator it = board.wiresMap.begin(); it != board.wiresMap.end(); ++it) {
        for (int i = 0; i < WireChunkSize * WireChunkSize * 2; i++) {
            it->second.wiresGrup[i] = -1;
        }
    }

    for (std::map<int64_t, ObjChunk>::iterator it = board.objsMap.begin(); it != board.objsMap.end(); ++it) {
        for (int i = 0; i < it->second.objs.size(); ++i) {
            it->second.objs[i].memory.clear();
        }
    }

    grups.clear();
}

bool objWiresCollision(const Obj& obj) {
    const ObjType& objt = getObjType(obj.typeID);
    int objxw = obj.x + obj.getWidth();
    int objyh = obj.y + obj.getHeight();

    for (int x = obj.x; x <= objxw; x++)
    for (int y = obj.y - 1; y <= objyh; y++) {
        if (board.getWire(x, y, 1)) {
            for (const auto& cType : objt.c) {
                auto c = cType;
                c.rotate(objt, obj.rotate);
                if (x == c.x + obj.x && y == c.y + obj.y && c.d == 1)
                    goto vException;
            }
            return true;
        vException:
            continue;
        }
    }

    for (int x = obj.x - 1; x <= objxw; x++)
    for (int y = obj.y; y <= objyh; y++) {
        if (board.getWire(x, y, 0)) {
            for (const auto& cType : objt.c) {
                auto c = cType;
                c.rotate(objt, obj.rotate);
                if (x == c.x + obj.x && y == c.y + obj.y && c.d == 0)
                    goto hException;
            }
            return true;
        hException:
            continue;
        }
    }

    return false;
}

bool canAddSegWire(int x, int y, int d)
{
    if (!board.getWire(x, y, d))
    {
        board.getWire(x, y, d) = true;

        Pos ch = board.getObjChunkCoords(x, y);

        for (int chx = ch.x - 1; chx <= ch.x; chx++)
        for (int chy = ch.y - 1; chy <= ch.y; chy++)
        {
            std::map<int64_t, ObjChunk>::iterator wchIt = board.objsMap.find(Pos(chx, chy).hash());
            if (wchIt != board.objsMap.end()) {
                for (int i = 0; i < wchIt->second.objs.size(); i++) {
                    if (objWiresCollision(wchIt->second.objs[i])) {
                        board.getWire(x, y, d) = false;
                        return false;
                    }
                }
            }
        }

        board.getWire(x, y, d) = false;
    }
    return true;
}

bool canAddAllSegWire(Pos topLeftCh, Pos bottomRightCh) {
    for (int chx = topLeftCh.x - 1; chx <= bottomRightCh.x; chx++)
    for (int chy = topLeftCh.y - 1; chy <= bottomRightCh.y; chy++)
    {
        std::map<int64_t, ObjChunk>::iterator wchIt = board.objsMap.find(Pos(chx, chy).hash());
        if (wchIt != board.objsMap.end()) {
            for (int i = 0; i < wchIt->second.objs.size(); i++) {
                if (objWiresCollision(wchIt->second.objs[i])) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool canAddWire(int boardID, int ax, int ay, int bx, int by, int cx, int cy)
{
    if (ay == by) {
        if (ax == bx) {
            if (by != cy) {
                for (int y = MIN(by, cy); y < MAX(by, cy); ++y)
                    if (!canAddSegWire(bx, y, 1)) return false;
            }
            else {
                for (int x = MIN(bx, cx); x < MAX(bx, cx); ++x)
                    if (!canAddSegWire(x, by, 0)) return false;
            }
        }
        else {
            for (int x = MIN(ax, bx); x < MAX(ax, bx); ++x)
                if (!canAddSegWire(x, ay, 0)) return false;
            for (int y = MIN(by, cy); y < MAX(by, cy); ++y)
                if (!canAddSegWire(bx, y, 1)) return false;
        }
    }
    else {
        for (int y = MIN(ay, by); y < MAX(ay, by); ++y)
            if (!canAddSegWire(ax, y, 1)) return false;
        for (int x = MIN(bx, cx); x < MAX(bx, cx); ++x)
            if (!canAddSegWire(x, by, 0)) return false;
    }

    return true;
}

void changeNode(int x, int y) {
    int count = 0;
    if (board.getWire(x, y, 0)) count++;
    if (board.getWire(x, y, 1)) count++;
    if (board.getWire(x - 1, y, 0)) count++;
    if (board.getWire(x, y - 1, 1)) count++;

    if (count < 3) {
        board.getWireNode(x, y) = false;
    }
    else if (count == 3) {
        board.getWireNode(x, y) = true;
    }
}

void setSegWire(std::vector<Action>& actionGrup, bool set, int x, int y, int d) {
    auto wire = board.getWire(x, y, d);
    if (wire != set) {
        wire = set;
        actionGrup.emplace_back(set, x, y, d);
    }
}

void setWire(int boardID, bool set, int ax, int ay, int bx, int by, int cx, int cy)
{
    if (set) {
        if (!canAddWire(boardID, ax, ay, bx, by, cx, cy)) return;
    }

    std::vector<Action> actionGrup;

    if (ay == by) {
        if (ax == bx) {
            if (by != cy) {
                for (int y = MIN(by, cy); y < MAX(by, cy); ++y)
                    setSegWire(actionGrup, set, bx, y, 1);
                for (int y = MIN(by, cy); y <= MAX(by, cy); ++y)
                    changeNode(bx, y);
            }
            else {
                for (int x = MIN(bx, cx); x < MAX(bx, cx); ++x)
                    setSegWire(actionGrup, set, x, by, 0);
                for (int x = MIN(bx, cx); x <= MAX(bx, cx); ++x)
                    changeNode(x, bx);
            }
        }
        else {
            for (int x = MIN(ax, bx); x < MAX(ax, bx); ++x)
                setSegWire(actionGrup, set, x, ay, 0);
            for (int y = MIN(by, cy); y < MAX(by, cy); ++y)
                setSegWire(actionGrup, set, bx, y, 1);

            for (int x = MIN(ax, bx); x <= MAX(ax, bx); ++x)
                changeNode(x, ay);
            for (int y = MIN(by, cy); y <= MAX(by, cy); ++y)
                changeNode(bx, y);
        }
    }
    else {
        for (int y = MIN(ay, by); y < MAX(ay, by); ++y)
            setSegWire(actionGrup, set, ax, y, 1);
        for (int x = MIN(bx, cx); x < MAX(bx, cx); ++x)
            setSegWire(actionGrup, set, x, by, 0);

        for (int y = MIN(ay, by); y <= MAX(ay, by); ++y)
            changeNode(ax, y);
        for (int x = MIN(bx, cx); x <= MAX(bx, cx); ++x)
            changeNode(x, by);
    }

    pushActionGrup(actionGrup);
}

void swapNode(int boardID, int x, int y, bool makeUndo) 
{
    std::vector<Action> actionGrup;

    int count = 0;
    if (board.getWire(x, y, 0)) count++;
    if (board.getWire(x, y, 1)) count++;
    if (board.getWire(x - 1, y, 0)) count++;
    if (board.getWire(x, y - 1, 1)) count++;


    if (count < 3) {
        auto node = board.getWireNode(x, y);
        if (node != false) {
            node = false;
            actionGrup.emplace_back(x, y);
        }
    }
    else if (count == 3) {
        auto node = board.getWireNode(x, y);
        if (node != true) {
            node = true;
            actionGrup.emplace_back(x, y);
        }
    }
    else {
        board.getWireNode(x, y) = !board.getWireNode(x, y);
        actionGrup.emplace_back(x, y);
    }

    if (makeUndo) pushActionGrup(actionGrup);
}

void drawWires(int boardID, double scale, double rectX, double rectY, double rectW, double rectH)
{
    EM_ASM(canvas.ctxw.fillStyle = "#3E3");
    Pos chA = board.getWireChunkCoords(rectX, rectY);
    Pos chB = board.getWireChunkCoords(rectX + rectW, rectY + rectH);

    for (int style = 0; style < 2; style++) {
        EM_ASM({
            canvas.ctxw.lineWidth = $0;
            canvas.ctxw.beginPath();
            }, style * 2 + 1);
        for (int chx = chA.x - 1; chx <= chB.x; chx++)
        for (int chy = chA.y - 1; chy <= chB.y; chy++)
        {
            std::map<int64_t, WireChunk>::iterator wchIt = board.wiresMap.find(Pos(chx, chy).hash());
            if (wchIt != board.wiresMap.end()) {
                WireChunk& wch = wchIt->second;
                for (int x = 0; x < WireChunkSize; x++)
                    for (int y = 0; y < WireChunkSize; y++)
                    {
                        if (wch.local(x, y, 0)) {
                            int g = wch.localGrup(x, y, 0);
                            if (g != -1) if (!grups[g]) g = -1;
                            if ((style == 0) == (g == -1)) {
                                EM_ASM({
                                    canvas.ctxw.moveTo($0 - $3, $1 + 0.5);
                                    canvas.ctxw.lineTo($0 + $2 + $3*2, $1 + 0.5);
                                    }, (x + chx * WireChunkSize - rectX)* scale, (y + chy * WireChunkSize - rectY)* scale, scale, style);
                            }
                        }
                        if (wch.local(x, y, 1)) {
                            int g = wch.localGrup(x, y, 1);
                            if (g != -1) if (!grups[g]) g = -1;
                            if ((style == 0) == (g == -1)) {
                                EM_ASM({
                                    canvas.ctxw.moveTo($0 + 0.5, $1 - $3);
                                    canvas.ctxw.lineTo($0 + 0.5, $1 + $2 + $3*2);
                                    }, (x + chx * WireChunkSize - rectX)* scale, (y + chy * WireChunkSize - rectY)* scale, scale, style);
                            }
                        }
                        if (style == 1) {
                            if (wch.localNode(x, y)) {
                                EM_ASM({
                                    canvas.ctxw.fillRect($0, $1, 5, 5);
                                    }, int((x + chx * WireChunkSize - rectX) * scale - 2), int((y + chy * WireChunkSize - rectY) * scale - 2));
                            }
                        }
                    }
            }
        }

        EM_ASM(canvas.ctxw.stroke());
    }
}

void removeObj(int boardID, int x, int y, bool makeUndo)
{
    Pos chPos = board.getObjChunkCoords(x, y);
    for (int cy = chPos.y - 1; cy <= chPos.y + 1; ++cy)
    for (int cx = chPos.x - 1; cx <= chPos.x + 1; ++cx) {
        std::map<int64_t, ObjChunk>::iterator wchIt = board.objsMap.find(Pos(cx, cy).hash());
        if (wchIt != board.objsMap.end()) {
            for (int i = 0; i < wchIt->second.objs.size(); i++) {
                if (checkCollide(x, y, 0, 0, wchIt->second.objs[i].x, wchIt->second.objs[i].y,
                    wchIt->second.objs[i].getWidth(), wchIt->second.objs[i].getHeight())) {
                    if (makeUndo) {
                        std::vector<Action> actionGrup;
                        actionGrup.emplace_back(false, wchIt->second.objs[i].x, wchIt->second.objs[i].y, wchIt->second.objs[i].typeID, wchIt->second.objs[i].rotate, wchIt->second.objs[i].wireNot);
                        pushActionGrup(actionGrup);
                    }
                    wchIt->second.objs.erase(wchIt->second.objs.begin() + i);
                    return;
                }
            }
        }
    }
}

bool canAddObj(int boardID, int x, int y, int typeID, int rotate)
{
    const Obj thisObj(x, y, typeID, rotate);
    int w = thisObj.getWidth();
    int h = thisObj.getHeight();

    Pos chA = board.getObjChunkCoords(x, y);
    Pos chB = board.getObjChunkCoords(x+ w, y + h);

    for (int chx = chA.x-1; chx <= chB.x; chx++)
    for (int chy = chA.y-1; chy <= chB.y; chy++)
    {
        std::map<int64_t, ObjChunk>::iterator wchIt = board.objsMap.find(Pos(chx, chy).hash());
        if (wchIt != board.objsMap.end()) {
            for (int i = 0; i < wchIt->second.objs.size(); i++) {
                if (checkCollide(x, y, w, h, wchIt->second.objs[i].x, wchIt->second.objs[i].y, 
                    wchIt->second.objs[i].getWidth(), wchIt->second.objs[i].getHeight())) {
                    return false;
                }
            }
        }
    }

    return !objWiresCollision(thisObj);
}

bool swapObjNot(int boardID, int x, int y, int d, bool makeUndo) {
    Pos chPos = board.getObjChunkCoords(x, y);
    for (int cy = chPos.y - 1; cy <= chPos.y+1; ++cy)
    for (int cx = chPos.x - 1; cx <= chPos.x+1; ++cx) {
        std::map<int64_t, ObjChunk>::iterator wchIt = board.objsMap.find(Pos(cx, cy).hash());
        if (wchIt != board.objsMap.end()) {
            for (int i = 0; i < wchIt->second.objs.size(); i++) {
                const ObjType& objt = getObjType(wchIt->second.objs[i].typeID);
                int ci = 0;
                for (const WireConectionType& c : objt.c) {
                    if (!c.hide) {
                        WireConectionType cType = c;
                        cType.rotate(objt, wchIt->second.objs[i].rotate);
                        if (cType.x + wchIt->second.objs[i].x == x && cType.y + wchIt->second.objs[i].y == y && cType.d == d) {
                            wchIt->second.objs[i].wireNot[ci] = !wchIt->second.objs[i].wireNot[ci];
                            if (makeUndo) {
                                std::vector<Action> actionGrup;
                                actionGrup.emplace_back(x, y, d);
                                pushActionGrup(actionGrup);
                            }
                            return true;
                        }
                        ++ci;
                    }
                }
            }
        }
    }
    return false;
}

bool clickEvent(int boardID, int x, int y)
{
    Obj* obj = board.getObjAt(x, y);
    if (obj != nullptr) {
        if (obj->typeID == 4) { // Switch input
            if (obj->memory.size() == 0) obj->memory.push_back(1);
            else obj->memory[0] = 1-obj->memory[0];
            drawObj(*obj);
            return true;
        } else if (obj->typeID == 0) { // Button input
            if (obj->memory.size() == 0) obj->memory.push_back(3);
            else obj->memory[0] = 3-obj->memory[0];
            drawObj(*obj);
            return true;
        }
    }
    return false;
}

void addObj(int boardID, int x, int y, int typeID, int rotate, bool makeUndo)
{
    if (canAddObj(boardID, x, y, typeID, rotate)) {
        board.objsMap[board.getObjChunkCoords(x, y).hash()].objs.emplace_back(x, y, typeID, rotate);
        if (makeUndo) {
            std::vector<Action> actionGrup;
            actionGrup.emplace_back(true, x, y, typeID, rotate);
            pushActionGrup(actionGrup);
        }
    }
}

void drawObj(const Obj& obj) {
    if (obj.memory.size() == 0) {
        EM_ASM({ drawObj($0, $1, $2, $3, canvas.ctxo, false, undefined, $4); },
            obj.x, obj.y, obj.rotate, obj.typeID, & obj.wireNot[0]);
    }
    else {
        EM_ASM({ drawObj($0, $1, $2, $3, canvas.ctxo, false, $4, $5); },
            obj.x, obj.y, obj.rotate, obj.typeID, & obj.memory[0], & obj.wireNot[0]);
    }
}

void drawObjs(int boardID, double scale, double rectX, double rectY, double rectW, double rectH)
{
    Pos chA = board.getObjChunkCoords(rectX, rectY);
    Pos chB = board.getObjChunkCoords(rectX + rectW, rectY + rectH);

    for (int chx = chA.x-1; chx <= chB.x; chx++)
    for (int chy = chA.y-1; chy <= chB.y; chy++)
    {
        std::map<int64_t, ObjChunk>::iterator wchIt = board.objsMap.find(Pos(chx, chy).hash());
        if (wchIt != board.objsMap.end()) {
            for (int i = 0; i < wchIt->second.objs.size(); i++)
                drawObj(wchIt->second.objs[i]);
        }
    }
}
void propagateGrup(int x, int y, int d, bool first) {
    auto wireActive = board.getWire(x, y, d);
    int& wireGrup = board.getWireGrup(x, y, d);
    if (wireActive && wireGrup == -1)
    {
        wireGrup = grups.size();

        Obj* obj = board.getObjAt(x, y);
        if (obj == nullptr || (getObjType(obj->typeID).w == 0 && getObjType(obj->typeID).h == 0)) {
            int count = 0;
            if (board.getWire(x, y, 1 - d)) count++;
            if (board.getWire(x - d, y - 1 + d, 1 - d)) count++;
            if (board.getWire(x - 1 + d, y - d, d)) count++;

            if (count == 1 || board.getWireNode(x, y)) {
                propagateGrup(x, y, 1 - d, false);
                propagateGrup(x - d, y - 1 + d, 1 - d, false);
            }
            propagateGrup(x - 1 + d, y - d, d, false);
        }
        obj = board.getObjAt(x + 1 - d, y + d);
        if (obj == nullptr || (getObjType(obj->typeID).w == 0 && getObjType(obj->typeID).h == 0)) {
            int count = 0;
            if (board.getWire(x + 1 - d, y + d, 1 - d)) count++;
            if (board.getWire(x + 1 - d * 2, y - 1 + d * 2, 1 - d)) count++;
            if (board.getWire(x + 1 - d, y + d, d)) count++;

            if (count == 1 || board.getWireNode(x + 1 - d, y + d)) {
                propagateGrup(x + 1 - d, y + d, 1 - d, false);
                propagateGrup(x + 1 - d * 2, y - 1 + d * 2, 1 - d, false);
            }
            propagateGrup(x + 1 - d, y + d, d, false);
        }
        if (first) grups.push_back(false);
    }
}

void initRun(int boardID)
{
    grups.clear();

    std::vector<std::map<int64_t, WireChunk>::iterator> blanckChunks;

    for (std::map<int64_t, WireChunk>::iterator it = board.wiresMap.begin(); it != board.wiresMap.end(); ++it) {
        int wiresCount = 0;
        for (int i = 0; i < WireChunkSize * WireChunkSize * 2; i++) {
            it->second.wiresGrup[i] = -1;
            if (it->second.wiresActive[i]) ++wiresCount;
        }
        if (wiresCount == 0)
            blanckChunks.push_back(it);
    }

    for (auto bch : blanckChunks) {
        board.wiresMap.erase(bch);
    }

    for (const auto& [chunkHash, wireChunk] : board.wiresMap) {
        const Pos* chPos = (const Pos*)(const void*)&chunkHash;
        for (int x = 0; x < WireChunkSize; x++) {
            for (int y = 0; y < WireChunkSize; y++) {
                for (int d = 0; d < 2; d++) {
                    propagateGrup(x + chPos->x * WireChunkSize, y + chPos->y * WireChunkSize, d, true);
                }
            }
        }
    }
}

void runStep(int boardID, bool autoRefresh)
{
    std::vector<bool> grupsSet(grups.size());

    for (std::map<int64_t, ObjChunk>::iterator it = board.objsMap.begin(); it != board.objsMap.end(); ++it) {
        for (int i = 0; i < it->second.objs.size(); ++i) {
            logic(it->second.objs[i], board, grups, grupsSet, autoRefresh);
        }
    }

    bool wireChanges = false;
    for (int i = 0; i < grups.size(); i++) {
        if (grups[i] != grupsSet[i]) {
            wireChanges = true;
            grups[i] = grupsSet[i];
        }
    }

    if (autoRefresh && (board.needRefresh || wireChanges)) {
        EM_ASM(refreshCanvasWires(););
        board.needRefresh = false;
    }
    else if (wireChanges) {
        board.needRefresh = true;
    }
}

std::vector<sNode>  selectedNodes;
std::vector<sWire> selectedWires;
std::vector<Obj> selectedObjs;
int selectedX = 0, selectedY = 0, selectedW = 0, selectedH = 0, selectedR = 0;

bool selectRect(int rectX, int rectY, int rectW, int rectH)
{
    selectedX = rectX;
    selectedY = rectY;
    selectedW = rectW;
    selectedH = rectH;

    if (selectedObjs.size() != 0 || selectedWires.size() != 0 || selectedNodes.size() != 0) {
        undoAction();
    }

    std::vector<Action> actionGrup;

    Pos chA = board.getObjChunkCoords(rectX, rectY);
    Pos chB = board.getObjChunkCoords(rectX + rectW, rectY + rectH);

    for (int chx = chA.x - 1; chx <= chB.x; chx++)
    for (int chy = chA.y - 1; chy <= chB.y; chy++)
    {
        std::map<int64_t, ObjChunk>::iterator wchIt = board.objsMap.find(Pos(chx, chy).hash());
        if (wchIt != board.objsMap.end()) {
            for (int i = 0; i < wchIt->second.objs.size(); i++) {
                if (checkCollide(rectX, rectY, rectW, rectH, wchIt->second.objs[i].x, wchIt->second.objs[i].y, wchIt->second.objs[i].getWidth(), wchIt->second.objs[i].getHeight())) {
                    wchIt->second.objs[i].memory.clear();
                    selectedObjs.push_back(wchIt->second.objs[i]);
                    actionGrup.emplace_back(false, wchIt->second.objs[i].x, wchIt->second.objs[i].y, wchIt->second.objs[i].typeID, wchIt->second.objs[i].rotate, wchIt->second.objs[i].wireNot);
                    wchIt->second.objs.erase(wchIt->second.objs.begin() + i);
                    --i;
                }
            }
        }
    }

    chA = board.getWireChunkCoords(rectX, rectY);
    chB = board.getWireChunkCoords(rectX + rectW + 1, rectY + rectH + 1);

    for (int chx = chA.x; chx <= chB.x; chx++)
    for (int chy = chA.y; chy <= chB.y; chy++)
    {
        for (int x = MAX(chx * WireChunkSize, rectX); x < MIN((chx + 1) * WireChunkSize, rectX + rectW + 1); x++)
        for (int y = MAX(chy * WireChunkSize, rectY); y < MIN((chy + 1) * WireChunkSize, rectY + rectH + 1); y++)
        {
            if (board.getWireNode(x, y)) {
                board.getWireNode(x, y) = false;
                actionGrup.emplace_back(x, y);
                selectedNodes.emplace_back(x, y);
            }
        }
    }


    for (int chx = chA.x; chx <= chB.x; chx++)
    for (int chy = chA.y; chy <= chB.y; chy++)
    {
        int count = 0;
        for (int x = MAX(chx * WireChunkSize, rectX); x < MIN((chx+1) * WireChunkSize, rectX + rectW + 1); x++)
        for (int y = MAX(chy * WireChunkSize, rectY); y < MIN((chy+1) * WireChunkSize, rectY + rectH + 1); y++)
        {
            ++count;
            if (board.getWire(x, y, 0) && x < rectX + rectW) {
                board.getWire(x, y, 0) = false;
                actionGrup.emplace_back(false, x, y, 0);
                selectedWires.emplace_back(x, y, 0);
            }
            if (board.getWire(x, y, 1) && y < rectY + rectH) {
                board.getWire(x, y, 1) = false;
                actionGrup.emplace_back(false, x, y, 1);
                selectedWires.emplace_back(x, y, 1);
            }
        }
    }

    if (selectedObjs.size() != 0 || selectedWires.size() != 0 || selectedNodes.size() != 0) {
        actionGrup.emplace_back(Action::Type::Select);
        pushActionGrup(actionGrup);
        return true;
    }

    return false;
}

void deleteSelected()
{
    selectedWires.clear();
    selectedNodes.clear();
    selectedObjs.clear();
}

void moveSelected(int dx, int dy)
{
    selectedX += dx;
    selectedY += dy;

    for (int i = 0; i < selectedObjs.size(); i++) {
        selectedObjs[i].x += dx;
        selectedObjs[i].y += dy;
    }

    for (int i = 0; i < selectedWires.size(); i++) {
        selectedWires[i].x += dx;
        selectedWires[i].y += dy;
    }

    for (int i = 0; i < selectedNodes.size(); i++) {
        selectedNodes[i].x += dx;
        selectedNodes[i].y += dy;
    }
}

void rotateSelected(int r)
{
    //int r = mod(rotate - selectedR, 4);
    selectedR += r;
    int w, h;
    for (int i = 0; i < selectedObjs.size(); i++) {
        w = selectedW;
        h = selectedH;
        for (int n = 0; n < r; n++) {
            int objx = selectedObjs[i].x;
            selectedObjs[i].x = selectedX + (selectedObjs[i].y - selectedY);
            selectedObjs[i].y = selectedY + w - (objx - selectedX + selectedObjs[i].getWidth());
            selectedObjs[i].rotate = (1 + selectedObjs[i].rotate) % 4;
            int t = w;
            w = h;
            h = t;
        }
    }

    for (int i = 0; i < selectedWires.size(); i++) {
        w = selectedW;
        h = selectedH;
        for (int n = 0; n < r; n++) {
            int wirex = selectedWires[i].x;
            selectedWires[i].d = 1 - selectedWires[i].d;
            selectedWires[i].x = selectedX + (selectedWires[i].y - selectedY);
            selectedWires[i].y = selectedY + w - (wirex - selectedX + selectedWires[i].d);
            int t = w;
            w = h;
            h = t;
        }
    }

    for (int i = 0; i < selectedNodes.size(); i++) {
        w = selectedW;
        h = selectedH;
        for (int n = 0; n < r; n++) {
            int nodex = selectedNodes[i].x;
            selectedNodes[i].x = selectedX + (selectedNodes[i].y - selectedY);
            selectedNodes[i].y = selectedY + w - (nodex - selectedX);
            int t = w;
            w = h;
            h = t;
        }
    }

    selectedW = w;
    selectedH = h;
}

bool canUnSelect()
{
    for (const auto& obj : selectedObjs) {
        if (!canAddObj(0, obj.x, obj.y, obj.typeID, obj.rotate))
            return false;
    }
    
    for (const auto& w : selectedWires)
        board.getWire(w.x, w.y, w.d).flip();
    bool res = canAddAllSegWire(board.getObjChunkCoords(selectedX, selectedY), board.getObjChunkCoords(selectedX + selectedW, selectedY + selectedH));
    for (const auto& w : selectedWires)
        board.getWire(w.x, w.y, w.d).flip();

    return res;
}

void unSelect()
{
    if (selectedObjs.size() != 0 || selectedWires.size() != 0 || selectedNodes.size() != 0) {
        if (!canUnSelect()) {
            undoAction();
        }
        else {

            for (const auto& obj : selectedObjs) {
                board.objsMap[board.getObjChunkCoords(obj.x, obj.y).hash()].objs.push_back(obj);
                actionsRegister[actionsRegister.size() - 1].emplace_back(true, obj.x, obj.y, obj.typeID, obj.rotate, obj.wireNot);
            }

            for (const auto& w : selectedWires) {
                setSegWire(actionsRegister[actionsRegister.size() - 1], true, w.x, w.y, w.d);
            }

            for (const auto& node : selectedNodes) {
                auto wireNode = board.getWireNode(node.x, node.y);
                swapNode(0, node.x, node.y, false);
                if (wireNode != board.getWireNode(node.x, node.y))
                    actionsRegister[actionsRegister.size() - 1].emplace_back(node.x, node.y);
            }

            for (const auto& w : selectedWires) {
                changeNode(w.x, w.y);
            }

            deleteSelected();
        }
    }
}

EM_JS(void, drawSelectChangeColor, (bool canPlace), {
    if (canPlace) {
        canvas.ctxt.fillStyle = "#6AE"; canvas.ctxt.strokeStyle = "#6AE";
    }
    else {
        canvas.ctxt.fillStyle = "#E55"; canvas.ctxt.strokeStyle = "#E55";
    }
});

void drawSelect(double scale, double boardX, double boardY)
{
    bool canPlace = canUnSelect();

    drawSelectChangeColor(canPlace);
    EM_ASM({canvas.ctxt.lineWidth = 1;canvas.ctxt.beginPath();});
    
    for (const auto& w : selectedWires) {
        EM_ASM({
            if ($3 == 0) {
                canvas.ctxt.moveTo($0, $1 + 0.5);
                canvas.ctxt.lineTo($0 + $2, $1 + 0.5);
            }
            else {
                canvas.ctxt.moveTo($0 + 0.5, $1);
                canvas.ctxt.lineTo($0 + 0.5, $1 + $2);
            }
            }, w.x* scale - boardX, w.y * scale - boardY, scale, w.d);
    }
    EM_ASM(canvas.ctxt.stroke());
    
    for (const auto& node : selectedNodes) {
        EM_ASM({
            canvas.ctxt.fillRect($0, $1, 5, 5);
            }, int(node.x * scale - boardX - 2), int(node.y * scale - boardY - 2));
    }
    
    for (const auto& obj : selectedObjs) {
        EM_ASM({ drawObj($0, $1, $2, $3, canvas.ctxt, $5, undefined, $4, true); },
            obj.x, obj.y, obj.rotate, obj.typeID, & obj.wireNot[0], !canPlace);
    }
}

CopyPasteData clipBoard;

void copySelected() {
    if (selectedObjs.size() != 0 || selectedWires.size() != 0 || selectedNodes.size() != 0) {
        clipBoard.wires = selectedWires;
        clipBoard.nodes = selectedNodes;
        clipBoard.objs = selectedObjs;
        clipBoard.x = selectedX;
        clipBoard.y = selectedY;
        clipBoard.w = selectedW;
        clipBoard.h = selectedH;
    }
}

bool pasteSelected() {
    if (clipBoard.wires.size() != 0 || clipBoard.nodes.size() != 0 || clipBoard.objs.size() != 0) {
        unSelect();
        std::vector<Action> actionGrup;
        actionGrup.emplace_back(Action::Type::Select);
        pushActionGrup(actionGrup);

        selectedWires = clipBoard.wires;
        selectedNodes = clipBoard.nodes;
        selectedObjs = clipBoard.objs;
        selectedX = clipBoard.x;
        selectedY = clipBoard.y;
        selectedW = clipBoard.w;
        selectedH = clipBoard.h;
        return true;
    }
    return false;
}

void duplicateSelected() {
    if (selectedObjs.size() != 0 || selectedWires.size() != 0 || selectedNodes.size() != 0) {
        copySelected();
        pasteSelected();
    }
}

void internalUndoAction(std::vector<std::vector<Action>>& actionsRegister, bool addReverse) {
    if (actionsRegister.size() > 0) {
        std::vector<Action>& actionGrup = actionsRegister.back();
        std::vector<sNode> nodes;

        for (int i = 0; i < actionGrup.size(); i++) {
            if (addReverse) i = actionGrup.size() - i - 1;
            if (actionGrup[i].type == Action::Type::SetSegWire) {
                nodes.emplace_back(actionGrup[i].x, actionGrup[i].y);
                if (actionGrup[i].d == 0) nodes.emplace_back(actionGrup[i].x+1, actionGrup[i].y);
                else nodes.emplace_back(actionGrup[i].x, actionGrup[i].y+1);
                board.getWire(actionGrup[i].x, actionGrup[i].y, actionGrup[i].d) = (actionGrup[i].set != addReverse);
            }
            else if (actionGrup[i].type == Action::Type::SwapNode) {
                nodes.emplace_back(actionGrup[i].x, actionGrup[i].y);
                swapNode(0, actionGrup[i].x, actionGrup[i].y, false);
            }
            else if (actionGrup[i].type == Action::Type::SetObj) {
                if (actionGrup[i].set == addReverse) {
                    removeObj(0, actionGrup[i].x, actionGrup[i].y, false);
                }
                else {
                    if (canAddObj(0, actionGrup[i].x, actionGrup[i].y, actionGrup[i].typeID, actionGrup[i].rotate)) {
                        Obj obj(actionGrup[i].x, actionGrup[i].y, actionGrup[i].typeID, actionGrup[i].rotate);
                        obj.wireNot = actionGrup[i].wireNot;
                        board.objsMap[board.getObjChunkCoords(actionGrup[i].x, actionGrup[i].y).hash()].objs.push_back(obj);
                    }
                }
            }
            else if (actionGrup[i].type == Action::Type::SwapObjNot) {
                swapObjNot(0, actionGrup[i].x, actionGrup[i].y, actionGrup[i].d, false);
            }
            else if (actionGrup[i].type == Action::Type::Select) {
                deleteSelected();
            }
            if (addReverse) i = actionGrup.size() - i - 1;
        }

        for (auto node : nodes) {
            changeNode(node.x, node.y);
        }

        if (addReverse) undoActionsRegister.push_back(actionGrup);
        actionsRegister.pop_back();
    }
}

void undoAction() {
    internalUndoAction(actionsRegister, true);
}

void reverseUndo() {
    if (undoActionsRegister.size() > 0) {
        actionsRegister.push_back(undoActionsRegister.back());
        internalUndoAction(undoActionsRegister, false);
    }
}


/*
void myCopy(const char* srcS, const char* srcB, char* dts) {
    while (srcS != srcB) {
        *dts = *srcS;
        ++dts; ++srcS;
    }
}*/


void LoadWiresMap(void* datav) {
    char* data = (char*)datav;
    board.objsMap.clear();
    board.wiresMap.clear();

    data += sizeof(int32_t);
    int wiresMemorySize = *((int32_t*)data);
    data += sizeof(int32_t);
    int objMemorySize = *((int32_t*)data);
    data += sizeof(int32_t);

    for (int i = 0; i < wiresMemorySize; i += sizeof(std::pair<int64_t, StoreWireChunk>)) {
        std::pair<int64_t, StoreWireChunk>* pair = (std::pair<int64_t, StoreWireChunk>*)&data[i];
        board.wiresMap.insert(*pair);
    }
    data += wiresMemorySize;
    char* objMemoryEnd = data + objMemorySize;

    while (data < objMemoryEnd) {
        std::pair<int64_t, ObjChunk> pair;

        pair.first = *(int64_t*)data;
        data += sizeof(int64_t);

        pair.second = ObjChunk();
        int objsLen = *(int*)data;
        data += sizeof(int);
        for (int i = 0; i < objsLen; i++) {
            pair.second.objs.emplace_back(((int*)data)[0], ((int*)data)[1], ((int*)data)[2], ((int*)data)[3]);
            data += sizeof(int) * 4;
            for (int wni = 0; wni < pair.second.objs[i].wireNot.size(); wni++) {
                pair.second.objs[i].wireNot[wni] = (data[wni / 8] >> (7 - (wni & 0x7)) & 0x1);
            }
            data += (pair.second.objs[i].wireNot.size() - 1) / 8 + 1;
        }
        board.objsMap.insert(pair);
    }

}

void* SaveWiresMap() {
    int32_t wiresMemorySize = board.wiresMap.size() * sizeof(std::pair<int64_t, StoreWireChunk>);
    int32_t objMemorySize = board.objsMap.size() * (sizeof(int) + sizeof(int64_t));
    for (const auto& [chPos, oCh] : board.objsMap) {
        objMemorySize += oCh.objs.size() * 4 * sizeof(int);
        for (int oi = 0; oi < oCh.objs.size(); oi++) {
            objMemorySize += (oCh.objs[oi].wireNot.size() - 1) / 8 + 1;
        }
    }
    const int32_t dataSize = wiresMemorySize + objMemorySize + sizeof(int32_t) * 3;

    char* data = new char[dataSize];
    void* datav = (void*)data;

    *((int32_t*)data) = dataSize;
    data += sizeof(int32_t);

    *((int32_t*)data) = wiresMemorySize;
    data += sizeof(int32_t);
    *((int32_t*)data) = objMemorySize;
    data += sizeof(int32_t);

    for (const auto& [chPos, wCh] : board.wiresMap) {
        *((std::pair<int64_t, StoreWireChunk>*)data) = std::pair<int64_t, StoreWireChunk>(chPos, wCh);
        data += sizeof(std::pair<int64_t, StoreWireChunk>);
    }

    for (const auto& [chPos, oCh] : board.objsMap) {
        *((int64_t*)data) = chPos;
        data += sizeof(int64_t);
        *((int*)data) = oCh.objs.size();
        data += sizeof(int);
        for (int oi = 0; oi < oCh.objs.size(); ++oi) {
            ((int*)data)[0] = oCh.objs[oi].x;
            ((int*)data)[1] = oCh.objs[oi].y;
            ((int*)data)[2] = oCh.objs[oi].typeID;
            ((int*)data)[3] = oCh.objs[oi].rotate;
            data += sizeof(int) * 4;
            for (int wni = 0; wni < oCh.objs[oi].wireNot.size(); wni++) {
                data[wni/8] = data[wni / 8] | (oCh.objs[oi].wireNot[wni] & 0x1) << (7 - (wni & 0x7));
            }
            data += (oCh.objs[oi].wireNot.size() - 1) / 8 + 1;
        }
    }
    return datav;
}