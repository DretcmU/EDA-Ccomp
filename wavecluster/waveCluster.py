import pygame
import numpy as np
import math

# Datos con algunos puntos negativos
points = [
    (-2, 1), (-2, 3), 
    (-3, 3), (-3, -1),
    (3, 0), (3,3),
    (3, 1), (3, 2),
    (0, 1),(1, 3), 
    (0, 1), (0,0),
    (0,0),(0,0),(3,3)
]

# Calcular rango para acomodar puntos negativos y positivos
min_x = min(p[0] for p in points)
max_x = max(p[0] for p in points)
min_y = min(p[1] for p in points)
max_y = max(p[1] for p in points)

GRID_WIDTH = max_x - min_x 
GRID_HEIGHT = max_y - min_y 

# Función para calcular la siguiente potencia de 2 mayor o igual a n
def next_power_of_two(n):
    return 1 if n == 0 else 2 ** math.ceil(math.log2(n))

# GRID_SIZE será la potencia de 2 que cubra el máximo entre ancho y alto
GRID_SIZE = next_power_of_two(max(GRID_WIDTH, GRID_HEIGHT))
print(max(GRID_WIDTH, GRID_HEIGHT), min_x,min_y,max_x,max_y,GRID_SIZE)

# Configuración pantalla
WIDTH, HEIGHT = 600, 600
CELL_SIZE = WIDTH // GRID_SIZE

BACKGROUND = (240, 240, 240)
POINT_COLOR = (30, 30, 150)
HIGHLIGHT_COLOR = (80, 200, 120)
THRESHOLD = 0.2  # Para WaveCluster

# Offset para mapear coordenadas a índices de matriz (que son >=0)
offset_x = -min_x
offset_y = -min_y

# Inicializar matriz de densidad con dimensiones GRID_SIZE x GRID_SIZE (potencia de 2)
density = np.zeros((GRID_SIZE, GRID_SIZE), dtype=int)
for x, y in points:
    density[y + offset_y][x + offset_x] += 1

result_wavelet = None  # global

def haar_wavelet_transform_2d(matrix):
    m = matrix.copy().astype(float)
    size = m.shape[0]
    temp = np.zeros_like(m)

    # Paso 1: Filas
    for y in range(size):
        for x in range(0, size, 2):
            avg = (m[y][x] + m[y][x+1]) / 2
            diff = (m[y][x] - m[y][x+1]) / 2
            temp[y][x//2] = avg
            temp[y][x//2 + size//2] = diff
    print("A:\n",temp)
    # Paso 2: Columnas
    result = np.zeros_like(m)
    for x in range(size):
        for y in range(0, size, 2):
            avg = (temp[y][x] + temp[y+1][x]) / 2
            diff = (temp[y][x] - temp[y+1][x]) / 2
            result[y//2][x] = avg
            result[y//2 + size//2][x] = diff
    print(result)
    return result

def density_to_color(value, max_value):
    if max_value == 0 or not np.isfinite(value):
        return (255, 255, 255)
    ratio = max(0.0, min(1.0, value / max_value))
    r = int(255 * ratio)
    g = int(255 * (1 - ratio))
    b = int(255 * (1 - ratio))
    return (r, g, g)

def draw_grid(screen, highlights=None, mode="normal", matrix_to_display=None):
    screen.fill(BACKGROUND)
    font = pygame.font.SysFont("Arial", 14)
    big_font = pygame.font.SysFont("Arial", 16, bold=True)

    max_val = np.max(matrix_to_display) if matrix_to_display is not None else np.max(density)

    for y in range(GRID_SIZE):
        for x in range(GRID_SIZE):
            rect = pygame.Rect(x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE)

            # Visualización
            if mode == "heatmap":
                val = matrix_to_display[y][x] if matrix_to_display is not None else 0
                color = density_to_color(val, max_val)
                pygame.draw.rect(screen, color, rect)
            elif highlights is not None and highlights[y][x]:
                pygame.draw.rect(screen, HIGHLIGHT_COLOR, rect)

            pygame.draw.rect(screen, (200, 200, 200), rect, 1)

            # Texto del valor (centrado)
            val = matrix_to_display[y][x] if matrix_to_display is not None else density[y][x]
            if isinstance(val, float):
                val_text = big_font.render(f"{val:.2f}", True, (0, 0, 0))
            else:
                val_text = big_font.render(str(val), True, (0, 0, 0))
            text_rect = val_text.get_rect(center=(x*CELL_SIZE + CELL_SIZE//2, y*CELL_SIZE + CELL_SIZE//2))
            screen.blit(val_text, text_rect)

            # Coordenadas (arriba a la izquierda)
            coord_x = x - offset_x
            coord_y = y - offset_y
            coord_text = font.render(f"({coord_x},{coord_y})", True, (80, 80, 80))
            coord_pos = (x * CELL_SIZE + 2, y * CELL_SIZE + 2)
            screen.blit(coord_text, coord_pos)

def main():
    global result_wavelet

    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("WaveCluster con Grilla y Mapa de Calor")

    running = True
    highlight = np.zeros_like(density)
    show_clusters = False
    heatmap_mode = False
    wavelet_mode = False

    while running:
        if wavelet_mode:
            draw_grid(screen, mode="heatmap", matrix_to_display=result_wavelet)
        elif heatmap_mode:
            draw_grid(screen, mode="heatmap", matrix_to_display=density)
        else:
            draw_grid(screen, highlight if show_clusters else None)

        pygame.display.flip()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RETURN:
                    result_wavelet = haar_wavelet_transform_2d(density)
                    half = GRID_SIZE // 2
                    approx = result_wavelet[:half, :half]
                    approx += result_wavelet[:half, half:]
                    approx += result_wavelet[half:, :half]
                    approx += result_wavelet[half:, half:]

                    for y in range(GRID_SIZE//2):
                        for x in range(GRID_SIZE//2):
                            if approx[y][x] >= THRESHOLD:
                                highlight[y*2][x*2] = 1
                                highlight[y*2+1][x*2] = 1
                                highlight[y*2][x*2+1] = 1
                                highlight[y*2+1][x*2+1] = 1

                    show_clusters = True
                    heatmap_mode = False
                    wavelet_mode = False

                elif event.key == pygame.K_SPACE:
                    heatmap_mode = not heatmap_mode
                    show_clusters = False
                    wavelet_mode = False

                elif event.key == pygame.K_a:
                    if result_wavelet is None:
                        result_wavelet = haar_wavelet_transform_2d(density)
                    wavelet_mode = True
                    heatmap_mode = False
                    show_clusters = False

    pygame.quit()

if __name__ == "__main__":
    main()
