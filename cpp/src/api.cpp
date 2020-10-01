#ifdef __INTELLISENSE__
#define EMSCRIPTEN_KEEPALIVE
#endif

#include <emscripten.h>
#include "boardMain.h"
#include "logic.h"

extern "C"
{
    // Main

    EMSCRIPTEN_KEEPALIVE int 
    _createBoard() {
        initTypes();
        return 1;
    }

    EMSCRIPTEN_KEEPALIVE void
    _initRun(int boardID) {
        initRun(boardID);
    }

    EMSCRIPTEN_KEEPALIVE void
    _runStep(int boardID, bool autoRefresh) {
        runStep(boardID, autoRefresh);
    }
    EMSCRIPTEN_KEEPALIVE void
    _stopRun(int boardID) {
        stopRun(boardID);
    }

    // Wire

    EMSCRIPTEN_KEEPALIVE bool
    _canAddWire(int boardID, int ax, int ay, int bx, int by, int cx, int cy) {
        return canAddWire(boardID, ax, ay, bx, by, cx, cy);
    }

    EMSCRIPTEN_KEEPALIVE void 
    _setWire(int boardID, bool set, int ax, int ay, int bx, int by, int cx, int cy) {
        setWire(boardID, set, ax, ay, bx, by, cx, cy);
    }

    EMSCRIPTEN_KEEPALIVE void
    _swapNode(int boardID, int x, int y) {
        client_swapNode(x, y);
    }

    EMSCRIPTEN_KEEPALIVE void 
    _drawWires(int boardID, double scale, double rectX, double rectY, double rectW, double rectH) {
        drawWires(boardID, scale, rectX, rectY, rectW, rectH);
    }

    //Objs

    EMSCRIPTEN_KEEPALIVE bool
        _clickEvent(int boardID, int x, int y) {
        return clickEvent(boardID, x, y);
    }

    EMSCRIPTEN_KEEPALIVE bool
        _swapObjNot(int boardID, int x, int y, int d) {
        return swapObjNot(boardID, x, y, d);
    }

    EMSCRIPTEN_KEEPALIVE bool
    _canAddObj(int boardID, int x, int y, int typeID, int rotate) {
        return canAddObj(boardID, x, y, typeID, rotate);
    }

    EMSCRIPTEN_KEEPALIVE void
    _addObj(int boardID, int x, int y, int typeID, int rotate) {
        addObj(boardID, x, y, typeID, rotate);
    }
    
    EMSCRIPTEN_KEEPALIVE void
    _removeObj(int boardID, int x, int y) {
        removeObj(boardID, x, y);
    }

    EMSCRIPTEN_KEEPALIVE void
    _drawObjs(int boardID, double scale, double rectX, double rectY, double rectW, double rectH) {
        drawObjs(boardID, scale, rectX, rectY, rectW, rectH);
    }

    // Select

    EMSCRIPTEN_KEEPALIVE bool
    _selectRect(int rectX, int rectY, int rectW, int rectH) {
        return selectRect(rectX, rectY, rectW, rectH);
    }

    EMSCRIPTEN_KEEPALIVE void
    _moveSelected(int dx, int dy) {
        moveSelected(dx, dy);
    }

    EMSCRIPTEN_KEEPALIVE void
    _deleteSelected(){
        deleteSelected();
    }

    EMSCRIPTEN_KEEPALIVE void
    _unSelect() {
        unSelect();
    }

    EMSCRIPTEN_KEEPALIVE void
    _rotateSelected(int r) {
        rotateSelected(r);
    }

    EMSCRIPTEN_KEEPALIVE void
    _drawSelect(double scale, double boardX, double boardY) {
        drawSelect(scale, boardX, boardY);
    }

    EMSCRIPTEN_KEEPALIVE void
    _duplicateSelected() {
        duplicateSelected();
    }

    EMSCRIPTEN_KEEPALIVE void
    _copySelected() {
        copySelected();
    }

    EMSCRIPTEN_KEEPALIVE bool
    _pasteSelected() {
        return pasteSelected();
    }

    // Ctrl + z

    EMSCRIPTEN_KEEPALIVE void
    _undoAction() {
        undoAction();
    }

    EMSCRIPTEN_KEEPALIVE void
    _reverseUndo() {
        reverseUndo();
    }

    //  Save board

    EMSCRIPTEN_KEEPALIVE void
    _loadBoard(void* data) {
        LoadBoard(data);
    }

    EMSCRIPTEN_KEEPALIVE void*
    _saveBoard() {
        return SaveBoard();
    }
}