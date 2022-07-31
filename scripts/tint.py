import png, sys

INPUT = sys.argv[1]
OUTPUT = sys.argv[2]
COLOR = (145, 189, 89)

COLOR = list(map(lambda x: x / 255, COLOR))
r = png.Reader(filename=INPUT)
im = r.read()
d = list(map(list, im[2]))
d_ = []
for row in d:
    d_.append([])
    i = 0
    while i < len(row):
        d_[-1] += list(map(int, [row[i] * COLOR[0], row[i+1] * COLOR[1], row[i+2] * COLOR[2], row[i+3]]))
        i += 4
w = png.Writer(im[0], im[1], alpha=True, greyscale=False)
f = open(OUTPUT, "wb+")
w.write(f, d_)
f.close()
