import pygame
import numpy as np
import math
from sklearn.cluster import DBSCAN
# Al inicio, después de importar módulos:
import random

def label_to_color(label):
    label = int(label)  # Convertir a int nativo de Python
    random.seed(label)
    return tuple(random.randint(50, 255) for _ in range(3)) if label != -1 else (100, 100, 100)


points = [
    (-2, 1),(-3, 6),(-3, 5), 
    (-3, -1),(3, 4), (3, 6),
    (3, 5), (4, 5),(-2,1),
    (0, 1), (-2, 5),(-1,1), 
    (0, 1), (0, 0), (-1,0),
    (-1,0),(0, 0), (0, 0), 
    (3, 3),(-3,6)
]

# import math

# def generate_smiley_points():
#     points = []

#     # Centro de la cara
#     cx, cy = 0, 0
#     radio = 8

#     # Borde de la cara (círculo grande)
#     for angle in range(0, 360, 5):
#         rad = math.radians(angle)
#         x = int(cx + radio * math.cos(rad))
#         y = int(cy + radio * math.sin(rad))
#         points.append((x, y))

#     # Ojo izquierdo
#     for dx in range(-1, 2):
#         for dy in range(-1, 2):
#             points.append((cx - 3 + dx, cy + 3 + dy))

#     # Ojo derecho
#     for dx in range(-1, 2):
#         for dy in range(-1, 2):
#             points.append((cx + 3 + dx, cy + 3 + dy))

#     # Sonrisa (arco)
#     for angle in range(210, 330, 5):
#         rad = math.radians(angle)
#         x = int(cx + (radio - 2) * math.cos(rad))
#         y = int(cy + (radio - 2) * math.sin(rad))
#         points.append((x, y))

#     return points

# points = generate_smiley_points()

# Rango para grid
min_x = min(p[0] for p in points)
max_x = max(p[0] for p in points)
min_y = min(p[1] for p in points)
max_y = max(p[1] for p in points)
GRID_WIDTH = max_x - min_x 
GRID_HEIGHT = max_y - min_y 

def next_power_of_two(n):
    return 1 if n == 0 else 2 ** math.ceil(math.log2(n))

GRID_SIZE = next_power_of_two(max(GRID_WIDTH, GRID_HEIGHT) + 1)
WIDTH, HEIGHT = 600, 600
CELL_SIZE = WIDTH // GRID_SIZE
BACKGROUND = (240, 240, 240)
POINT_COLOR = (30, 30, 150)
HIGHLIGHT_COLOR = (80, 200, 120)
THRESHOLD = 0.4

offset_x = -min_x
offset_y = -min_y

density = np.zeros((GRID_SIZE, GRID_SIZE), dtype=int)
for x, y in points:
    density[y + offset_y][x + offset_x] += 1

result_wavelet = None
labels = np.full_like(density, -1, dtype=int)

