#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <cmath>
#include <ctime>
#include <algorithm>

using namespace std;

struct Bullet {
    SDL_Rect rect;
    float velX, velY;
    bool active;
    int tankID;
};

const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;
const int FIRE_RATE = 200;
const int MOVE_SPEED = 1;
const int TANK_INVULNERABLE_TIME = 1000;
const int TANK_RESPAWN_DELAY = 2000;

const int MAZE_HEIGHT = 10;
const int MAZE_WIDTH = 10;
const int WALL_WIDTH = SCREEN_WIDTH / MAZE_WIDTH;
const int WALL_HEIGHT = SCREEN_HEIGHT / MAZE_HEIGHT;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Rect dstTank1, dstTank2;
SDL_Texture* background = NULL;
SDL_Texture* tank1 = NULL;
SDL_Texture* tank2 = NULL;
SDL_Texture* bulletTex = NULL;
SDL_Texture* hWall = NULL;
SDL_Texture* vWall = NULL;

bool up1 = false;
bool down1 = false;
bool left1 = false;
bool right1 = false;
bool shoot1 = false;
bool hit1 = false;

bool up2 = false;
bool down2 = false;
bool left2 = false;
bool right2 = false;
bool shoot2 = false;
bool hit2 = false;

bool replay = false;

int timer1 = 0, timer2 = 0;
int invulnerable1 = 0, invulnerable2 = 0;
int point1 = 0, point2 = 0;

vector<Bullet> bullets;
vector<vector<int>> maze(MAZE_HEIGHT, vector<int>(MAZE_WIDTH, 0)); // Initialize maze with all walls

bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return SDL_HasIntersection(&a, &b);
}

void generateMaze() {
    maze = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 0, 1, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 1, 1, 0, 1, 0, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 1, 0, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
}

bool checkMazeCollision(const SDL_Rect& rect) {
    int left = rect.x / WALL_WIDTH;
    int right = (rect.x + rect.w) / WALL_WIDTH;
    int top = rect.y / WALL_HEIGHT;
    int bottom = (rect.y + rect.h) / WALL_HEIGHT;

    if (left < 0 || right >= MAZE_WIDTH || top < 0 || bottom >= MAZE_HEIGHT)
        return true;

    for (int i = top; i <= bottom; ++i) {
        for (int j = left; j <= right; ++j) {
            if (maze[i][j] == 1)
                return true;
        }
    }

    return false;
}

void renderMaze() {
    SDL_Rect wallRect; // Declare wallRect outside the loop
    for (int y = 0; y < MAZE_HEIGHT; ++y) {
        for (int x = 0; x < MAZE_WIDTH; ++x) {
            if (maze[y][x] == 1) {
                wallRect.x = x * WALL_WIDTH;
                wallRect.y = y * WALL_HEIGHT;
                wallRect.w = WALL_WIDTH;
                wallRect.h = WALL_HEIGHT;

                bool leftWall = (x > 0 && maze[y][x - 1] == 1);
                bool rightWall = (x < MAZE_WIDTH - 1 && maze[y][x + 1] == 1);
                bool topWall = (y > 0 && maze[y - 1][x] == 1);
                bool bottomWall = (y < MAZE_HEIGHT - 1 && maze[y + 1][x] == 1);

                if (leftWall && rightWall && topWall && bottomWall) {
                    // Intersection
                    SDL_RenderCopy(renderer, hWall, NULL, &wallRect);
                } else if (leftWall && rightWall) {
                    // Horizontal wall
                    SDL_RenderCopy(renderer, hWall, NULL, &wallRect);
                } else if (topWall && bottomWall) {
                    // Vertical wall
                    SDL_RenderCopy(renderer, vWall, NULL, &wallRect);
                } else {
                    // Corner or single wall segment
                    SDL_RenderCopy(renderer, hWall, NULL, &wallRect);
                }
            }
        }
    }
}

void shootBullet(int tankX, int tankY, float angle, int& timer, int tankID) {
    if (timer >= FIRE_RATE) {
        Bullet bullet;
        bullet.rect.x = tankX + 20;
        bullet.rect.y = tankY + 20;
        bullet.rect.w = bullet.rect.h = 10;
        float radians = (angle - 90) * M_PI / 180;
        bullet.velX = cos(radians) * 5;
        bullet.velY = sin(radians) * 5;
        bullet.active = true;
        bullet.tankID = tankID;
        bullets.push_back(bullet);
        timer = 0;
    }
}

