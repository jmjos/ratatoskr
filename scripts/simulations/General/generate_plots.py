import numpy as np
import pickle
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import sys
import os
###############################################################################


def plot_latencies(results):
    """
    Read the raw results from a dictionary of objects, then plot the latencies.

    Parameters:
        - results: a dictionary of raw data from the pickle file.

    Return:
        - None.
    """
    latenciesFlit = results['latenciesFlit']
    latenciesNetwork = results['latenciesNetwork']
    latenciesPacket = results['latenciesPacket']
    injectionRates = results['injectionRates']

    meanLatenciesFlit = np.mean(latenciesFlit, axis=1)
    meanLatenciesPacket = np.mean(latenciesPacket, axis=1)
    meanLatenciesNetwork = np.mean(latenciesNetwork, axis=1)
    stdLatenciesFlit = np.std(latenciesFlit, axis=1)
    stdLatenciesPacket = np.std(latenciesPacket, axis=1)
    stdLatenciesNetwork = np.std(latenciesNetwork, axis=1)

    fig = plt.figure()
    plt.ylabel('Latencies in ns', fontsize=11)
    plt.xlabel('Injection Rate', fontsize=11)
    plt.xlim([0, 1])
    linestyle = {'linestyle': '--', 'linewidth': 1, 'markeredgewidth': 1,
                 'elinewidth': 1, 'capsize': 10}
    plt.errorbar(injectionRates, meanLatenciesFlit, yerr=stdLatenciesFlit,
                 color='r', **linestyle, marker='*')
    plt.errorbar(injectionRates, meanLatenciesNetwork,
                 yerr=stdLatenciesNetwork, color='b', **linestyle,
                 marker='s')
    plt.errorbar(injectionRates, meanLatenciesPacket, yerr=stdLatenciesPacket,
                 color='g', **linestyle, marker='^')

    plt.legend(['Flit', 'Network', 'Packet'])
    fig.suptitle('Latencies', fontsize=16)
    plt.show()
###############################################################################


def plot_VCUsage_stats(inj_dfs, inj_rates):
    """
    Plot the VC usage statistics.

    Parameteres:
        - inj_dfs: the data frames of an injection rate.
        - inj_rates: the number of injection rates.

    Return:
        - None.
    """
    for inj_df, inj_rate in zip(inj_dfs, inj_rates):
        for layer_id, df in enumerate(inj_df):
            plt.figure()  # plot a figure for each injection rate and layer
            plt.title('Layer ' + str(layer_id) +
                      ', Injection Rate = ' + str(inj_rate))
            plt.ylabel('Count', fontsize=11)
            plt.xlabel('VC Usage', fontsize=11)
            for col in df.columns.levels[0].values:
                plt.errorbar(df.index.values, df[col, 'mean'].values,
                             yerr=df[col, 'std'].values)
            plt.legend(df.columns.levels[0].values)
            plt.show()
###############################################################################


def plot_BuffUsage_stats(inj_dicts, inj_rates):
    """
    Plot the buffer usage statistics.

    Parameters:
        - inj_dicts: the data dictionaries of an injection rate.
        - inj_rates: the number of injection rates.

    Return:
        - None.
    """
    for inj_dict, inj_rate in zip(inj_dicts, inj_rates):
        for layer_name in inj_dict:
            layer_dict = inj_dict[layer_name]
            fig = plt.figure()
            for it, d in enumerate(layer_dict):
                df = layer_dict[d]
                if not df.empty:
                    ax = fig.add_subplot(3, 2, it+1, projection='3d')
                    lx = df.shape[0]
                    ly = df.shape[1]
                    xpos = np.arange(0, lx, 1)
                    ypos = np.arange(0, ly, 1)
                    xpos, ypos = np.meshgrid(xpos, ypos, indexing='ij')

                    xpos = xpos.flatten()
                    ypos = ypos.flatten()
                    zpos = np.zeros(lx*ly)

                    dx = 1 * np.ones_like(zpos)
                    dy = dx.copy()
                    dz = df.values.flatten()

                    ax.bar3d(xpos, ypos, zpos, dx, dy, dz, color='b')

                    ax.set_yticks(ypos)
                    ax.set_xlabel('Buffer Size')
                    ax.set_ylabel('VC Index')
                    ax.set_zlabel('Count')
                    ax.set_title('Direction:'+str(d))

            fig.suptitle('Layer: '+str(layer_name)+', Injection Rate = '
                         +str(inj_rate), fontsize=16)
            plt.show()
###############################################################################


def read_raw_results(results_file):
    """
    Read the raw results from the pickle file.

    Parameters:
        - results_file: the path to the pickle file.

    Return:
        - results: a dictionary of objects.
    """
    results = None
    with open(results_file, 'rb') as f:
        results = pickle.load(f)
    return results
###############################################################################


def main():
    """ Main Point of Execution."""
    try:
        folder = sys.argv[1]
    except Exception:
        raise
    else:
        results = read_raw_results(os.path.join(folder, 'rawResults.pkl'))

        plot_latencies(results)

        plot_VCUsage_stats(results['VCUsage'], results['injectionRates'])

        plot_BuffUsage_stats(results['BuffUsage'], results['injectionRates'])
###############################################################################


if __name__ == '__main__':
    main()

