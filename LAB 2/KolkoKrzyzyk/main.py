import pygame

pygame.init()
win = pygame.display.set_mode((600, 600))
pygame.display.set_caption("Kółko i Krzyżyk")

WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (200, 0, 0)
BLUE = (0, 0, 200)

board = [[None, None, None], [None, None, None], [None, None, None]]
current_player = 'X'
font = pygame.font.Font(None, 250)

win.fill(WHITE)
pygame.draw.line(win, BLACK, (200, 0), (200, 600), 5)
pygame.draw.line(win, BLACK, (400, 0), (400, 600), 5)
pygame.draw.line(win, BLACK, (0, 200), (600, 200), 5)
pygame.draw.line(win, BLACK, (0, 400), (600, 400), 5)
pygame.display.flip()

run = True
while run:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False

        if event.type == pygame.MOUSEBUTTONDOWN:
            x, y = event.pos
            col = x // 200
            row = y // 200

            if board[row][col] is None:
                board[row][col] = current_player

                if current_player == 'X':
                    symbol_color = RED
                else:
                    symbol_color = BLUE

                text = font.render(current_player, True, symbol_color)
                text_rect = text.get_rect(center=(col * 200 + 100, row * 200 + 100))
                win.blit(text, text_rect)
                pygame.display.flip()

                current_player = 'O' if current_player == 'X' else 'X'