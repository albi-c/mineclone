import png, sys

INPUT1 = sys.argv[1]
INPUT2 = sys.argv[2]
OUTPUT = sys.argv[3]

r1 = png.Reader(filename=INPUT1)
im1 = r1.read()
d1 = list(map(list, im1[2]))

r2 = png.Reader(filename=INPUT2)
im2 = r2.read()
d2 = list(map(list, im2[2]))

d = []
for i, row1 in enumerate(d1):
    row2 = d2[i]
    j = 0
    d.append([])
    while j < len(row1):
        m1 = (255-row2[j+3])/255
        m2 = row2[j+3]/255
        d[-1] += list(map(lambda x: min(int(x), 255), [row1[j] * m1 + row2[j] * m2, row1[j+1] * m1 + row2[j+1] * m2, row1[j+2] * m1 + row2[j+2] * m2, max(row1[j+3], row2[j+3])]))
        j += 4

w = png.Writer(im1[0], im1[1], alpha=True, greyscale=False)
f = open(OUTPUT, "wb+")
w.write(f, d)
f.close()