void bulletCollision() {
    for (int i = 0; i < bullets.size(); i++) {
        if (bullets[i].active) {
            if (bullets[i].tankID == 1) {
                if (checkCollision(bullets[i].rect, dstTank2)) {
                    hit2 = true;
                    bullets[i].active = false;
                    point1++;
                    cout << "Tank 1:" << point1 << endl;
                    dstTank1.x = 0;
                    dstTank1.y = SCREEN_HEIGHT / 2;
                    dstTank2.x = SCREEN_WIDTH;
                    dstTank2.y = SCREEN_HEIGHT / 2;
                    if (point2 >= 5) {
                        cout << "Tank 1 wins!" << endl;
                    }
                }
            } else if (bullets[i].tankID == 2) {
                if (checkCollision(bullets[i].rect, dstTank1)) {
                    hit1 = true;
                    bullets[i].active = false;
                    point2++;
                    cout << "Tank 2:" << point2 << endl;
                    dstTank1.x = 0;
                    dstTank1.y = SCREEN_HEIGHT / 2;
                    dstTank2.x = SCREEN_WIDTH;
                    dstTank2.y = SCREEN_HEIGHT / 2;
                    if (point1 >= 5) {
                        cout << "Tank 2 wins! " << endl;
                    }
                }
            }
        }
    }
}

void renderBullet() {
    for (int i = 0; i < bullets.size(); i++) {
        if(bullets[i].active){
            SDL_RenderCopy(renderer, bulletTex, NULL, &bullets[i].rect);
        }
    }
}

void updateBullet() {
    for (int i = 0; i < bullets.size(); i++) {
        if (bullets[i].active) {
            bullets[i].rect.x += bullets[i].velX;
            bullets[i].rect.y += bullets[i].velY;

            if (bullets[i].rect.x < 0 || bullets[i].rect.x > SCREEN_WIDTH || bullets[i].rect.y < 0 || bullets[i].rect.y > SCREEN_HEIGHT) {
                bullets[i].active = false;
            }

            if (checkMazeCollision(bullets[i].rect)) {
                bullets[i].active = false;
            }
        }
    }

    bullets.erase(remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }), bullets.end());
}

void keepInWindow(SDL_Rect& obj1, SDL_Rect& obj2, int windowWidth, int windowHeight) {
    if (obj1.x < 0) {
        obj1.x = 0;
    }
    if (obj1.x + obj1.w > windowWidth) {
        obj1.x = windowWidth - obj1.w;
    }
    if (obj1.y < 0) {
        obj1.y = 0;
    }
    if (obj1.y + obj1.h > windowHeight) {
        obj1.y = windowHeight - obj1.h;
    }

    if (obj2.x < 0) {
        obj2.x = 0;
    }
    if (obj2.x + obj2.w > windowWidth) {
        obj2.x = windowWidth - obj2.w;
    }
    if (obj2.y < 0) {
        obj2.y = 0;
    }
    if (obj2.y + obj2.h > windowHeight) {
        obj2.y = windowHeight - obj2.h;
    }
}

void collision(SDL_Rect& obj1, SDL_Rect& obj2) {
    bool colliding = true;
    while (colliding) {
        colliding = false;
        int deltaX = (obj1.x + obj1.w / 2) - (obj2.x + obj2.w / 2);
        int deltaY = (obj1.y + obj1.h / 2) - (obj2.y + obj2.h / 2);

        if (abs(deltaX) > abs(deltaY)) {
            if (deltaX > 0) {
                obj1.x += 1;
            } else {
                obj1.x -= 1;
            }
            if (checkCollision(obj1, obj2)) {
                colliding = true;
            }
        } else {
            if (deltaY > 0) {
                obj1.y += 1;
            } else {
                obj1.y -= 1;
            }
            if (checkCollision(obj1, obj2)) {
                colliding = true;
            }
        }
    }
}

bool handleInput(SDL_Event ev)
{
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
            case SDL_QUIT:
                return false;
                break;
            case SDL_KEYDOWN:
                if (ev.key.keysym.sym == SDLK_ESCAPE) {
                    return false;
                } else if (ev.key.keysym.sym == SDLK_UP && invulnerable1 == 0) {
                    up2 = true;
                } else if (ev.key.keysym.sym == SDLK_DOWN && invulnerable1 == 0) {
                    down2 = true;
                } else if (ev.key.keysym.sym == SDLK_LEFT && invulnerable1 == 0) {
                    left2 = true;
                } else if (ev.key.keysym.sym == SDLK_RIGHT && invulnerable1 == 0) {
                    right2 = true;
                } else if (ev.key.keysym.sym == SDLK_w && invulnerable2 == 0) {
                    up1 = true;
                } else if (ev.key.keysym.sym == SDLK_s && invulnerable2 == 0) {
                    down1 = true;
                } else if (ev.key.keysym.sym == SDLK_a && invulnerable2 == 0) {
                    left1 = true;
                } else if (ev.key.keysym.sym == SDLK_d && invulnerable2 == 0) {
                    right1 = true;
                } else if (ev.key.keysym.sym == SDLK_KP_1 && invulnerable1 == 0) {
                    shoot2 = true;
                } else if (ev.key.keysym.sym == SDLK_SPACE && invulnerable2 == 0) {
                    shoot1 = true;
                }
                break;
            case SDL_KEYUP:
                if (ev.key.keysym.sym == SDLK_UP) {
                    up2 = false;
                } else if (ev.key.keysym.sym == SDLK_DOWN) {
                    down2 = false;
                } else if (ev.key.keysym.sym == SDLK_LEFT) {
                    left2 = false;
                } else if (ev.key.keysym.sym == SDLK_RIGHT) {
                    right2 = false;
                } else if (ev.key.keysym.sym == SDLK_w) {
                    up1 = false;
                } else if (ev.key.keysym.sym == SDLK_s) {
                    down1 = false;
                } else if (ev.key.keysym.sym == SDLK_a) {
                    left1= false;
                } else if (ev.key.keysym.sym == SDLK_d) {
                    right1 = false;
                } else if (ev.key.keysym.sym == SDLK_KP_1) {
                    shoot2 = false;
                } else if (ev.key.keysym.sym == SDLK_SPACE) {
                    shoot1 = false;
                }
                break;
        }
    }

    return true;
}

