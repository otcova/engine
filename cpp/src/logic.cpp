#ifdef __INTELLISENSE__
#define EM_ASM_INT()
#define EM_ASM()
#endif

#include <emscripten.h>
#include <vector>
#include <iostream>
#include "logic.h"
#include "boardMain.h"

std::vector<ObjType> objTypes;

bool displayStates[10][7] = {
	{1, 0, 1, 1, 1, 1, 1},
	{0, 0, 0, 0, 0, 1, 1},
	{1, 1, 1, 0, 1, 1, 0},
	{1, 1, 1, 0, 0, 1, 1},
	{0, 1, 0, 1, 0, 1, 1},
	{1, 1, 1, 1, 0, 0, 1},
	{1, 1, 1, 1, 1, 0, 1},
	{1, 0, 0, 0, 0, 1, 1},
	{1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 0, 1, 1}};

void initTypes()
{
	objTypes.clear();
	int objTypesLen = EM_ASM_INT({return objTypes.length});
	objTypes.reserve(objTypesLen);

	for (int i = 0; i < objTypesLen; i++)
	{
		ObjType objt;
		objt.w = EM_ASM_INT({return objTypes[$0].w}, i);
		objt.h = EM_ASM_INT({return objTypes[$0].h}, i);
		int connectionsLen = EM_ASM_INT({return objTypes[$0].wires.length}, i);
		objt.c.reserve(connectionsLen);

		for (int ci = 0; ci < connectionsLen; ci++)
		{
			objt.c.push_back({EM_ASM_INT({ // x
								  if (objTypes[$0].wires[$1].dir == "left")
									  return -1;
								  else if (objTypes[$0].wires[$1].dir == "right")
									  return objTypes[$0].w;
								  else if (objTypes[$0].wires[$1].dir == "up")
									  return objTypes[$0].wires[$1].pos;
								  else if (objTypes[$0].wires[$1].dir == "down")
									  return objTypes[$0].wires[$1].pos;
							  },
										 i, ci),
							  EM_ASM_INT({ // y
								  if (objTypes[$0].wires[$1].dir == "left")
									  return objTypes[$0].wires[$1].pos;
								  else if (objTypes[$0].wires[$1].dir == "right")
									  return objTypes[$0].wires[$1].pos;
								  else if (objTypes[$0].wires[$1].dir == "up")
									  return -1;
								  else if (objTypes[$0].wires[$1].dir == "down")
									  return objTypes[$0].h;
							  },
										 i, ci),
							  EM_ASM_INT({ // d
								  if (objTypes[$0].wires[$1].dir == "left")
									  return 0;
								  else if (objTypes[$0].wires[$1].dir == "right")
									  return 0;
								  else if (objTypes[$0].wires[$1].dir == "up")
									  return 1;
								  else if (objTypes[$0].wires[$1].dir == "down")
									  return 1;
							  },
										 i, ci),
							  EM_ASM_INT({ // not
								  if (objTypes[$0].wires[$1].not )
									  return 1;
								  return 0;
							  },
										 i, ci) == 1,
							  EM_ASM_INT({ // hide
								  if (objTypes[$0].wires[$1].hide)
									  return 1;
								  return 0;
							  },
										 i, ci) == 1});
		}

		objTypes.push_back(objt);
	}
}

const ObjType &getObjType(int typeID)
{
	return objTypes[typeID];
}

inline bool get(Board &board, std::vector<bool> &grups, const Obj &obj, const ObjType &objt, int index)
{
	WireConectionType cType = objt.c[index];
	cType.rotate(objt, obj.rotate);
	int g = board.getWireGrup(obj.x + cType.x, obj.y + cType.y, cType.d);
	if (g == -1)
		return obj.wireNot[index];
	return grups[g] != obj.wireNot[index];
}

inline void set(Board &board, std::vector<bool> &grups, const Obj &obj, const ObjType &objt, int index, bool val)
{
	val = val != obj.wireNot[index];
	if (val)
	{
		WireConectionType cType = objt.c[index];
		cType.rotate(objt, obj.rotate);
		int g = board.getWireGrup(obj.x + cType.x, obj.y + cType.y, cType.d);
		if (g != -1)
		{
			grups[g] = val;
		}
	}
}

