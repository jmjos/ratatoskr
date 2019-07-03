'''
optimizations fucntions
'''
from itertools import permutations


def ext_placement_search(func, B, find_max=False):
    '''
    trys out all possible discrete placements
    and finds the one that minimzes(or maximizes,
    if "max"=True the return value of "func()".
    '''
    perm = set(range(B))
    cost = func(perm)
    for i in permutations(range(B)):
        perm_neu = i
        cost_neu = func(i)

        if (find_max is True and cost_neu > cost) or (find_max is False
                                                      and cost_neu < cost):
            perm, cost = perm_neu, cost_neu
    return {'placement': perm, 'cost': cost}
