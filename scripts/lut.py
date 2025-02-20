edges = [
    (0, 1), (1, 2), (2, 3), (3, 0),
    (4, 5), (5, 6), (6, 7), (7, 4),
    (0, 4), (1, 5), (2, 6), (3, 7)
]

lookup_table = []
for case in range(256):
    #print('{ ', end='')
    num = 0
    for edge in edges:
        if ((case & (1 << edge[0])) != 0) > ((case & (1 << edge[1])) != 0):
            #print(f'{{{edge[0]}, {edge[1]}}}', end=', ')
            num += 1
        elif ((case & (1 << edge[0])) != 0) < ((case & (1 << edge[1])) != 0):
            #print(f'{{{edge[1]}, {edge[0]}}}', end=', ')
            num += 1
    #print(' },')
    print(f'{num}, ', end='')