void displayWinScreen(SDL_Renderer* renderer, const std::string& winner, const std::string& replayOption, const std::string& exitOption) {
    // Load font
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("font/Peepo.ttf", 24);

    // Set up colors
    SDL_Color white = { 255, 255, 255 };

    // Render text
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, winner.c_str(), white);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = { SCREEN_WIDTH / 2 - textSurface->w / 2, SCREEN_HEIGHT / 2 - textSurface->h / 2, textSurface->w, textSurface->h };

    SDL_FreeSurface(textSurface);

    // Render text to screen
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        // Render replay and exit buttons
    SDL_Rect replayRect = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50, 200, 50 };
    SDL_Rect exitRect = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 120, 200, 50 };

    // Load font for buttons
    TTF_Font* buttonFont = TTF_OpenFont("font/Peepo.ttf", 18);

    // Render replay button
    SDL_Surface* replaySurface = TTF_RenderText_Solid(buttonFont, "Replay (incomplete)", white);
    SDL_Texture* replayTexture = SDL_CreateTextureFromSurface(renderer, replaySurface);
    SDL_RenderCopy(renderer, replayTexture, NULL, &replayRect);

    // Render exit button
    SDL_Surface* exitSurface = TTF_RenderText_Solid(buttonFont, "Exit (hit ESC)", white);
    SDL_Texture* exitTexture = SDL_CreateTextureFromSurface(renderer, exitSurface);
    SDL_RenderCopy(renderer, exitTexture, NULL, &exitRect);

    SDL_RenderPresent(renderer);

    bool running = true;
    SDL_Event evt;
    while (running) {
        running = handleInput(evt);
    }

    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(replayTexture);
    SDL_DestroyTexture(exitTexture);
    TTF_CloseFont(font);
    TTF_CloseFont(buttonFont);
    TTF_Quit();
}

void renderScores(SDL_Renderer* renderer, TTF_Font* font, int score1, int score2) {
    // Set up colors
    SDL_Color white = { 255, 255, 255 };

    // Rendered text surfaces
    SDL_Surface* scoreSurface1 = TTF_RenderText_Solid(font, ("Player 1: " + std::to_string(score1)).c_str(), white);
    SDL_Surface* scoreSurface2 = TTF_RenderText_Solid(font, ("Player 2: " + std::to_string(score2)).c_str(), white);

    // Score textures
    SDL_Texture* scoreTexture1 = SDL_CreateTextureFromSurface(renderer, scoreSurface1);
    SDL_Texture* scoreTexture2 = SDL_CreateTextureFromSurface(renderer, scoreSurface2);

    // Score positions
    SDL_Rect scoreRect1 = { 10, 10, scoreSurface1->w, scoreSurface1->h };
    SDL_Rect scoreRect2 = { SCREEN_WIDTH - scoreSurface2->w - 10, 10, scoreSurface2->w, scoreSurface2->h };

    // Render score textures
    SDL_RenderCopy(renderer, scoreTexture1, NULL, &scoreRect1);
    SDL_RenderCopy(renderer, scoreTexture2, NULL, &scoreRect2);

    // Clean up
    SDL_FreeSurface(scoreSurface1);
    SDL_FreeSurface(scoreSurface2);
    SDL_DestroyTexture(scoreTexture1);
    SDL_DestroyTexture(scoreTexture2);
}



