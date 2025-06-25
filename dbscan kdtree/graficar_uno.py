import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

# Dataset específico a visualizar
dataset_name = "noisy_moons"  # <-- cambia esto si quieres otro
base_path = "."
output_dir = "cluster_results"

# Crear carpeta si no existe
os.makedirs(output_dir, exist_ok=True)

# Métodos a comparar
methods = [("DBSCAN", "_dbscan.csv"), ("KDTree", "_kdtree.csv")]

# Preparar figura general
fig, axs = plt.subplots(1, 2, figsize=(12, 5))
fig.suptitle(f"Comparación de métodos para: {dataset_name}", fontsize=16)

for j, (method, suffix) in enumerate(methods):
    filename = f"datasets/{dataset_name}{suffix}"
    path = os.path.join(base_path, filename)
    if not os.path.exists(path):
        print(f"Archivo no encontrado: {path}")
        continue

    df = pd.read_csv(path)
    ax = axs[j]

    clusters = df["cluster"].unique()
    n_clusters = len(clusters[clusters != -1])
    colors = plt.cm.tab20(np.linspace(0, 1, n_clusters))

    for k, cluster_id in enumerate(sorted(clusters)):
        color = "black" if cluster_id == -1 else colors[k % len(colors)]
        cluster_points = df[df["cluster"] == cluster_id]
        ax.scatter(cluster_points["x"], cluster_points["y"], c=[color], s=10, label=f"Cluster {cluster_id}" if cluster_id != -1 else "Ruido")

    ax.set_title(f"{dataset_name} - {method}")
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.legend(fontsize=8)

# Guardar y mostrar
plt.tight_layout(rect=[0, 0, 1, 0.95])
plt.savefig(os.path.join(output_dir, f"{dataset_name}_comparacion.png"))
plt.show()