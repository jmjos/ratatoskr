#!/bin/python

# Copyright 2018 Jan Moritz Joseph

# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
###############################################################################
import numpy as np
import pickle
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from PyPDF2 import PdfFileMerger
import glob as glob
import os
import itertools
###############################################################################


def plot_latencies(results, comp):
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
    latenciesFlitComp = comp['latenciesFlit']
    latenciesNetworkComp = comp['latenciesNetwork']
    latenciesPacketComp = comp['latenciesPacket']
    injectionRates = results['injectionRates']

    meanLatenciesFlit = np.mean(latenciesFlit, axis=1)
    meanLatenciesPacket = np.mean(latenciesPacket, axis=1)
    meanLatenciesNetwork = np.mean(latenciesNetwork, axis=1)
    stdLatenciesFlit = np.std(latenciesFlit, axis=1)
    stdLatenciesPacket = np.std(latenciesPacket, axis=1)
    stdLatenciesNetwork = np.std(latenciesNetwork, axis=1)

    meanLatenciesFlitComp = np.mean(latenciesFlitComp, axis=1)
    meanLatenciesPacketComp = np.mean(latenciesPacketComp, axis=1)
    meanLatenciesNetworkComp = np.mean(latenciesNetworkComp, axis=1)
    stdLatenciesFlitComp = np.std(latenciesFlitComp, axis=1)
    stdLatenciesPacketComp = np.std(latenciesPacketComp, axis=1)
    stdLatenciesNetworkComp = np.std(latenciesNetworkComp, axis=1)

    middle = meanLatenciesFlitComp + .5 * (meanLatenciesFlit - meanLatenciesFlitComp)
    gain = (meanLatenciesFlit - meanLatenciesFlitComp)/meanLatenciesFlit

    fig = plt.figure()
    plt.ylabel('Latencies in ns', fontsize=11)
    plt.xlabel('Injection Rate', fontsize=11)
    plt.xlim([0, .03])
    linestyle = {'linestyle': '--', 'linewidth': 1, 'markeredgewidth': 1,
                 'elinewidth': 1, 'capsize': 10}
    plt.errorbar(injectionRates, meanLatenciesFlit, yerr=stdLatenciesFlit,
                 color='r', **linestyle, marker='*')
    #plt.errorbar(injectionRates, meanLatenciesNetwork,
    #             yerr=stdLatenciesNetwork, color='b', **linestyle,
    #             marker='s')
    plt.errorbar(injectionRates, meanLatenciesPacket, yerr=stdLatenciesPacket,
                 color='g', **linestyle, marker='^')

    plt.errorbar(injectionRates, meanLatenciesFlitComp, yerr=stdLatenciesFlitComp,
                 color='c', **linestyle, marker='*')
    #plt.errorbar(injectionRates, meanLatenciesNetworkComp,
    #             yerr=stdLatenciesNetworkComp, color='y', **linestyle,
    #             marker='s')
    plt.errorbar(injectionRates, meanLatenciesPacketComp, yerr=stdLatenciesPacketComp,
                 color='k', **linestyle, marker='^')

    for i in range(len(injectionRates)):
        plt.text(injectionRates[i], middle[i], str(int(gain[i]*100))+"%", fontsize='12')

    plt.legend(['Base: Flit', 'Base: Packet', 'Comp: Flit', 'Comp: Packet'])
    fig.suptitle('Latencies', fontsize=16)
    # plt.show()
    fig.savefig('latencies.pdf')
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
            fig = plt.figure()  # plot a figure for each inj_rate and layer
            plt.title('Layer ' + str(layer_id) +
                      ', Injection Rate = ' + str(inj_rate))
            plt.ylabel('Count', fontsize=11)
            plt.xlabel('VC Usage', fontsize=11)
            for col in df.columns.levels[0].values:
                plt.errorbar(df.index.values, df[col, 'mean'].values,
                             yerr=df[col, 'std'].values)
            plt.legend(df.columns.levels[0].values)
            # plt.show()
            fig.savefig('VC_' + str(layer_id) + '_' + str(inj_rate) + '.pdf')
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
        for layer_id, layer_name in enumerate(inj_dict):
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
                         + str(inj_rate), fontsize=16)
            # plt.show()
            fig.savefig('Buff_' + str(layer_id) + '_' + str(inj_rate) + '.pdf')
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


def merge_pdfs(output_path):
    """Merge the generated reports in one pdf."""
    try:
        os.remove(output_path)
    except FileNotFoundError:
        pass

    input_paths = glob.glob('*.pdf')
    input_paths.sort()
    pdf_merger = PdfFileMerger()

    for path in input_paths:
        pdf_merger.append(path)

    with open(output_path, 'wb') as fileobj:
        pdf_merger.write(fileobj)

    for path in input_paths:
        os.remove(path)
###############################################################################


def main():
    """Main Point of Execution."""
    resultsBaseline = read_raw_results('Baseline/rawResults.pkl')
    resultsComp = read_raw_results('RQa/rawResults.pkl')
    resultsAsync = read_raw_results('Asynchronous/rawResults.pkl')

    plot_latencies(resultsBaseline, resultsComp)

    #plot_VCUsage_stats(results['VCUsage'], results['injectionRates'])

    #plot_BuffUsage_stats(results['BuffUsage'], results['injectionRates'])

    merge_pdfs('performance_buffer_VCUsage_report.pdf')
###############################################################################


if __name__ == '__main__':
    main()
