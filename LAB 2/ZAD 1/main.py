import pygame
import sys

pygame.init()
width = 600
height = 600
win = pygame.display.set_mode((width, height))
pygame.display.set_caption("Zadanie 1")

CZARNY = (0, 0, 0)
ZIELONY = (0, 255, 0)

try:
    oryginal_img = pygame.image.load('maxresdefault.jpg').convert()

    oryginal_img.set_colorkey(ZIELONY)

    cat = pygame.transform.scale(oryginal_img, (300, 300))
except:
    cat = pygame.Surface((300, 300))
    cat.fill(ZIELONY)

win.fill(CZARNY)
srodek_x = (width - cat.get_width()) // 2
srodek_y = (height - cat.get_height()) // 2

win.blit(cat, (srodek_x, srodek_y))
pygame.display.flip()

run = True
while run:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False
            pygame.quit()
            sys.exit()
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_1:
                z1 = pygame.transform.scale(cat, (int(width * 0.2), int(height * 0.2)))
                win.fill(CZARNY)
                win.blit(z1, ((width - z1.get_width()) // 2, (height - z1.get_height()) // 2))
                pygame.display.flip()
            elif event.key == pygame.K_2:
                z2 = pygame.transform.rotate(cat, -45)
                win.fill(CZARNY)
                win.blit(z2, ((width - z2.get_width()) // 2, (height - z2.get_height()) // 2))
                pygame.display.flip()
            elif event.key == pygame.K_3:
                z3 = pygame.transform.flip(cat, 0, 1)
                win.fill(CZARNY)
                win.blit(z3, ((width - z3.get_width()) // 2, (height - z3.get_height()) // 2))
                pygame.display.flip()
            elif event.key == pygame.K_4:
                z4a = pygame.transform.scale(cat, (int(width * 0.7), height))
                z4b = pygame.transform.rotozoom(z4a, 45, 0.7)
                win.fill(CZARNY)
                win.blit(z4b, ((width - z4b.get_width()) // 2, (height - z4b.get_height()) // 2))
                pygame.display.flip()
            elif event.key == pygame.K_5:
                z5 = pygame.transform.scale(cat, (int(width * 0.9), int(height * 0.35)))
                win.fill(CZARNY)
                win.blit(z5, ((width - z5.get_width()) // 2, 1))
                pygame.display.flip()
            elif event.key == pygame.K_6:
                z6a = pygame.transform.scale(cat, (int(width * 0.35), height))
                z6b = pygame.transform.rotozoom(cat, 270, 1)
                win.fill(CZARNY)
                win.blit(z6b, ((width - z6b.get_width()) // 2, (height - z6b.get_height()) // 2))
                pygame.display.flip()
            elif event.key == pygame.K_7:
                z7a = pygame.transform.scale(cat, (int(cat.get_width() * 0.7), cat.get_height()))
                z7b = pygame.transform.rotate(z7a, 180)
                win.fill(CZARNY)
                win.blit(z7b, ((width - z7b.get_width()) // 2, (height - z7b.get_height()) // 2))
                pygame.display.flip()
            elif event.key == pygame.K_8:
                z8a = pygame.transform.scale(cat, (int(cat.get_width() * 1.2), int(cat.get_height() * 0.7)))
                z8b = pygame.transform.rotate(z8a, -20)
                win.fill(CZARNY)
                win.blit(z8b, ((width - z8a.get_width()) // 2, ((height - z8a.get_height()) // 2) + 100 ))
                pygame.display.flip()
            elif event.key == pygame.K_9:
                z9 = pygame.transform.rotozoom(cat, 180, 1)
                win.fill(CZARNY)
                win.blit(z9, ((width - z9.get_width() ), (height - z9.get_height()) // 2))
                pygame.display.flip()

    pygame.display.update()