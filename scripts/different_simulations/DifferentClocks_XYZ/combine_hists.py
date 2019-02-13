import os
import sys
import pandas as pd
import numpy as np
###############################################################################


def combine_VC_hists(directory):
    """
    Combine the VC histograms from csv files.

    Parameters:
        - directory: the path of the directory that contains the files.

    Return:
        - A dataframe object of the combined csv files,
        or None if the directory doesn't exist.
    """
    if os.path.exists(directory):
        layer0 = pd.DataFrame()
        layer1 = pd.DataFrame()
        layer2 = pd.DataFrame()
        for filename in os.listdir(directory):
            router_id = int(filename.split('.')[0])
            if router_id in range(0, 16):
                temp = pd.read_csv(directory + '/' + filename, header=None,
                                   index_col=0).T
                layer0 = layer0.add(temp, fill_value=0)
            if router_id in range(16, 32):
                temp = pd.read_csv(directory + '/' + filename, header=None,
                                   index_col=0).T
                layer1 = layer1.add(temp, fill_value=0)
            if router_id in range(32, 48):
                temp = pd.read_csv(directory + '/' + filename, header=None,
                                   index_col=0).T
                layer2 = layer2.add(temp, fill_value=0)

        data = [layer0, layer1, layer2]
        for df in data:
            df.columns.name = 'Direction'
            df.index.name = 'Number of VCs'

        return data
    else:
        return None
###############################################################################


def read_dataframe(layers, path, layer, dir):
    """
    Read a data frame from csv file then accumulate the data.

    Parameters:
        - layers: a dictionary of dictionaries, and this is the data that
        needs to bee updated.
        - path: the path of the csv file to be read.
        - layer: the key of outmost dictionary layers.
        - dir: the key of innermost dictionary layers[layer]

    Return:
       - The updated data structure layers.
    """
    temp = pd.read_csv(path, index_col=0)
    if not temp.empty:
        layers[layer][dir] = layers[layer][dir].add(temp, fill_value=0)
        return layers
###############################################################################


def init_data_structure():
    """
    Initialize the data structure named 'layers' which is a dictionary of
    dictionaries.

    Parameters:
        - None

    Return:
        - The initilazed data structure
    """
    layer_temp = {'Up': pd.DataFrame(), 'Down': pd.DataFrame(),
                  'North': pd.DataFrame(), 'South': pd.DataFrame(),
                  'East': pd.DataFrame(), 'West': pd.DataFrame()}
    layers = {'Bottom': layer_temp, 'Middle': layer_temp.copy(),
              'Top': layer_temp.copy()}

    del layers['Bottom']['Down']  # bottom layer has no down direction
    del layers['Top']['Up']  # top layer has no up direction

    return layers
###############################################################################


def combine_Buff_hists(directory):
    """
        Combine the Buffer histograms from csv files.

        Parameters:
            - directory: the path of the directory that contains the files.

        Return:
            - A dataframe object of the combined csv files,
            or None if the directory doesn't exist.
    """
    if os.path.exists(directory):
        layers = init_data_structure()

        for filename in os.listdir(directory):
            router_id = int(filename.split('.')[0].split('_')[0])
            direction = filename.split('.')[0].split('_')[1]
            path = directory + '/' + filename
            if router_id in range(0, 16):
                layer = 'Bottom'
                if direction == 'Up':
                    layers = read_dataframe(layers, path, layer, 'Up')
                if direction == 'North':
                    layers = read_dataframe(layers, path, layer, 'North')
                if direction == 'South':
                    layers = read_dataframe(layers, path, layer, 'South')
                if direction == 'East':
                    layers = read_dataframe(layers, path, layer, 'East')
                if direction == 'West':
                    layers = read_dataframe(layers, path, layer, 'West')
            if router_id in range(16, 32):
                layer = 'Middle'
                if direction == 'Up':
                    layers = read_dataframe(layers, path, layer, 'Up')
                if direction == 'Down':
                    layers = read_dataframe(layers, path, layer, 'Down')
                if direction == 'North':
                    layers = read_dataframe(layers, path, layer, 'North')
                if direction == 'South':
                    layers = read_dataframe(layers, path, layer, 'South')
                if direction == 'East':
                    layers = read_dataframe(layers, path, layer, 'East')
                if direction == 'West':
                    layers = read_dataframe(layers, path, layer, 'West')
            if router_id in range(32, 48):
                layer = 'Top'
                if direction == 'Down':
                    layers = read_dataframe(layers, path, layer, 'Down')
                if direction == 'North':
                    layers = read_dataframe(layers, path, layer, 'North')
                if direction == 'South':
                    layers = read_dataframe(layers, path, layer, 'South')
                if direction == 'East':
                    layers = read_dataframe(layers, path, layer, 'East')
                if direction == 'West':
                    layers = read_dataframe(layers, path, layer, 'West')

        # average the buffer usage over the inner routers (#4)
        for l in layers:
            for d in layers[l]:
                layers[l][d] = np.ceil(layers[l][d] / 4)

        return layers
    else:
        return None
###############################################################################


""" Main point of execution """
if __name__ == '__main__':
    try:
        VC_dir = sys.argv[1]
        Buff_dir = sys.argv[2]
    except Exception:
        print('Please enter the directory path.')
    else:
        combine_VC_hists(VC_dir)
        combine_Buff_hists(Buff_dir)
