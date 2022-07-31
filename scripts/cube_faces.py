# https://stackoverflow.com/questions/37753117/determine-clockwise-vertexes-that-make-up-a-face-for-a-cube

FACE_EDGES = [
    [(0, 1), (1, 2), (2, 3), (0, 3)],
    [(4, 7), (6, 7), (5, 6), (4, 5)],
    [(0, 4), (4, 5), (1, 5), (0, 1)],
    [(1, 5), (5, 6), (2, 6), (1, 2)],
    [(2, 6), (6, 7), (3, 7), (2, 3)],
    [(0, 4), (0, 3), (3, 7), (4, 7)]
]

# gets vertices of a face, but may be "clockwise" or                                                                                                                 
# "counterclockwise"                                                                                                                                                 
def get_vertices_from_edges(edges):
    verts = [edges[0][0]]
    for i in list(range(3)):
        for a, b in edges:
            if a == verts[-1] and b not in verts:
                verts.append(b)
                break
            if b == verts[-1] and a not in verts:
                verts.append(a)
                break
    return verts

def get_face_vertices(face_edges):
    face_verts = []
    for edges in face_edges:
        face_verts.append(get_vertices_from_edges(edges))
    # orientations of vertices may not be correct yet                                                                                                                

    orient_order = list(range(6)) # the order in which to fix orientations of faces                                                                                        
    if face_verts[0][0] not in face_verts[1]:
        # make sure the first two faces touch each other, so that all                                                                                                
        # subsequent faces will share an edge with a previously                                                                                                      
        # oriented face                                                                                                                                              
        orient_order[1] = 2
        orient_order[2] = 1

    oriented_edges = set()
    for face_index in orient_order:
        verts = face_verts[face_index]
        edges = zip(verts, verts[-1:] + verts[:-1])

        # make it so that if face A and face B share an edge, they                                                                                                   
        # orient the shared edge in opposite ways                                                                                                                    
        needs_reverse = False
        for a, b in edges:
            if (a, b) in oriented_edges:
                needs_reverse = True
                break
        if needs_reverse:
            verts.reverse()

        edges = zip(verts, verts[-1:] + verts[:-1])
        for a, b in edges:
            oriented_edges.add((a, b))

    return face_verts

# print(get_face_vertices(FACE_EDGES))

VERTICES = [
    [-1, 1, 1],
    [1, 1, 1],
    [1, 1, -1],
    [-1, 1, -1],
    [-1, -1, 1],
    [1, -1, 1],
    [1, -1, -1],
    [-1, -1, -1],
]

INDICES = [
    3, 0, 1, 3, 1, 2
]

def vertex(n):
    return ", ".join(map(str, map(lambda x: x / 2, VERTICES[n])))

for face in get_face_vertices(FACE_EDGES):
    for i in INDICES:
        print(vertex(face[i]))
    print()
