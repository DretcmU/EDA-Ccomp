import matplotlib.pyplot as plt
import numpy as np

con = 0
means = []
dimensions = [10, 50, 100, 500, 1000, 2000, 5000]
for dimension in dimensions:
    with open(f"distancias_{dimension}.txt", "r") as file:
        distancias = [0] + [float(distancia) for distancia in file.read().split()]
        plt.title(f"Dimension: {dimension}, Distancias: {len(distancias)}")
        plt.xlabel('Distancias')
        plt.ylabel('Frecuencias')
        plt.hist(distancias, edgecolor = "white", bins=50+con)
        con+=10
        plt.show()
        means.append(np.mean(distancias))
    

plt.title("Resumen")
plt.xlabel('Dimensiones')
plt.ylabel('Distancias avg')
plt.plot(dimensions,means)
plt.show()