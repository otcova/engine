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
    _initRun() {
        initRun();
    }
    
    EMSCRIPTEN_KEEPALIVE void
    _set_async(bool async_is_on) {
        set_async(async_is_on);
    }
    
    EMSCRIPTEN_KEEPALIVE void
    _runStep() {
        runStep();
    }
    EMSCRIPTEN_KEEPALIVE void
    _stopRun() {
        stopRun();
    }

    // Wire

    EMSCRIPTEN_KEEPALIVE bool
    _canAddWire(int ax, int ay, int bx, int by, int cx, int cy) {
        return canAddWire(ax, ay, bx, by, cx, cy);
    }

    EMSCRIPTEN_KEEPALIVE void 
    _setWire(bool set, int ax, int ay, int bx, int by, int cx, int cy) {
        setWire(set, ax, ay, bx, by, cx, cy);
    }

    EMSCRIPTEN_KEEPALIVE void
    _swapNode(int x, int y) {
        client_swapNode(x, y);
    }

    EMSCRIPTEN_KEEPALIVE void 
    _drawWires(double scale, double rectX, double rectY, double rectW, double rectH) {
        drawWires(scale, rectX, rectY, rectW, rectH);
    }

    //Objs

    EMSCRIPTEN_KEEPALIVE bool
        _clickEvent(int x, int y) {
        return clickEvent(x, y);
    }

    EMSCRIPTEN_KEEPALIVE bool
        _swapObjNot(int x, int y, int d) {
        return swapObjNot(x, y, d, true);
    }

    EMSCRIPTEN_KEEPALIVE bool
    _canAddObj(int x, int y, int typeID, int rotate) {
        return canAddObj(x, y, typeID, rotate);
    }

    EMSCRIPTEN_KEEPALIVE void
    _addObj(int x, int y, int typeID, int rotate) {
        addObj(x, y, typeID, rotate);
    }
    
    EMSCRIPTEN_KEEPALIVE void
    _removeObj(int x, int y) {
        removeObj(x, y);
    }

    EMSCRIPTEN_KEEPALIVE void
    _drawObjs(double scale, double rectX, double rectY, double rectW, double rectH) {
        drawObjs(scale, rectX, rectY, rectW, rectH);
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