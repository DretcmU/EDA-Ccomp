import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import random

def graficar_rtree(csv_mbrs, csv_puntos=None, titulo="R-Tree Visualizado"):
    df = pd.read_csv(csv_mbrs)

    # Si no hay nivel, asigna uno ficticio
    if 'nivel' not in df.columns:
        df['nivel'] = 0

    colores_niveles = [
        'red', 'blue', 'green', 'orange', 'purple', 'cyan', 'magenta',
        'brown', 'lime', 'olive', 'gray', 'black'
    ]

    fig, ax = plt.subplots(figsize=(8, 8))
    ax.set_title(titulo)

    # Dibujar los MBRs
    for _, row in df.iterrows():
        xmin, ymin, xmax, ymax, nivel = row['xmin'], row['ymin'], row['xmax'], row['ymax'], row['nivel']
        color = colores_niveles[int(nivel) % len(colores_niveles)]
        rect = patches.Rectangle((xmin, ymin), xmax - xmin, ymax - ymin,
                                 linewidth=1, edgecolor=color, facecolor='none')
        ax.add_patch(rect)

    # Dibujar los puntos, si los hay
    if csv_puntos:
        puntos = pd.read_csv(csv_puntos)
        if {'x', 'y'}.issubset(puntos.columns):
            ax.scatter(puntos['x'], puntos['y'], c='black', s=2)

    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_aspect('equal', adjustable='box')
    plt.grid(True)
    plt.show()

# Ejemplo de uso
graficar_rtree("mbrs_rtree.csv", csv_puntos="puntos.csv", titulo="Visualización del R-Tree")