def haar_wavelet_transform_2d(matrix):
    m = matrix.copy().astype(float)
    size = m.shape[0]
    temp = np.zeros_like(m)

    for y in range(size):
        for x in range(0, size, 2):
            avg = (m[y][x] + m[y][x+1]) / 2
            diff = (m[y][x] - m[y][x+1]) / 2
            temp[y][x//2] = avg
            temp[y][x//2 + size//2] = diff

    result = np.zeros_like(m)
    for x in range(size):
        for y in range(0, size, 2):
            avg = (temp[y][x] + temp[y+1][x]) / 2
            diff = (temp[y][x] - temp[y+1][x]) / 2
            result[y//2][x] = avg
            result[y//2 + size//2][x] = diff
    return result

def density_to_color(value, max_value):
    if max_value == 0 or not np.isfinite(value):
        return (255, 255, 255)
    ratio = max(0.0, min(1.0, value / max_value))
    r = int(255 * ratio)
    g = int(255 * (1 - ratio))
    b = int(255 * (1 - ratio))
    return (r, g, g)

# Modifica la función draw_grid:
def draw_grid(screen, highlights=None, mode="normal", matrix_to_display=None, show_clusters=False):
    font = pygame.font.SysFont("Arial", 14)
    big_font = pygame.font.SysFont("Arial", 16, bold=True)
    screen.fill(BACKGROUND)

    max_val = np.max(matrix_to_display) if matrix_to_display is not None else np.max(density)

    for y in range(GRID_SIZE):
        for x in range(GRID_SIZE):
            rect = pygame.Rect(x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE)

            if mode == "heatmap":
                val = matrix_to_display[y][x] if matrix_to_display is not None else 0
                color = density_to_color(val, max_val)
                pygame.draw.rect(screen, color, rect)
            elif show_clusters and labels[y][x] != -1:
                pygame.draw.rect(screen, label_to_color(labels[y][x]), rect)
            elif highlights is not None and highlights[y][x]:
                pygame.draw.rect(screen, HIGHLIGHT_COLOR, rect)

            pygame.draw.rect(screen, (200, 200, 200), rect, 1)

            val = matrix_to_display[y][x] if matrix_to_display is not None else density[y][x]
            if show_clusters and labels[y][x] != -1:
                val_text = big_font.render(str(labels[y][x]), True, (0, 0, 0))
            elif isinstance(val, float):
                val_text = big_font.render(f"{val:.2f}", True, (0, 0, 0))
            else:
                val_text = big_font.render(str(val), True, (0, 0, 0))

            text_rect = val_text.get_rect(center=(x*CELL_SIZE + CELL_SIZE//2, y*CELL_SIZE + CELL_SIZE//2))
            screen.blit(val_text, text_rect)

            coord_x = x - offset_x
            coord_y = y - offset_y
            coord_text = font.render(f"({coord_x},{coord_y})", True, (80, 80, 80))
            coord_pos = (x * CELL_SIZE + 2, y * CELL_SIZE + 2)
            screen.blit(coord_text, coord_pos)

def main():
    global result_wavelet, labels

    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("WaveCluster + Clustering")

    running = True
    highlight = np.zeros_like(density)
    show_clusters = False
    heatmap_mode = False
    wavelet_mode = False
    highlight_mode = False

    while running:
        if wavelet_mode:
            draw_grid(screen, mode="heatmap", matrix_to_display=result_wavelet)
        elif heatmap_mode:
            draw_grid(screen, mode="heatmap", matrix_to_display=density)
        elif highlight_mode:
            draw_grid(screen, highlights=highlight, show_clusters=False)
        else:
            draw_grid(screen, highlights=highlight if show_clusters else None, show_clusters=show_clusters)

        pygame.display.flip()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_s:
                    # Aplicar transformada Wavelet y mostrar solo puntos relevantes
                    result_wavelet = haar_wavelet_transform_2d(density)
                    half = GRID_SIZE // 2
                    highlight = np.zeros_like(density)

                    for y in range(half):
                        for x in range(half):
                            val = result_wavelet[:half, :half][y][x]
                            if val >= THRESHOLD:
                                for dy in [0, 1]:
                                    for dx in [0, 1]:
                                        yy = y * 2 + dy
                                        xx = x * 2 + dx
                                        highlight[yy][xx] = 1

                    highlight_mode = True
                    show_clusters = False
                    heatmap_mode = False
                    wavelet_mode = False

                elif event.key == pygame.K_RETURN:
                    # Aplicar clustering sobre puntos resaltados
                    dense_points = []
                    labels = np.full_like(density, -1, dtype=int)

                    for y in range(GRID_SIZE):
                        for x in range(GRID_SIZE):
                            if highlight[y][x] == 1:
                                dense_points.append([y, x])

                    if dense_points:
                        clustering = DBSCAN(eps=1.0, min_samples=2).fit(dense_points)
                        for point, label in zip(dense_points, clustering.labels_):
                            y, x = point
                            labels[y][x] = label

                    print(dense_points)
                    print(labels)
                    show_clusters = True
                    highlight_mode = False
                    heatmap_mode = False
                    wavelet_mode = False

                elif event.key == pygame.K_SPACE:
                    heatmap_mode = not heatmap_mode
                    wavelet_mode = False
                    highlight_mode = False
                    show_clusters = False

                elif event.key == pygame.K_a:
                    if result_wavelet is None:
                        result_wavelet = haar_wavelet_transform_2d(density)
                    wavelet_mode = True
                    heatmap_mode = False
                    show_clusters = False
                    highlight_mode = False

    pygame.quit()

if __name__ == "__main__":
    main()
