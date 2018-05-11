import numpy as np
import networkx as nx
import matplotlib.pyplot as plt
import xml.etree.ElementTree as ET
############################################################################
num_senders = 16
num_receivers = 2
############################################################################
sender_nodes = list(range(0, num_senders))
receiver_nodes = list(range(num_senders, num_senders + num_receivers))

senders_color = list('g' * num_senders)
receivers_color = list('r' * num_receivers)
color_list = senders_color + receivers_color

graph = nx.DiGraph()

graph.add_nodes_from(sender_nodes)
graph.add_nodes_from(receiver_nodes)

for i in sender_nodes:
    for j in receiver_nodes:
        w = str(i) + '-' + str(j)
        graph.add_edge(i, j, label=w)

############################################################################
# Plot it
pos = nx.circular_layout(graph)
nx.draw_networkx(graph, pos=pos, with_labels=True, node_color=color_list, alpha=0.5)
edge_labels = nx.get_edge_attributes(graph, 'label')
nx.draw_networkx_edge_labels(graph, pos=pos, edge_labels=edge_labels)
plt.show()