int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer);
    SDL_SetWindowTitle(window, "Tank Trouble");

    TTF_Init();
    TTF_Font* font = TTF_OpenFont("font/Peepo.ttf", 18);

    generateMaze();

    background = IMG_LoadTexture(renderer, "Img/background.png");
    tank1 = IMG_LoadTexture(renderer, "Img/tank1.png");
    tank2 = IMG_LoadTexture(renderer, "Img/tank2.png");
    bulletTex = IMG_LoadTexture(renderer, "Img/bullet.png");
    hWall = IMG_LoadTexture(renderer, "img/hWall.png");
    vWall = IMG_LoadTexture(renderer, "img/vWall.png");

    dstTank1.x = 0;
    dstTank1.y = SCREEN_HEIGHT / 2;
    dstTank1.w = dstTank1.h = 64;

    dstTank2.x = SCREEN_WIDTH;
    dstTank2.y = SCREEN_HEIGHT / 2;
    dstTank2.w = dstTank2.h = 64;

    int tank1Angle = 0, tank2Angle = 0;
    int tank1AngleRadian = tank1Angle * M_PI/180;
    int tank2AngleRadian = tank2Angle * M_PI/180;
    int past = SDL_GetTicks();

    SDL_Event ev;

    while (true) {
        if (!handleInput(ev)) {
            break;
        }

        int now = SDL_GetTicks();
        if (now - past >= 10) {
            if (up1) {
                dstTank1.y -= MOVE_SPEED;
                tank1Angle = 0;
                if (checkMazeCollision(dstTank1)) dstTank1.y += MOVE_SPEED; // Revert if collision
            } else if (down1) {
                dstTank1.y += MOVE_SPEED;
                tank1Angle = 180;
                if (checkMazeCollision(dstTank1)) dstTank1.y -= MOVE_SPEED; // Revert if collision
            } else if (left1) {
                dstTank1.x -= MOVE_SPEED;
                tank1Angle = 270;
                if (checkMazeCollision(dstTank1)) dstTank1.x += MOVE_SPEED; // Revert if collision
            } else if (right1) {
                dstTank1.x += MOVE_SPEED;
                tank1Angle = 90;
                if (checkMazeCollision(dstTank1)) dstTank1.x -= MOVE_SPEED; // Revert if collision
            }
            if (up2) {
                dstTank2.y -= MOVE_SPEED;
                tank2Angle = 0;
                if (checkMazeCollision(dstTank2)) dstTank2.y += MOVE_SPEED; // Revert if collision
            } else if (down2) {
                dstTank2.y += MOVE_SPEED;
                tank2Angle = 180;
                if (checkMazeCollision(dstTank2)) dstTank2.y -= MOVE_SPEED; // Revert if collision
            } else if (left2) {
                dstTank2.x -= MOVE_SPEED;
                tank2Angle = 270;
                if (checkMazeCollision(dstTank2)) dstTank2.x += MOVE_SPEED; // Revert if collision
            } else if (right2) {
                dstTank2.x += MOVE_SPEED;
                tank2Angle = 90;
                if (checkMazeCollision(dstTank2)) dstTank2.x -= MOVE_SPEED; // Revert if collision
            }

            keepInWindow(dstTank1, dstTank2, SCREEN_WIDTH, SCREEN_HEIGHT);
            if (dstTank1.x < dstTank2.x + dstTank2.w &&
                dstTank1.x + dstTank1.w > dstTank2.x &&
                dstTank1.y < dstTank2.y + dstTank2.h &&
                dstTank1.y + dstTank1.h > dstTank2.y) {
                collision(dstTank1, dstTank2);
            }

            if (shoot1) {
                shootBullet(dstTank1.x, dstTank1.y, tank1Angle, timer1, 1);
            }
            if (shoot2) {
                shootBullet(dstTank2.x, dstTank2.y, tank2Angle, timer2, 2);
            }

            if (point1 >= 5) {
                displayWinScreen(renderer, "Player 1 Wins!", "Replay", "Exit");
                break;
            } else if (point2 >= 5) {
                displayWinScreen(renderer, "Player 2 Wins!", "Replay", "Exit");
                break;
            }

            updateBullet();
            bulletCollision();
            past = now;
            timer1 += 10;
            timer2 += 10;
        }

        SDL_RenderCopy(renderer, background, NULL, NULL);
        renderMaze();
        SDL_RenderCopyEx(renderer, tank1, NULL, &dstTank1, tank1Angle, NULL, SDL_FLIP_NONE);
        SDL_RenderCopyEx(renderer, tank2, NULL, &dstTank2, tank2Angle, NULL, SDL_FLIP_NONE);
        renderScores(renderer, font, point1, point2);
        renderBullet();

        SDL_RenderPresent(renderer);

        SDL_Delay(10);

    }
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(tank1);
    SDL_DestroyTexture(tank2);
    SDL_DestroyTexture(bulletTex);
    SDL_DestroyTexture(hWall);
    SDL_DestroyTexture(vWall);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}
