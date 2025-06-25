import pandas as pd
import matplotlib.pyplot as plt

# Leer resultados
df = pd.read_csv("datasets/resultados_tiempos.csv")

# Obtener lista única de datasets
datasets = df["dataset"].unique()

# Crear gráfico por dataset
for dataset in datasets:
    sub = df[df["dataset"] == dataset]
    plt.figure(figsize=(8, 5))
    plt.plot(sub["n_puntos"], sub["tiempo_clasico_ms"], marker='o', label="DBSCAN clásico")
    plt.plot(sub["n_puntos"], sub["tiempo_kdtree_ms"], marker='s', label="DBSCAN KDTree")
    plt.title(f"Tiempos de ejecución - {dataset}")
    plt.xlabel("Número de puntos")
    plt.ylabel("Tiempo (ms)")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(f"tiempos_{dataset}.png")
    plt.show()