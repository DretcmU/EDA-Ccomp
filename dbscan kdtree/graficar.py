import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

# Nombres de los datasets
dataset_names = ["noisy_circles", "noisy_moons", "blobs", "aniso", "varied"]
base_path = "."  # Directorio actual
output_dir = "cluster_results"

# Crear carpeta para las imágenes si no existe
os.makedirs(output_dir, exist_ok=True)

# Tamaño de figura general
fig, axs = plt.subplots(len(dataset_names), 2, figsize=(12, 3 * len(dataset_names)))
fig.suptitle("Comparación DBSCAN Clásico vs DBSCAN con KD-Tree", fontsize=16)

for i, name in enumerate(dataset_names):
    for j, suffix in enumerate(["_dbscan.csv", "_kdtree.csv"]):
        method = "DBSCAN" if j == 0 else "KDTree"
        filename = f"{name}{suffix}"
        path = os.path.join(base_path, filename)
        if not os.path.exists(path):
            print(f"Archivo no encontrado: {path}")
            continue

        df = pd.read_csv(path)
        ax = axs[i, j]

        clusters = df["cluster"].unique()
        n_clusters = len(clusters[clusters != -1])
        colors = plt.cm.tab20(np.linspace(0, 1, n_clusters))

        for k, cluster_id in enumerate(sorted(clusters)):
            color = "black" if cluster_id == -1 else colors[k % len(colors)]
            cluster_points = df[df["cluster"] == cluster_id]
            ax.scatter(cluster_points["x"], cluster_points["y"], c=[color], s=10, label=f"Cluster {cluster_id}" if cluster_id != -1 else "Ruido")

        ax.set_title(f"{name} - {method}")
        ax.set_xlabel("x")
        ax.set_ylabel("y")

        # Guardar imagen individual
        fig_single, ax_single = plt.subplots(figsize=(6, 5))
        for k, cluster_id in enumerate(sorted(clusters)):
            color = "black" if cluster_id == -1 else colors[k % len(colors)]
            cluster_points = df[df["cluster"] == cluster_id]
            ax_single.scatter(cluster_points["x"], cluster_points["y"], c=[color], s=10, label=f"Cluster {cluster_id}" if cluster_id != -1 else "Ruido")
        ax_single.set_title(f"{name} - {method}")
        ax_single.set_xlabel("x")
        ax_single.set_ylabel("y")
        ax_single.legend(fontsize=8)
        fig_single.tight_layout()
        fig_single.savefig(os.path.join(output_dir, f"{name}_{method.lower()}.png"))
        plt.close(fig_single)

# Guardar figura general
plt.tight_layout(rect=[0, 0, 1, 0.97])
plt.savefig(os.path.join(output_dir, "comparacion_general.png"))
plt.show()