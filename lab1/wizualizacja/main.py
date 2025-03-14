# %%
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from sys import argv

if len(argv) < 2:
    print("Usage: {} <cycles> <points>".format(argv[0]))
    exit(1)

cycle_file = argv[1]
points_file = argv[2]
print(cycle_file, points_file)
dataset, _, algorithm = cycle_file.split('.')[0].split('_')

# Wczytanie danych
cycles = pd.read_csv(cycle_file, header=None).T
points = pd.read_csv(points_file, header=None)

# list of successive points
cycle1 = cycles[0].values
cycle2 = cycles[1].values

x = points[1].values
y = points[2].values

# %%
def connectpoints(x, y, p1, p2, color='black'):
    x1, x2 = x[p1], x[p2]
    y1, y2 = y[p1], y[p2]
    plt.plot([x1, x2], [y1, y2], color=color)

# %%
# visualization of the tsp problem
plt.figure(figsize=(10, 10))
plt.title(f'Wizualizacja dla zbioru {dataset} oraz algorytmu {algorithm}')
plt.xlabel('x')
plt.ylabel('y')
plt.scatter(x, y)
# mark first points in each cycle
plt.scatter(x[cycle1[0]], y[cycle1[0]], color='red', label='Pierwszy cykl', s=100)
plt.scatter(x[cycle2[0]], y[cycle2[0]], color='blue', label='Drugi cykl', s=100)
plt.legend()
for i in range(len(cycle1)-1):
    connectpoints(x, y, cycle1[i], cycle1[i+1], color='red')
    connectpoints(x, y, cycle2[i], cycle2[i+1], color='blue')
# plt.tight_layout()
plt.savefig(f'{dataset}_{algorithm}.png')
# plt.show()
plt.close()
print(f'Zapisano wizualizację do pliku {dataset}_{algorithm}.png')