'''
reads the results and generates the
switching probability matrices as well
as the actual vectors
'''
import numpy as np
N = 3000  # numper of consetuitive clock cycles
name = "sensors_to_memory"
strings = ['HD', '0D', '1D', '2D', '3D', '4D', '5D',
           '__', '0_', '1_', '2_', '3_', '4_', '5_']
file_names = ['link_134.txt', 'link_240.txt', 'link_255.txt']
true_values = np.zeros((len(file_names), N+1))
mux_matrices = np.zeros((len(file_names), len(strings), len(strings)))
''' ONLY WORKS AFTER ALL ERRORS ARE FIXED
for i in range(len(file_names)):
    with open(file_names[i]) as f:
        prev = f.read(2)
        temp =[strings.index(prev)]
        while True:
            curr = f.read(2)
            if not curr:
                break
            else:
                id1, id2 = [strings.index(curr), strings.index(prev)]
                temp.append(id1)
                matrix[id1, id2] += 1
                prev = curr
'''

for i in range(len(file_names)):
    matrix = np.zeros((len(strings), len(strings)))
    with open(file_names[i]) as f:
        prev = f.read(2)
        temp = [strings.index(prev)]
        for k in range(3000):
            curr = f.read(2)
            id1, id2 = [strings.index(curr), strings.index(prev)]
            temp.append(id1)
            matrix[id1, id2] += 1
            prev = curr
    true_values[i, :] = temp
    mux_matrices[i, :, :] = matrix/N
    np.savez('%s.npz' % name, mux_matrices=mux_matrices,
             true_values=true_values, link_events=strings,
             links=file_names)
    # to load np.load('%s.npz' % name)
