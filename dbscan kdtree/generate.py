import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from sklearn import datasets

# 1. Generar datasets
n_samples = 1500

noisy_circles, _ = datasets.make_circles(n_samples=n_samples, factor=0.5, noise=0.05, random_state=170)
noisy_moons, _ = datasets.make_moons(n_samples=n_samples, noise=0.05, random_state=170)
blobs, _ = datasets.make_blobs(n_samples=n_samples, random_state=170)

X_base, _ = datasets.make_blobs(n_samples=n_samples, random_state=170)
transformation = [[0.6, -0.6], [-0.4, 0.8]]
X_aniso = np.dot(X_base, transformation)

varied, _ = datasets.make_blobs(n_samples=n_samples, cluster_std=[1.0, 2.5, 0.5], random_state=170)

datasets_dict = {
    "noisy_circles.csv": noisy_circles,
    "noisy_moons.csv": noisy_moons,
    "blobs.csv": blobs,
    "aniso.csv": X_aniso,
    "varied.csv": varied,
}

for name, data in datasets_dict.items():
    df = pd.DataFrame(data, columns=["x", "y"])
    df.to_csv("datasets/"+name, index=False)
    print(f"Guardado: {name}")


fig, axs = plt.subplots(2, 3, figsize=(15, 10))
axs = axs.flatten()

titles = ["Noisy Circles", "Noisy Moons", "Blobs", "Anisotropic", "Varied"]
filenames = list(datasets_dict.keys())

for i, (title, filename) in enumerate(zip(titles, filenames)):
    df = pd.read_csv("datasets/"+filename)
    axs[i].scatter(df["x"], df["y"], s=5)
    axs[i].set_title(title)
    axs[i].set_xlabel("x")
    axs[i].set_ylabel("y")

# Eliminar último subplot vacío
fig.delaxes(axs[-1])
plt.tight_layout()
plt.show()