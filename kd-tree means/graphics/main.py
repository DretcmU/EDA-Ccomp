import pandas as pd
import matplotlib.pyplot as plt

# # Inicializar listas para almacenar los DataFrames
# kd_data = []
# force_data = []


# for numero in range(1,11,1):
#     kd_file = f"../Test_{numero}.csv"
#     force_file = f"../Test_force_{numero}.csv"

#     df_kd = pd.read_csv(kd_file)
#     df_force = pd.read_csv(force_file)

#     kd_data.append(df_kd)
#     force_data.append(df_force)

#     # Graficar
#     plt.figure(figsize=(10, 6))
#     plt.plot(df_kd["iteracion"], df_kd["centroides_movidos"], label="KD-Tree", marker='o')
#     plt.plot(df_force["iteracion"], df_force["centroides_movidos"], label="Fuerza bruta", marker='x')

#     plt.xlabel("Iteración")
#     plt.ylabel("Centroides movidos")
#     plt.title(f"Comparación de convergencia (Test {numero})")
#     plt.legend()
#     plt.grid(True)
#     plt.tight_layout()
#     plt.savefig(f"diesVeces/comparacion_Test_{numero}.png")
#     plt.show()


# # Concatenar todos los DataFrames por filas (suponiendo mismas iteraciones)
# kd_all = pd.concat(kd_data)
# force_all = pd.concat(force_data)

# # Agrupar por iteración y calcular promedio
# kd_avg = kd_all.groupby("iteracion")["centroides_movidos"].mean().reset_index()
# force_avg = force_all.groupby("iteracion")["centroides_movidos"].mean().reset_index()

# # Graficar promedios
# plt.figure(figsize=(10, 6))
# plt.plot(kd_avg["iteracion"], kd_avg["centroides_movidos"], label="KD-Tree (Promedio)", marker='o')
# plt.plot(force_avg["iteracion"], force_avg["centroides_movidos"], label="Fuerza bruta (Promedio)", marker='x')

# plt.xlabel("Iteración")
# plt.ylabel("Centroides movidos (promedio)")
# plt.title("Promedio de convergencia en 10 pruebas")
# plt.legend()
# plt.grid(True)
# plt.tight_layout()
# plt.savefig("diesVeces/comparacion_promedio.png")
# plt.show()

# Leer datos
df_kdtree = pd.read_csv("../tiempos.csv")
df_brute = pd.read_csv("../fuerza_bruta.csv")


pivot_kdtree = df_kdtree.pivot(index="n", columns="k", values="tiempo_ms")
pivot_brute = df_brute.pivot(index="n", columns="k", values="tiempo_ms")

# eje x = n, eje y = tiempo, para cada k ===
plt.figure(figsize=(10, 6))
for k in pivot_kdtree.columns:
    plt.plot(pivot_brute.index, pivot_brute[k], label=f'brute-force')    
    plt.plot(pivot_kdtree.index, pivot_kdtree[k], label=f'k-d tree')
    plt.xlabel("n")
    plt.ylabel("runing time(ms)")
    title=f"cluster_points, k=={k}"
    plt.title(title)
    plt.legend()
    plt.grid(True)
    plt.savefig(f"matplot/{title}.png")
    plt.show()

# #  eje x = k, eje y = tiempo, para cada n ===
# pivot_kdtree_by_k = df_kdtree.pivot(index="k", columns="n", values="tiempo_ms")
# pivot_brute_by_k = df_brute.pivot(index="k", columns="n", values="tiempo_ms")

# for n in pivot_kdtree_by_k.columns:
#     plt.figure(figsize=(10, 6))
#     plt.plot(pivot_brute_by_k.index, pivot_brute_by_k[n], label="brute-force")
#     plt.plot(pivot_kdtree_by_k.index, pivot_kdtree_by_k[n], label="k-d tree")
#     plt.xlabel("k")
#     plt.ylabel("runing time(ms)")
#     title=f"cluster_points, n=={n}"
#     plt.title(title)
#     plt.legend()
#     plt.grid(True)
#     plt.savefig(f"matplot/{title}.png")
#     plt.show()

    
