import matplotlib.pyplot as plt
import numpy as np

con = 0
for dimension in [10, 50, 100, 500, 1000, 2000, 5000]:
    with open(f"{dimension}.txt", "r") as file:
        distances = [0] + [float(distance) for distance in file.read().split()]
        plt.title(f"Dimension: {dimension}, Distances: {len(distances)-1}")
        plt.xlabel('Distancias')
        plt.ylabel('Frecuencias')
        plt.hist(distances, edgecolor = "white", bins=50+con)
        con+=10
        plt.show()