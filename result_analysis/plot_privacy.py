#! /usr/bin/python
# -*- encode: utf-8 -*-

"""Usage: python plot_privacy.py name

# name in 'er', 'pl', 'real'
"""

import sys
import os
import math
import matplotlib.pyplot as plt


Attributes = {
    'er': [
        'er_1000000.txt',                                               # Filename
        [r'$p = 0.0001$', r'$p = 0.0002$', r'$p = 0.0003$', ],       # Series
        r'G($n = 1000000, p$)',                                         # Title of G
        0, 400, None, None,                                             # x & y limits
    ],
    'pl': [
        'star_1000000_2.159.txt',
        [r'$\beta = 2.1$', r'$\beta = 2.5$', r'$\beta = 2.9$', ],
        r'Power_law($n = 1000000, \beta$)',
        0, 1200, -7000, None,
    ],
    'real': [
        'real.txt',
        [
            r'Arxiv-CondMat',
            r'YouTube',
            r'LiveJournal',
            r'DBLP',
            r'Facebook',
        ],
        r'Real networks',
        0, 350, -1600, None,
    ],
}

LineStyles = ['-', '--', '-.', '.', '-']


DefaultDataDir = 'G:/Data/SubgraphQuery/result'

LoadMax = 1500

LineWidth = 7.0
FontSize = 25


# Numbers.
n = 1000000


def load_file(name, data_dir):
    """

    return: List of matches, they are big integers.
    """

    attribute = Attributes[name]
    filename, series = attribute[0], attribute[1]

    ser_num = len(series)

    data = [[] for _ in range(ser_num)]

    with open(os.path.join(data_dir, filename), 'r') as f:
        for i, line in enumerate(f):
            if i >= LoadMax:
                break

            words = line.split()

            for j in range(ser_num):
                a = int(words[2 * j])

                if a == 0:
                    data[j].append(None)
                else:
                    data[j].append(math.log10(a) + int(words[2 * j + 1]) - len(words[2 * j]))

    return data


def plot_data(name, data_dir):
    data = load_file(name, data_dir)

    attribute = Attributes[name]

    series, title_g, xmin, xmax, ymin, ymax = attribute[1], attribute[2], attribute[3], attribute[4], attribute[5], attribute[6]

    # plt.semilogy()

    for i, line in enumerate(data):
        xs = list(range(len(line)))

        line_cal = [math.log10(i + 1) - l - math.log10(n) if l is not None else None for l in line]

        plt.plot(xs, line_cal, LineStyles[i], label=attribute[1][i], linewidth=LineWidth)

    plt.legend(fontsize=40, loc='lower left')

    plt.xlim(xmax=xmax, xmin=xmin)
    plt.ylim(ymax=ymax, ymin=ymin)

    plt.xlabel(r'Query size $n_Q$', fontsize=30)
    plt.ylabel(r'$\log_{10}{DAG}$', fontsize=30)

    plt.title(
        r'$G$: {} $Q$: Star($n_Q$)'.format(title_g),
        fontdict={
            'fontname': 'Times New Roman',
            'fontsize': 30,
        },
    )

    plt.show()


def main():
    argc = len(sys.argv)

    name = sys.argv[1]

    if argc <= 2:
        data_dir = DefaultDataDir
    else:
        data_dir = sys.argv[2]

    # Some settings
    # plt.rcParams['font.sans-serif'] = ['Times New Roman']
    plt.rcParams['font.size'] = FontSize

    plot_data(name, data_dir)


if __name__ == '__main__':
    main()
