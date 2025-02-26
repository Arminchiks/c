import pygame
import random
import time
import os

pygame.init()

# Ekrāna lielums
WIDTH, HEIGHT = 1800, 1000

# Krāsas
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
BLUE = (0, 0, 255)
GREEN = (0, 255, 0)
YELLOW = (255, 255, 0)
GRAY = (200, 200, 200)

# Bultiņu krāsas un pozīcijas
ARROW_COLORS = [RED, BLUE, GREEN, YELLOW]
ARROW_KEYS = [pygame.K_UP, pygame.K_DOWN, pygame.K_LEFT, pygame.K_RIGHT]
ARROW_POSITIONS = [
    (WIDTH // 2, HEIGHT // 6),       # Up
    (WIDTH // 2, 5 * HEIGHT // 6),   # Down
    (WIDTH // 6, HEIGHT // 2),       # Left
    (5 * WIDTH // 6, HEIGHT // 2),   # Right
]

# Sākuma ekrāns
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Reakcijas tests")

# Fonts
font = pygame.font.Font(None, 50)
small_font = pygame.font.Font(None, 30)

def display_text(text, x, y, color=BLACK):
    """Display text on the screen."""
    text_surface = font.render(text, True, color)
    screen.blit(text_surface, (x, y))

def draw_restart_button():
    """Draw the restart button."""
    button_rect = pygame.Rect(WIDTH // 2 - 100, HEIGHT // 2 + 100, 200, 100)
    pygame.draw.rect(screen, GRAY, button_rect)
    pygame.draw.rect(screen, BLACK, button_rect, 3)
    display_text("RESTART", WIDTH // 2 - 60, HEIGHT // 2 + 130)

def draw_arrows(highlighted=None):
    """Draw arrows on the screen, optionally highlighting one."""
    for i, pos in enumerate(ARROW_POSITIONS):
        color = ARROW_COLORS[i] if i == highlighted else BLACK
        
        if i == 0:  # Bulta uz augsu
            points = [(pos[0], pos[1] - 50), (pos[0] - 40, pos[1] + 50), (pos[0] + 40, pos[1] + 50)]
        elif i == 1:  # Bulta uz leju
            points = [(pos[0], pos[1] + 50), (pos[0] - 40, pos[1] - 50), (pos[0] + 40, pos[1] - 50)]
        elif i == 2:  # Bulta pa kreisi
            points = [(pos[0] - 50, pos[1]), (pos[0] + 50, pos[1] - 40), (pos[0] + 50, pos[1] + 40)]
        else:  # Bulta pa labi
            points = [(pos[0] + 50, pos[1]), (pos[0] - 50, pos[1] - 40), (pos[0] - 50, pos[1] + 40)]
        
        pygame.draw.polygon(screen, color, points)
        pygame.draw.polygon(screen, BLACK, points, 3) 

def draw_start_button():
    """Draw the start button."""
    button_rect = pygame.Rect(WIDTH // 2 - 100, HEIGHT // 2 - 50, 200, 100)
    pygame.draw.rect(screen, GRAY, button_rect)
    pygame.draw.rect(screen, BLACK, button_rect, 3)
    display_text("START", WIDTH // 2 - 50, HEIGHT // 2 - 20)

def display_results(results):
    """Display the results after the game is over."""
    screen.fill(WHITE)
    display_text("Rezultāti", WIDTH // 2 - 100, 50, BLACK)

    y_offset = 120
    for i, result in enumerate(results):
        result_text = f"Mēģinājums {i + 1}: {result:.3f} sekundes"
        result_surface = small_font.render(result_text, True, BLACK)
        screen.blit(result_surface, (WIDTH // 2 - 200, y_offset))
        y_offset += 30

        # Adjust scrolling if results exceed screen height
        if y_offset > HEIGHT - 150:
            break

    if results:
        avg_time = sum(results) / len(results)
        min_time = min(results)
        max_time = max(results)
        stats_text = [
            f"Vidējais: {avg_time:.3f} sekundes",
            f"Minimums: {min_time:.3f} sekundes",
            f"Maksimums: {max_time:.3f} sekundes",
        ]

        y_offset += 30
        for stat in stats_text:
            stat_surface = small_font.render(stat, True, BLACK)
            screen.blit(stat_surface, (WIDTH // 2 - 200, y_offset))
            y_offset += 30

    pygame.display.flip()

def main():
    running = True
    highlighted = None
    last_highlighted = None
    start_time = None
    game_started = False
    start_button_clicked = False
    countdown = 3
    unlit_timer = 0  # Timer for unlit state
    light_delay = 0  # Randomized delay for next highlight
    results = []  # Store reaction times
    max_tests = 2  # Maximum tests
    test_count = 0
    game_over = False

    while running:
        screen.fill(WHITE)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

            if not start_button_clicked:
                if event.type == pygame.MOUSEBUTTONDOWN:
                    mouse_pos = event.pos
                    button_rect = pygame.Rect(WIDTH // 2 - 100, HEIGHT // 2 - 50, 200, 100)
                    if button_rect.collidepoint(mouse_pos):
                        start_button_clicked = True
                        countdown = 3
                        pygame.time.set_timer(pygame.USEREVENT, 1000)

            elif start_button_clicked and countdown == 0 and not game_over:
                if event.type == pygame.KEYDOWN and highlighted is not None:
                    if event.key == ARROW_KEYS[highlighted]:
                        reaction_time = time.time() - start_time
                        results.append(reaction_time)
                        test_count += 1
                        highlighted = None
                        game_started = False
                        if test_count >= max_tests:
                            game_over = True

            elif event.type == pygame.USEREVENT and countdown > 0:
                countdown -= 1
                if countdown == 0:
                    pygame.time.set_timer(pygame.USEREVENT, 0)

            elif game_over and event.type == pygame.MOUSEBUTTONDOWN:
                mouse_pos = event.pos
                button_rect = pygame.Rect(WIDTH // 2 - 100, HEIGHT // 2 + 100, 200, 100)
                if button_rect.collidepoint(mouse_pos):
                    # Reset all variables to initial state
                    highlighted = None
                    last_highlighted = None
                    start_time = None
                    game_started = False
                    start_button_clicked = False
                    countdown = 3
                    unlit_timer = 0
                    light_delay = 0
                    results = []
                    test_count = 0
                    game_over = False

        if not start_button_clicked:
            draw_start_button()
        elif countdown > 0:
            display_text(f"Tests sākas pēc: {countdown}", WIDTH // 2 - 150, HEIGHT // 2 - 30)
        elif game_over:
            display_results(results)
            draw_restart_button()
        else:
            if not game_started:
                if highlighted is None:
                    if unlit_timer == 0:
                        # Unlit state
                        unlit_timer = pygame.time.get_ticks() + 300  # Wait 300ms before showing next
                        light_delay = random.randint(1000, 10000)  # Randomized delay (1-10 seconds)
                    elif pygame.time.get_ticks() >= unlit_timer + light_delay:
                        # Highlight a new arrow
                        last_highlighted = highlighted
                        highlighted = random.randint(0, 3)
                        while highlighted == last_highlighted:
                            highlighted = random.randint(0, 3)
                        unlit_timer = 0  # Reset unlit timer
                        start_time = time.time()
                        game_started = True

            # Draw arrows
            draw_arrows(highlighted)

        pygame.display.flip()

    pygame.quit()

if __name__ == "__main__":
    main()
