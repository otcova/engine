img = input("image: ").replace('"', '')
boardData = input("board: ").replace('"', '')

result = "myboard.png"



from PIL import Image

fh = open(boardData, "rb")
ba = bytearray(fh.read())
fh.close()


im = Image.open(img)
pixels = im.load()

def set_bit(v, index, x):
  """Set the index:th bit of v to 1 if x is truthy, else to 0, and return the new value."""
  mask = 1 << index   # Compute mask, an integer with just bit 'index' set.
  v &= ~mask          # Clear the bit indicated by the mask (if x is False)
  if x:
    v |= mask         # If x was True, set the bit indicated by the mask.
  return v

def get_bit(v, index):
  return (v & (1 << index)) != 0

ba.append(0)
ba.append(0)
ba.append(0)
bai = 0
baii = 0

for y in range(0, im.height):
    for x in range(0, im.width):
        if bai+3 < len(ba):
            pixels[x, y] = (set_bit(pixels[x, y][0], 0, get_bit(ba[bai], baii)),
                            set_bit(pixels[x, y][1], 0, get_bit(ba[bai], baii+1)),
                            pixels[x, y][2])
            if (baii >= 6):
              bai += 1
              baii = 0
            else:
              baii += 2

im.save(result)

imv = Image.open(result)
bai = 0
baii = 0

for y in range(0, imv.height):
    for x in range(0, imv.width):
        if bai+3 < len(ba):
            if (get_bit(pixels[x, y][0], 0) != get_bit(ba[bai], baii)):
                print("R Error " + str(x) + ", " + str(y))
            if (get_bit(pixels[x, y][1], 0) != get_bit(ba[bai], baii+1)):
                print("G Error " + str(x) + ", " + str(y))
            if (baii >= 6):
              bai += 1
              baii = 0
            else:
              baii += 2

imv.close()
