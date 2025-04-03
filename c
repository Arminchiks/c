import pygame
import random
import time

pygame.init()


WIDTH, HEIGHT = 1800, 1000

WHITE = (240, 240, 240)  
LIGHT_GRAY = (220, 220, 220)  
BLACK = (20, 20, 20)  
RED = (255, 50, 50)
BLUE = (50, 50, 255)
GREEN = (50, 255, 50)
YELLOW = (255, 255, 50)
BUTTON_COLOR = (70, 130, 180)  
BUTTON_HOVER_COLOR = (100, 160, 210)
SHADOW_COLOR = (100, 100, 100)  
BORDER_COLOR = (150, 150, 150)


ARROW_COLORS = [RED, BLUE, GREEN, YELLOW]
ARROW_KEYS = [pygame.K_UP, pygame.K_DOWN, pygame.K_LEFT, pygame.K_RIGHT]
ARROW_POSITIONS = [
    (WIDTH // 2, HEIGHT // 6),       # Uz augšu
    (WIDTH // 2, 5 * HEIGHT // 6),   # Uz leju
    (WIDTH // 6, HEIGHT // 2),       # Pa kreisi
    (5 * WIDTH // 6, HEIGHT // 2),   # Pa labi
]


screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Reakcijas Tests")


font = pygame.font.SysFont("Arial", 40)
small_font = pygame.font.SysFont("Arial", 30)
title_font = pygame.font.SysFont("Arial", 60, bold=True)  

running = True
highlighted = None
last_highlighted = None
start_time = None
game_started = False
start_button_clicked = False
countdown = 3
unlit_timer = 0
light_delay = 0
results = []
max_tests = 5 
test_count = 0
game_over = False
start_button_rect = None
instruction_button_rect = None
quit_button_rect = None


def draw_gradient_background():
    """Draw a subtle gradient background."""
    for i in range(HEIGHT):
        color = (
            int(WHITE[0] + (LIGHT_GRAY[0] - WHITE[0]) * i / HEIGHT),
            int(WHITE[1] + (LIGHT_GRAY[1] - WHITE[1]) * i / HEIGHT),
            int(WHITE[2] + (LIGHT_GRAY[2] - WHITE[2]) * i / HEIGHT),
        )
        pygame.draw.line(screen, color, (0, i), (WIDTH, i))

def draw_rounded_button(text, x, y, width, height, color, hover_color):

    mouse_pos = pygame.mouse.get_pos()
    button_rect = pygame.Rect(x, y, width, height)
    
    current_color = color
    if button_rect.collidepoint(mouse_pos):
        current_color = hover_color
    
    shadow_offset = 5
    shadow_rect = pygame.Rect(x + shadow_offset, y + shadow_offset, width, height)
    pygame.draw.rect(screen, SHADOW_COLOR, shadow_rect, border_radius=15)
    
    
    pygame.draw.rect(screen, current_color, button_rect, border_radius=15)
    
    text_surface = font.render(text, True, WHITE) 
    text_rect = text_surface.get_rect(center=button_rect.center)
    screen.blit(text_surface, text_rect)
    
    return button_rect

def display_text(text, x, y, color=BLACK, center=True, font_name="Arial", font_size=30, bold=False):
  
    font = pygame.font.SysFont(font_name, font_size, bold)
    text_surface = font.render(text, True, color)
    text_rect = text_surface.get_rect(center=(x, y)) if center else text_surface.get_rect(topleft=(x, y))
    screen.blit(text_surface, text_rect)

def instrukcija_screen():
   
    instructions_running = True

    while instructions_running:
        draw_gradient_background()
        display_text("Instrukcija", WIDTH // 2, 100, font_size=50, bold=True)

        instruction_lines = [
            "1. Spied 'SĀKT', lai sāktu testu.",
            "2. Kad bulta parādās, nospied attiecīgo bulttaustiņu.",
            "3. Tavs reakcijas laiks tiks mērīts.",
            "4. Veic vairākus mēģinājumus un apskati rezultātus.",
        ]

        y_offset = 200
        for line in instruction_lines:
            display_text(line, WIDTH // 2, y_offset, font_size=35)
            y_offset += 50

        return_button_rect = draw_rounded_button("ATGRIEZTIES", WIDTH // 2 - 150, HEIGHT - 150, 300, 80, BUTTON_COLOR, BUTTON_HOVER_COLOR)

        pygame.display.flip()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                return False 
            if event.type == pygame.MOUSEBUTTONDOWN:
                mouse_pos = event.pos
                if return_button_rect.collidepoint(mouse_pos):
                    return True

    return True 

def draw_arrows(highlighted=None):
 
    arrow_size = 140  
    glow_radius = 24  

    for i, pos in enumerate(ARROW_POSITIONS):
        color = ARROW_COLORS[i] if i == highlighted else BLACK

        
        x, y = pos

       
        if i == 0:  # Uz augšu
            points = [(x, y - arrow_size), (x - arrow_size // 2, y), (x + arrow_size // 2, y)]
        elif i == 1:  # Uz leju
            points = [(x, y + arrow_size), (x - arrow_size // 2, y), (x + arrow_size // 2, y)]
        elif i == 2:  # Kreisais
            points = [(x - arrow_size, y), (x, y - arrow_size // 2), (x, y + arrow_size // 2)]
        else:  # Labais
            points = [(x + arrow_size, y), (x, y - arrow_size // 2), (x, y + arrow_size // 2)]

       
        if i == highlighted:
            for r in range(glow_radius, 0, -2):
                glow_color = color + (50,) 
                pygame.draw.polygon(screen, glow_color, points)

    
        pygame.draw.polygon(screen, color, points)

def display_results(results):
   
    draw_gradient_background()
    display_text("Rezultāti", WIDTH // 2, 50, color=BLACK, font_size=50, bold=True)
    
    y_offset = 120
    for i, result in enumerate(results):
        result_text = f"Mēģinājums {i + 1}: {result:.3f} sekundes"
        display_text(result_text, WIDTH // 2, y_offset, color=BLACK, font_size=30)
        y_offset += 40

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

        y_offset += 50
        for stat in stats_text:
            display_text(stat, WIDTH // 2, y_offset, color=BLACK, font_size=30)
            y_offset += 40

    restart_button_rect = draw_rounded_button("VĒLREIZ", WIDTH // 2 - 150, HEIGHT - 150, 300, 80, BUTTON_COLOR, BUTTON_HOVER_COLOR)
    pygame.display.flip()
    return restart_button_rect

def main():
  
    global running, highlighted, last_highlighted, start_time, game_started
    global start_button_clicked, countdown, unlit_timer, light_delay, results, test_count, game_over
    global start_button_rect, instruction_button_rect, quit_button_rect, max_tests

    running = True
    highlighted = None
    last_highlighted = None
    start_time = None
    game_started = False
    start_button_clicked = False
    countdown = 3
    unlit_timer = 0
    light_delay = 0
    results = []
    max_tests = 3
    test_count = 0
    game_over = False

    while running:
        draw_gradient_background()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif not start_button_clicked:
                if event.type == pygame.MOUSEBUTTONDOWN:
                    mouse_pos = event.pos
                    if start_button_rect.collidepoint(mouse_pos):
                        start_button_clicked = True
                        countdown = 3
                        pygame.time.set_timer(pygame.USEREVENT, 1000)
                    elif instruction_button_rect.collidepoint(mouse_pos):
                        running = instrukcija_screen()
                    elif quit_button_rect.collidepoint(mouse_pos):
                        running = False
            elif event.type == pygame.USEREVENT and countdown > 0:
                countdown -= 1
                if countdown == 0:
                    pygame.time.set_timer(pygame.USEREVENT, 0)
            elif game_over and event.type == pygame.MOUSEBUTTONDOWN:
                mouse_pos = event.pos
                restart_button_rect = display_results(results)
                if restart_button_rect.collidepoint(mouse_pos):
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
            elif event.type == pygame.KEYDOWN and highlighted is not None:
                if event.key == ARROW_KEYS[highlighted]:
                    reaction_time = time.time() - start_time
                    results.append(reaction_time)
                    test_count += 1
                    highlighted = None
                    game_started = False
                    if test_count >= max_tests:
                        game_over = True

        if not start_button_clicked:
        
            border_rect = pygame.Rect(WIDTH // 6 - 5, HEIGHT // 6 - 5, 2 * WIDTH // 3 + 10 , 2 * HEIGHT // 3 + 10 )
            pygame.draw.rect(screen, BORDER_COLOR, border_rect, 5)

        
            start_button_rect = draw_rounded_button("SĀKT", WIDTH // 2 - 150, HEIGHT // 2 - 100, 300, 80, BUTTON_COLOR, BUTTON_HOVER_COLOR)
            instruction_button_rect = draw_rounded_button("Instrukcija", WIDTH // 2 - 150, HEIGHT // 2 + 20, 300, 80, BUTTON_COLOR, BUTTON_HOVER_COLOR)
            quit_button_rect = draw_rounded_button("IZIET", WIDTH // 2 - 150, HEIGHT // 2 + 140, 300, 80, BUTTON_COLOR, BUTTON_HOVER_COLOR)

            display_text("Reakcijas Tests", WIDTH // 2, HEIGHT // 4, font_size=70, color=BLACK, bold=True)
        elif countdown > 0:
            display_text(f"Tests sākas pēc: {countdown}", WIDTH // 2, HEIGHT // 2 - 30, font_size=45)
        elif game_over:
            display_results(results)
        else:
            if not game_started:
                if highlighted is None:
                    if unlit_timer == 0:
                        unlit_timer = pygame.time.get_ticks() + 300
                        light_delay = random.randint(1000, 8000)
                    elif pygame.time.get_ticks() >= unlit_timer + light_delay:
                        last_highlighted = highlighted
                        highlighted = random.randint(0, 3)
                        while highlighted == last_highlighted:
                            highlighted = random.randint(0, 3)
                        unlit_timer = 0
                        start_time = time.time()
                        game_started = True

            draw_arrows(highlighted)

        pygame.display.flip()


    pygame.quit()

if __name__ == "__main__":
    main()