void logic(Obj &obj, Board &b, std::vector<bool> &grupsGet, std::vector<bool> &grupsSet, bool draw)
{
	const ObjType &objt = getObjType(obj.typeID);
	bool changes = false;
	switch (obj.typeID)
	{
	case 0: //Button
		if (obj.memory.size() == 0)
			obj.memory.push_back(0);
		else if (obj.memory[0] > 0)
		{
			obj.memory[0] -= 1;
			drawObj(obj);
		}
		set(b, grupsSet, obj, objt, 0, obj.memory[0]);
		set(b, grupsSet, obj, objt, 1, obj.memory[0]);
		set(b, grupsSet, obj, objt, 2, obj.memory[0]);
		set(b, grupsSet, obj, objt, 3, obj.memory[0]);
		return;
	case 1: //1 Bit
	{
		if (obj.memory.size() == 0)
			obj.memory.push_back(0);
			
		if (get(b, grupsGet, obj, objt, 4))
		{
			obj.memory[0] = 0;
		}
		else if (get(b, grupsGet, obj, objt, 1))
		{
			if (get(b, grupsGet, obj, objt, 3))
				obj.memory[0] = get(b, grupsGet, obj, objt, 2);
			set(b, grupsSet, obj, objt, 0, obj.memory[0]);
		}
		return;
	}
	case 2: //XOr
		set(b, grupsSet, obj, objt, 2, get(b, grupsGet, obj, objt, 0) == get(b, grupsGet, obj, objt, 1));
		return;
	case 3: //Not
		set(b, grupsSet, obj, objt, 1, get(b, grupsGet, obj, objt, 0));
		return;
	case 4: //Switch
		if (obj.memory.size() == 0)
			obj.memory.push_back(0);
		set(b, grupsSet, obj, objt, 0, obj.memory[0]);
		set(b, grupsSet, obj, objt, 1, obj.memory[0]);
		set(b, grupsSet, obj, objt, 2, obj.memory[0]);
		set(b, grupsSet, obj, objt, 3, obj.memory[0]);
		return;
	case 5: // Led
	{
		if (draw)
		{
			bool update = false;
			bool state = get(b, grupsGet, obj, objt, 0) || get(b, grupsGet, obj, objt, 1) || get(b, grupsGet, obj, objt, 2) || get(b, grupsGet, obj, objt, 3);
			if (obj.memory.size() == 0)
			{
				obj.memory.push_back(state ? 7 : 0);
				update = true;
			}
			else if (obj.memory[0] != state)
			{
				obj.memory[0] = state ? 7 : 0;
				update = true;
			}
			if (update)
				drawObj(obj);
		}
		return;
	}
	case 6: // Display
	{
		if (draw)
		{
			if (obj.memory.size() == 0)
			{
				for (int i = 0; i < 14; i++)
					obj.memory.push_back(false);
			}

			int state = 0;
			if (get(b, grupsGet, obj, objt, 0))
				state += 1;
			if (get(b, grupsGet, obj, objt, 1))
				state += 2;
			if (get(b, grupsGet, obj, objt, 2))
				state += 4;
			if (get(b, grupsGet, obj, objt, 3))
				state += 8;
			if (get(b, grupsGet, obj, objt, 4))
				state += 16;
			if (get(b, grupsGet, obj, objt, 5))
				state += 32;

			for (int i = 0; i < 7; i++)
				obj.memory[i] = displayStates[state % 10][i] ? 7 : 0;
			for (int i = 0; i < 7; i++)
				obj.memory[i + 7] = displayStates[state / 10][i] ? 7 : 0;

			drawObj(obj);
		}
		return;
	}
	case 7: // Color led
	{
		if (draw)
		{
			if (obj.memory.size() == 0)
				obj.memory.push_back(0);

			int result = 0;
			if (get(b, grupsGet, obj, objt, 0))
				result += 1;
			if (get(b, grupsGet, obj, objt, 1))
				result += 2;
			if (get(b, grupsGet, obj, objt, 2))
				result += 4;

			if (result != obj.memory[0])
			{
				obj.memory[0] = result;
				drawObj(obj);
			}
		}
		return;
	}
	case 8: // Clock
		if (obj.memory.size() == 0)
			obj.memory.push_back(0);
		else if (++obj.memory[0] >= 4 * 2)
			obj.memory[0] = 0;
		set(b, grupsSet, obj, objt, 0, obj.memory[0] < 4);
		return;
	case 9: // Half adder
	{
		bool inA = get(b, grupsGet, obj, objt, 0);
		bool inB = get(b, grupsGet, obj, objt, 1);
		set(b, grupsSet, obj, objt, 2, inA != inB);
		set(b, grupsSet, obj, objt, 3, inA && inB);
		return;
	}
	case 10: // Half adder
	{
		bool inA = get(b, grupsGet, obj, objt, 0);
		bool inB = get(b, grupsGet, obj, objt, 1);
		bool inC = get(b, grupsGet, obj, objt, 2);
		set(b, grupsSet, obj, objt, 3, (inA != inB) != inC);
		set(b, grupsSet, obj, objt, 4, ((inA != inB) && inC) || (inA && inB));
		return;
	}
	case 11: // Gated D lantch
	{
		if (obj.memory.size() == 0)
		{
			obj.memory.push_back(0);
			obj.memory.push_back(0);
		}
		int clk = get(b, grupsGet, obj, objt, 1);
		if (!obj.memory[1] && clk)
		{
			obj.memory[0] = get(b, grupsGet, obj, objt, 0);
		}
		obj.memory[1] = clk;
		set(b, grupsSet, obj, objt, 2, obj.memory[0]);
		return;
	}
	case 12: // Gated RS lantch
		if (obj.memory.size() == 0)
			obj.memory.push_back(0);
		if (get(b, grupsGet, obj, objt, 1))
		{
			if (get(b, grupsGet, obj, objt, 0))
				obj.memory[0] = true;
			else if (get(b, grupsGet, obj, objt, 2))
				obj.memory[0] = false;
		}
		set(b, grupsSet, obj, objt, 3, obj.memory[0]);
		return;
	case 13: // RS lantch
		if (obj.memory.size() == 0)
			obj.memory.push_back(0);
		else if (get(b, grupsGet, obj, objt, 0))
			obj.memory[0] = true;
		else if (get(b, grupsGet, obj, objt, 1))
			obj.memory[0] = false;

		set(b, grupsSet, obj, objt, 2, obj.memory[0]);
		return;
	}
	if (obj.typeID >= 14 && obj.typeID <= 28) // And {x}
	{
		bool res = get(b, grupsGet, obj, objt, 0) && get(b, grupsGet, obj, objt, 1);
		for (int i = 2; i < obj.typeID - 12; i++)
			res = res && get(b, grupsGet, obj, objt, i);
		set(b, grupsSet, obj, objt, obj.typeID - 12, res);
	}
	else if (obj.typeID >= 29 && obj.typeID <= 43) // Or {x}
	{
		bool res = get(b, grupsGet, obj, objt, 0) || get(b, grupsGet, obj, objt, 1);
		for (int i = 2; i < obj.typeID - 27; i++)
			res = res || get(b, grupsGet, obj, objt, i);
		set(b, grupsSet, obj, objt, obj.typeID - 27, res);
	}
	else if (obj.typeID >= 44 && obj.typeID <= 58) // XOr {x}
	{
		bool res = get(b, grupsGet, obj, objt, 0) != get(b, grupsGet, obj, objt, 1);
		for (int i = 2; i < obj.typeID - 42; i++)
			res = res != get(b, grupsGet, obj, objt, i);
		set(b, grupsSet, obj, objt, obj.typeID - 42, res);
	}
	else if (obj.typeID >= 59 && obj.typeID <= 63) // Decoder
	{
		unsigned char number = 0;

		int size = obj.typeID - 57;

		if (get(b, grupsGet, obj, objt, 0)) number |= 0b1;
		if (get(b, grupsGet, obj, objt, 1)) number |= 0b10;

		if (size >= 3)
			if (get(b, grupsGet, obj, objt, 2)) number |= 0b100;
		if (size >= 4)
			if (get(b, grupsGet, obj, objt, 3)) number |= 0b1000;
		if (size >= 5)
			if (get(b, grupsGet, obj, objt, 4)) number |= 0b10000;
		if (size >= 6)
			if (get(b, grupsGet, obj, objt, 5)) number |= 0b100000;

		set(b, grupsSet, obj, objt, size + number, true);
	}
	else if (obj.typeID == 64) // Point Display
	{
		if (obj.memory.size() == 0) {
			for (int i = 0; i < 64; i++)
				obj.memory.push_back(0);
		}
		if (get(b, grupsGet, obj, objt, 0)) {
			for (int i = 0; i < 64; i++)
				obj.memory[i] = 0;
			drawObj(obj);
		} else if (get(b, grupsGet, obj, objt, 1)) {
			unsigned char number = 0;
			if (get(b, grupsGet, obj, objt, 2)) number |= 0b1;
			if (get(b, grupsGet, obj, objt, 3)) number |= 0b10;
			if (get(b, grupsGet, obj, objt, 4)) number |= 0b100;
			if (get(b, grupsGet, obj, objt, 5)) number |= 0b1000;
			if (get(b, grupsGet, obj, objt, 6)) number |= 0b10000;
			if (get(b, grupsGet, obj, objt, 7)) number |= 0b100000;
			if (obj.memory[number] == 0) {
				obj.memory[number] = 7;
				drawObj(obj);
			}
		}
	}
	else if (obj.typeID >= 65 && obj.typeID <= 79) // Dor
	{
		int size = obj.typeID - 63;
		if (get(b, grupsGet, obj, objt, 0)) {
			for (int i = 0; i < size; i++) {
				if (get(b, grupsGet, obj, objt, i+1)) 
					set(b, grupsSet, obj, objt, size + i + 1, true);
			}
		}
	}
}
