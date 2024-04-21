#include <SFML/Graphics.hpp>
#include <iostream>
#include <array>
#include <cmath>
#include <set>

// -- объ€влени€ констант --
constexpr unsigned ANTIALIASING_LEVEL = 8;
constexpr unsigned WINDOW_WIDTH = 900;
constexpr unsigned WINDOW_HEIGHT = 750;
constexpr unsigned MAX_FPS = 60;
constexpr unsigned MAP_SPRITES_WIDTH = 8;
constexpr unsigned MAP_SPRITES_COUNT = 36;
constexpr unsigned MAP_SPRITE_SIZE = 24;
constexpr unsigned MAP_SPRITE_SIZE_HALF = MAP_SPRITE_SIZE / 2;
constexpr unsigned GAME_MAP_WIDTH = 28;
constexpr unsigned GAME_MAP_HEIGHT = 31;
constexpr unsigned HERO_SPRITES_COUNT = 6;
constexpr unsigned HERO_SPRITE_WIDTH = 24;
constexpr unsigned HERO_SPRITE_HEIGHT = 42;
constexpr unsigned ENEMY_SPRITES_COUNT = 2;
constexpr unsigned ENEMY_SPRITE_HEIGHT = 42;
constexpr unsigned ENEMY_SPRITE_WIDTH = 42;
constexpr unsigned DIGITS_SPRITES_COUNT = 10;
constexpr unsigned DIGITS_SPRITE_SIZE = 20;
constexpr unsigned TEXT_SPRITES_COUNT = 9;
constexpr unsigned TEXT_SPRITE_HEIGHT = 20;
constexpr std::array<int, TEXT_SPRITES_COUNT> TEXT_SPRITE_WIDTH{82, 102, 62, 120, 80, 80, 102, 102, 20};
constexpr unsigned HERO_SPEED_INITIAL = 100.f;
constexpr unsigned ENEMY_SPEED_INITIAL = 100.f;
constexpr unsigned LIMIT_PIXELS_TO_TURN = 3;
constexpr unsigned COIN_ITEM_COST = 10;
constexpr unsigned STAR_ITEM_COST = 50;

static const sf::Vector2f HERO_INITIAL_POSITION = {MAP_SPRITE_SIZE * 14, MAP_SPRITE_SIZE * 17 + MAP_SPRITE_SIZE_HALF};

typedef std::array<std::array<int, GAME_MAP_WIDTH>, GAME_MAP_HEIGHT> GameMap;

struct Sprite
{
    sf::Texture t;
    sf::Sprite s;
};

enum struct Direction
{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct Root
{
    int unsigned highScore;
};

struct Hero
{
    sf::Vector2f position;
    Direction direction;
    Direction directionDesired;
    bool catched;
    float timeIteration;
    int curSpriteNum;
    float curSpeed;
};

struct Enemy
{
    sf::Vector2f position;
    Direction direction;
    float timeIteration;
    int curSpriteNum;
    float curSpeed;
};

struct Game
{
    int unsigned lives;
    int unsigned coins;
    int unsigned stars;
    int unsigned score;
    int unsigned stage;
};

// -- предварительные объ€влени€ функций (всех, кроме main) --
void createWindow(sf::RenderWindow &);
void initMap(GameMap &);
void initHero(Hero &);
void initEnemy(Enemy &);
void initGame(Game &);
bool initSpritesMap(sf::Image, std::vector<Sprite *> &, int);
bool initSpritesHero(sf::Image, std::vector<Sprite *> &, int);
void handleEvents(sf::RenderWindow &, Hero &);
void renderMap(sf::RenderWindow &, const GameMap &, Hero);
void renderHero(sf::RenderWindow &, std::vector<Sprite *>);
void clearSprites(std::vector<Sprite *> &);
void updatePacman(Hero &, float, const GameMap &);
void calcCollisionsItems(Hero &, Game &game, GameMap &);
void calcCollisionsEnemies(Hero &, Enemy &);

// -- определени€ функций --

float Distance(sf::Vector2f from, sf::Vector2f to)
{
    sf::Vector2f delta = to - from;
    return sqrt(delta.x * delta.x + delta.y * delta.y);
}

Direction getDirectionByChar(char d)
{
    switch (d)
    {
    case 'U':
        return Direction::UP;
    case 'R':
        return Direction::RIGHT;
    case 'D':
        return Direction::DOWN;
    case 'L':
        return Direction::LEFT;
    }
    return Direction::NONE;
}

char getCharByDirection(Direction dir)
{
    switch (dir)
    {
    case Direction::UP:
        return 'U';
    case Direction::RIGHT:
        return 'R';
    case Direction::DOWN:
        return 'D';
    case Direction::LEFT:
        return 'L';
    }
    return 'N';
}

char getFirstDirection(const std::set<char> &dirs)
{
    if (dirs.size())
    {
        for (char item : dirs)
        {
            return item;
        }
    }
    return 'N';
}

char getDirPrimary(const Hero &pacman, const Enemy &enemy)
{
    int deltaX = static_cast<int>(enemy.position.x - pacman.position.x);
    int deltaY = static_cast<int>(enemy.position.y - pacman.position.y);
    if (abs(deltaX) > abs(deltaY))
    {
        return deltaX < 0 ? 'R' : 'L';
    }
    if (abs(deltaX) < abs(deltaY))
    {
        return deltaY < 0 ? 'D' : 'U';
    }
    return 'N';
}

char getDirSecondary(const Hero &pacman, const Enemy &enemy)
{
    int deltaX = static_cast<int>(enemy.position.x - pacman.position.x);
    int deltaY = static_cast<int>(enemy.position.y - pacman.position.y);
    if (abs(deltaX) > abs(deltaY))
    {
        return deltaY < 0 ? 'D' : 'U';
    }
    if (abs(deltaX) < abs(deltaY))
    {
        return deltaX < 0 ? 'R' : 'L';
    }
    return 'N';
}

std::set<char> getPossibleDirections(int posMapX, int posMapY, Direction curDirection, const GameMap &map)
{
    std::set<char> directions;

    // добавл€ем направление, если оно свободно и не противоположно движению
    if (curDirection != Direction::DOWN && map[posMapY - 1][posMapX] <= 2)
    {
        directions.insert('U');
    }
    if (curDirection != Direction::UP && map[posMapY + 1][posMapX] <= 2)
    {
        directions.insert('D');
    }
    if (curDirection != Direction::RIGHT && map[posMapY][posMapX - 1] <= 2)
    {
        directions.insert('L');
    }
    if (curDirection != Direction::LEFT && map[posMapY][posMapX + 1] <= 2)
    {
        directions.insert('R');
    }

    return directions;
}

bool isNextStepPossible(int posMapX, int posMapY, Direction curDirection, const GameMap &map)
{
    switch (curDirection)
    {
    case Direction::UP:
        return map[posMapY - 1][posMapX] <= 2;
    case Direction::RIGHT:
        return map[posMapY][posMapX + 1] <= 2;
    case Direction::DOWN:
        return map[posMapY + 1][posMapX] <= 2;
    case Direction::LEFT:
        return map[posMapY][posMapX - 1] <= 2;
    }
    return true;
}

Direction defineDirection(const Hero &pacman, const Enemy &enemy, std::set<char> &directions)
{
    char dir = getDirPrimary(pacman, enemy);

    if (directions.count(dir))
    {
        // длинна€ ось до игрока
        return getDirectionByChar(dir);
    }
    else
    {
        directions.erase(dir);
        dir = getDirSecondary(pacman, enemy);
        if (directions.count(dir))
        {
            // коротка€ ось до игрока
            return getDirectionByChar(dir);
        }
        else
        {
            dir = getCharByDirection(enemy.direction);
            if (directions.count(dir))
            {
                // текущее направление движени€
                return getDirectionByChar(dir);
            }
            else
            {
                directions.erase(dir);
                getDirectionByChar(getFirstDirection(directions));
            }
        }
    }
}

// ‘ункци€ создаЄт окно приложени€.
void createWindow(sf::RenderWindow &window)
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = ANTIALIASING_LEVEL;
    window.create(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
        "PacMan Game Clone", sf::Style::Default, settings);
    window.setFramerateLimit(MAX_FPS);
}

bool handlePackmanKeyPress(const sf::Event::KeyEvent &event, Hero &packman)
{
    bool handled = true;
    switch (event.code)
    {
    case sf::Keyboard::Up:
        packman.directionDesired = Direction::UP;
        break;
    case sf::Keyboard::Down:
        packman.directionDesired = Direction::DOWN;
        break;
    case sf::Keyboard::Left:
        packman.directionDesired = Direction::LEFT;
        break;
    case sf::Keyboard::Right:
        packman.directionDesired = Direction::RIGHT;
        break;
    default:
        handled = false;
        break;
    }
    return handled;
}

bool handlePackmanKeyRelease(const sf::Event::KeyEvent &event, Hero &packman)
{
    bool handled = true;
    switch (event.code)
    {
    case sf::Keyboard::Up:
        if (packman.directionDesired == Direction::UP)
        {
            packman.directionDesired = Direction::NONE;
        }
        break;
    case sf::Keyboard::Down:
        if (packman.directionDesired == Direction::DOWN)
        {
            packman.directionDesired = Direction::NONE;
        }
        break;
    case sf::Keyboard::Left:
        if (packman.directionDesired == Direction::LEFT)
        {
            packman.directionDesired = Direction::NONE;
        }
        break;
    case sf::Keyboard::Right:
        if (packman.directionDesired == Direction::RIGHT)
        {
            packman.directionDesired = Direction::NONE;
        }
        break;
    default:
        handled = false;
        break;
    }

    return handled;
}

bool initSpritesMap(sf::Image sprites_file, std::vector<Sprite *> &sprites, int sprites_count)
{
    Sprite *sprite;

    for (int i = 0; i < sprites_count; i++)
    {
        int x = i % MAP_SPRITES_WIDTH * MAP_SPRITE_SIZE;
        int y = i / MAP_SPRITES_WIDTH * MAP_SPRITE_SIZE;
        sprite = new Sprite;
        sprites.push_back(sprite);
        sprites[i]->t.loadFromImage(sprites_file, sf::IntRect(x, y, MAP_SPRITE_SIZE, MAP_SPRITE_SIZE));
        sprites[i]->s.setTexture(sprites[i]->t);
    }

    return true;
}

bool initSpritesHero(sf::Image sprites_file, std::vector<Sprite *> &sprites, int sprites_count)
{
    Sprite *sprite;
    int originX = HERO_SPRITE_WIDTH / 2;
    int originY = HERO_SPRITE_HEIGHT / 2;

    for (int i = 0; i < sprites_count; i++)
    {
        int x = i * HERO_SPRITE_WIDTH;
        sprite = new Sprite;
        sprites.push_back(sprite);
        sprites[i]->t.loadFromImage(sprites_file, sf::IntRect(x, 0, HERO_SPRITE_WIDTH, HERO_SPRITE_HEIGHT));
        sprites[i]->s.setTexture(sprites[i]->t);
        sprites[i]->s.setOrigin(originX, originY);
    }

    return true;
}

bool initSpritesEnemy(sf::Image sprites_file, std::vector<Sprite *> &sprites, int sprites_count)
{
    Sprite *sprite;
    int originX = ENEMY_SPRITE_WIDTH / 2;
    int originY = ENEMY_SPRITE_HEIGHT / 2;

    for (int i = 0; i < sprites_count; i++)
    {
        int x = i * ENEMY_SPRITE_WIDTH;
        sprite = new Sprite;
        sprites.push_back(sprite);
        sprites[i]->t.loadFromImage(sprites_file, sf::IntRect(x, 0, ENEMY_SPRITE_WIDTH, ENEMY_SPRITE_HEIGHT));
        sprites[i]->s.setTexture(sprites[i]->t);
        sprites[i]->s.setOrigin(originX, originY);
    }

    return true;
}

bool initSpritesDigits(sf::Image sprites_file, std::vector<Sprite *> &sprites, int sprites_count)
{
    Sprite *sprite;

    for (int i = 0; i < sprites_count; i++)
    {
        int x = i * DIGITS_SPRITE_SIZE;
        sprite = new Sprite;
        sprites.push_back(sprite);
        sprites[i]->t.loadFromImage(sprites_file, sf::IntRect(x, 0, DIGITS_SPRITE_SIZE, DIGITS_SPRITE_SIZE));
        sprites[i]->s.setTexture(sprites[i]->t);
    }

    return true;
}

bool initSpritesText(sf::Image sprites_file, std::vector<Sprite *> &sprites, int sprites_count)
{
    Sprite *sprite;

    for (int i = 0; i < sprites_count; i++)
    {
        int y = i * TEXT_SPRITE_HEIGHT;
        sprite = new Sprite;
        sprites.push_back(sprite);
        sprites[i]->t.loadFromImage(sprites_file, sf::IntRect(0, y, TEXT_SPRITE_WIDTH[i], TEXT_SPRITE_HEIGHT));
        sprites[i]->s.setTexture(sprites[i]->t);
    }

    return true;
}

// ‘ункци€ обрабатывает все событи€, скопившиес€ в очереди событий SFML.
void handleEvents(sf::RenderWindow &window, Hero &pacman)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            handlePackmanKeyPress(event.key, pacman);
        }
        else if (event.type == sf::Event::KeyReleased)
        {
            handlePackmanKeyRelease(event.key, pacman);
        }
    }
}

// ‘ункци€ рисует карту
void renderMap(sf::RenderWindow &window, const GameMap &map, std::vector<Sprite *> sprites)
{
    for (auto y = 0; y < GAME_MAP_HEIGHT; y++)
    {
        for (auto x = 0; x < GAME_MAP_WIDTH; x++)
        {
            sprites[map[y][x]]->s.setPosition(sf::Vector2f(x * MAP_SPRITE_SIZE, y * MAP_SPRITE_SIZE));
            window.draw(sprites[map[y][x]]->s);
        }
    }
}

// ‘ункци€ рисует геро€
void renderHero(sf::RenderWindow &window, std::vector<Sprite *> sprites, Hero hero)
{
    Sprite *sprite;

    switch (hero.direction)
    {
    case Direction::LEFT:
        sprite = sprites[hero.curSpriteNum];
        sprite->s.setScale(-1, 1);
        break;
    case Direction::RIGHT:
        sprite = sprites[hero.curSpriteNum];
        sprite->s.setScale(1, 1);
        break;
    case Direction::UP:
        sprite = sprites[3];
        sprite->s.setScale(hero.curSpriteNum ? 1 : -1, 1);
        break;
    case Direction::DOWN:
        sprite = sprites[2];
        sprite->s.setScale(hero.curSpriteNum ? 1 : -1, 1);
        break;
    default:
        sprite = sprites[4];
        break;
    }

    sprite->s.setPosition(hero.position);
    window.draw(sprite->s);

    sprite = NULL;
    delete sprite;
}

void renderHeroLives(sf::RenderWindow &window, std::vector<Sprite *> sprites, sf::Vector2f position)
{
    Sprite *sprite;

    sprite = sprites[0];
    sprite->s.setScale(1, 1);

    sprite->s.setPosition(position);
    window.draw(sprite->s);

    sprite = NULL;
    delete sprite;
}

void renderEnemy(sf::RenderWindow &window, std::vector<Sprite *> sprites, Enemy enemy)
{
    Sprite *sprite;

    sprite = sprites[enemy.curSpriteNum];

    sprite->s.setPosition(enemy.position);
    window.draw(sprite->s);

    sprite = NULL;
    delete sprite;
}

void renderDigit(sf::RenderWindow &window, std::vector<Sprite *> sprites, sf::Vector2f position, int digit)
{
    Sprite *sprite;

    sprite = sprites[digit];

    sprite->s.setPosition(position);
    window.draw(sprite->s);

    sprite = NULL;
    delete sprite;
}

void renderText(sf::RenderWindow &window, std::vector<Sprite *> sprites, sf::Vector2f position, int itemNumber)
{
    Sprite *sprite;

    sprite = sprites[itemNumber];

    sprite->s.setPosition(position);
    window.draw(sprite->s);

    sprite = NULL;
    delete sprite;
}

void printNumber(sf::RenderWindow &window, std::vector<Sprite *> sprites, sf::Vector2f position, int unsigned value)
{
    int unsigned i = 0;
    do
    {
        int unsigned digit = value % 10;
        value = value / 10;
        renderDigit(window, sprites, sf::Vector2f(position.x - static_cast<float>(i * DIGITS_SPRITE_SIZE), position.y), digit);
        i++;
    } while (value != 0);
}

void renderInfo(sf::RenderWindow &window, Root &root, Game &game, std::vector<Sprite *> sprites_text, std::vector<Sprite *> sprites_digits, std::vector<Sprite *> sprites_hero)
{
    renderText(window, sprites_text, sf::Vector2f(690.0, 24.0), 0);
    renderText(window, sprites_text, sf::Vector2f(790.0, 24.0), 1);
    printNumber(window, sprites_digits, sf::Vector2f(870.0, 48.0), root.highScore);
    renderText(window, sprites_text, sf::Vector2f(690.0, 96.0), 2);
    printNumber(window, sprites_digits, sf::Vector2f(870.0, 120.0), game.score);
    renderText(window, sprites_text, sf::Vector2f(690.0, 168.0), 6);
    printNumber(window, sprites_digits, sf::Vector2f(870.0, 192.0), game.coins);
    if (game.lives < 4)
    {
        for (int i = 0; i < game.lives; i++)
        {
            renderHeroLives(window, sprites_hero, sf::Vector2f(690.0 + HERO_SPRITE_WIDTH / 2 + 36.0 * i, 600.0));
        }
    }
    else
    {
        renderHeroLives(window, sprites_hero, sf::Vector2f(690.0 + HERO_SPRITE_WIDTH / 2, 600.0));
        renderText(window, sprites_text, sf::Vector2f(734.0, 590.0), 8);
        printNumber(window, sprites_digits, sf::Vector2f(774.0, 590.0), game.lives);
    }
}

void initRoot(Root &root)
{
    root.highScore = 10000;
}

void initGame(Game &game)
{
    game.lives = 2;
    game.stage = 1;
    game.coins = 0;
    game.stars = 0;
    game.score = 0;
}

void initMap(GameMap &map)
{
    map = {{{4, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 16, 17, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 5},
            {8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 28, 26, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10},
            {8, 1, 15, 29, 29, 14, 1, 15, 29, 29, 29, 14, 1, 28, 26, 1, 15, 29, 29, 29, 14, 1, 15, 29, 29, 14, 1, 10},
            {8, 2, 28, 0, 0, 26, 1, 28, 0, 0, 0, 26, 1, 28, 26, 1, 28, 0, 0, 0, 26, 1, 28, 0, 0, 26, 2, 10},
            {8, 1, 13, 27, 27, 12, 1, 13, 27, 27, 27, 12, 1, 13, 12, 1, 13, 27, 27, 27, 12, 1, 13, 27, 27, 12, 1, 10},
            {8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10},
            {8, 1, 15, 29, 29, 14, 1, 15, 14, 1, 15, 29, 29, 29, 29, 29, 29, 14, 1, 15, 14, 1, 15, 29, 29, 14, 1, 10},
            {8, 1, 13, 27, 27, 12, 1, 28, 26, 1, 13, 27, 27, 23, 22, 27, 27, 12, 1, 28, 26, 1, 13, 27, 27, 12, 1, 10},
            {8, 1, 1, 1, 1, 1, 1, 28, 26, 1, 1, 1, 1, 28, 26, 1, 1, 1, 1, 28, 26, 1, 1, 1, 1, 1, 1, 10},
            {6, 11, 11, 11, 11, 14, 1, 28, 25, 29, 29, 14, 0, 28, 26, 0, 15, 29, 29, 24, 26, 1, 15, 11, 11, 11, 11, 7},
            {0, 0, 0, 0, 0, 8, 1, 28, 22, 27, 27, 12, 0, 13, 12, 0, 13, 27, 27, 23, 26, 1, 10, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 8, 1, 28, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 26, 1, 10, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 8, 1, 28, 26, 0, 30, 11, 34, 3, 3, 35, 11, 31, 0, 28, 26, 1, 10, 0, 0, 0, 0, 0},
            {9, 9, 9, 9, 9, 12, 1, 13, 12, 0, 10, 0, 0, 0, 0, 0, 0, 8, 0, 13, 12, 1, 13, 9, 9, 9, 9, 9},
            {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
            {11, 11, 11, 11, 11, 14, 1, 15, 14, 0, 10, 0, 0, 0, 0, 0, 0, 8, 0, 15, 14, 1, 15, 11, 11, 11, 11, 11},
            {0, 0, 0, 0, 0, 8, 1, 28, 26, 0, 32, 9, 9, 9, 9, 9, 9, 33, 0, 28, 26, 1, 10, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 8, 1, 28, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 26, 1, 10, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 8, 1, 28, 26, 0, 15, 29, 29, 29, 29, 29, 29, 14, 0, 28, 26, 1, 10, 0, 0, 0, 0, 0},
            {4, 9, 9, 9, 9, 12, 1, 13, 12, 0, 13, 27, 27, 23, 22, 27, 27, 12, 0, 13, 12, 1, 13, 9, 9, 9, 9, 5},
            {8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 28, 26, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10},
            {8, 1, 15, 29, 29, 14, 1, 15, 29, 29, 29, 14, 1, 28, 26, 1, 15, 29, 29, 29, 14, 1, 15, 29, 29, 14, 1, 10},
            {8, 1, 13, 27, 23, 26, 1, 13, 27, 27, 27, 12, 1, 13, 12, 1, 13, 27, 27, 27, 12, 1, 28, 22, 27, 12, 1, 10},
            {8, 2, 1, 1, 28, 26, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 28, 26, 1, 1, 2, 10},
            {18, 29, 14, 1, 28, 26, 1, 15, 14, 1, 15, 29, 29, 29, 29, 29, 29, 14, 1, 15, 14, 1, 28, 26, 1, 15, 29, 19},
            {20, 27, 12, 1, 13, 12, 1, 28, 26, 1, 13, 27, 27, 23, 22, 27, 27, 12, 1, 28, 26, 1, 13, 12, 1, 13, 27, 21},
            {8, 1, 1, 1, 1, 1, 1, 28, 26, 1, 1, 1, 1, 28, 26, 1, 1, 1, 1, 28, 26, 1, 1, 1, 1, 1, 1, 10},
            {8, 1, 15, 29, 29, 29, 29, 24, 25, 29, 29, 14, 1, 28, 26, 1, 15, 29, 29, 24, 25, 29, 29, 29, 29, 14, 1, 10},
            {8, 1, 13, 27, 27, 27, 27, 27, 27, 27, 27, 12, 1, 13, 12, 1, 13, 27, 27, 27, 27, 27, 27, 27, 27, 12, 1, 10},
            {8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10},
            {6, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 7}}};
}

void initHero(Hero &hero)
{
    hero.position = HERO_INITIAL_POSITION;
    hero.direction = Direction::LEFT;
    hero.directionDesired = Direction::NONE;
    hero.catched = false;
    hero.curSpeed = HERO_SPEED_INITIAL;
    hero.timeIteration = 0.0f;
    hero.curSpriteNum = 0;
}

void initEnemy(Enemy &enemy)
{
    enemy.position = {
        MAP_SPRITE_SIZE * 15,
        MAP_SPRITE_SIZE * 11 + MAP_SPRITE_SIZE_HALF,
    };
    enemy.direction = Direction::LEFT;
    enemy.curSpeed = ENEMY_SPEED_INITIAL;
    enemy.curSpriteNum = 0;
}

void clearSprites(std::vector<Sprite *> &sprites_map)
{
    for (int i = 0; i < sprites_map.size(); i++)
    {
        delete sprites_map[i];
    }

    sprites_map.clear();
}

void updatePacman(Hero &pacman, float elapsedTime, const GameMap &map)
{
    const float step = pacman.curSpeed * elapsedTime; // весь путь на итерацию
    sf::Vector2f position = pacman.position;

    int curMapPositionY = static_cast<int>(position.y) / MAP_SPRITE_SIZE;
    int curMapPositionX = static_cast<int>(position.x) / MAP_SPRITE_SIZE;
    int desMapPositionX = curMapPositionX;
    int desMapPositionY = curMapPositionY;

    if (pacman.directionDesired != pacman.direction && pacman.directionDesired != Direction::NONE)
    {
        int delta;

        switch (pacman.directionDesired)
        {
        case Direction::UP:
        case Direction::DOWN:
            delta = static_cast<int>(position.x) - (curMapPositionX * MAP_SPRITE_SIZE + MAP_SPRITE_SIZE_HALF);
            break;
        case Direction::LEFT:
        case Direction::RIGHT:
            delta = static_cast<int>(position.y) - (curMapPositionY * MAP_SPRITE_SIZE + MAP_SPRITE_SIZE_HALF);
            break;
        }

        switch (pacman.directionDesired)
        {
        case Direction::UP:
            desMapPositionY = curMapPositionY - 1;
            break;
        case Direction::DOWN:
            desMapPositionY = curMapPositionY + 1;
            break;
        case Direction::LEFT:
            desMapPositionX = curMapPositionX - 1;
            break;
        case Direction::RIGHT:
            desMapPositionX = curMapPositionX + 1;
            break;
        }

        if (std::abs(delta) < LIMIT_PIXELS_TO_TURN && map[desMapPositionY][desMapPositionX] <= 2)
        {
            pacman.direction = pacman.directionDesired;

            switch (pacman.directionDesired)
            {
            case Direction::UP:
            case Direction::DOWN:
                position.x = curMapPositionX * MAP_SPRITE_SIZE + MAP_SPRITE_SIZE_HALF;
                break;
            case Direction::LEFT:
            case Direction::RIGHT:
                position.y = curMapPositionY * MAP_SPRITE_SIZE + MAP_SPRITE_SIZE_HALF;
                break;
            }
        }
    }

    int newMapPositionX, newMapPositionY;

    switch (pacman.direction)
    {
    case Direction::UP:
        position.y -= step;
        newMapPositionY = (static_cast<int>(position.y) - MAP_SPRITE_SIZE_HALF) / MAP_SPRITE_SIZE;
        if (newMapPositionY != curMapPositionY && newMapPositionY >= 0 && newMapPositionY <= GAME_MAP_HEIGHT - 1)
        {
            if (map[newMapPositionY][curMapPositionX] > 2)
            {
                position.y = curMapPositionY * MAP_SPRITE_SIZE + MAP_SPRITE_SIZE_HALF;
                pacman.direction = Direction::NONE;
            }
        }
        break;
    case Direction::DOWN:
        position.y += step;
        newMapPositionY = (static_cast<int>(position.y) + MAP_SPRITE_SIZE_HALF) / MAP_SPRITE_SIZE;
        if (newMapPositionY != curMapPositionY && newMapPositionY >= 0 && newMapPositionY <= GAME_MAP_HEIGHT - 1)
        {
            if (map[newMapPositionY][curMapPositionX] > 2)
            {
                position.y = curMapPositionY * MAP_SPRITE_SIZE + MAP_SPRITE_SIZE_HALF;
                pacman.direction = Direction::NONE;
            }
        }
        break;
    case Direction::LEFT:
        position.x -= step;
        newMapPositionX = (static_cast<int>(position.x) - MAP_SPRITE_SIZE_HALF) / MAP_SPRITE_SIZE;
        if (newMapPositionX != curMapPositionX && newMapPositionX >= 0 && newMapPositionX <= GAME_MAP_WIDTH - 1)
        {
            if (map[curMapPositionY][newMapPositionX] > 2)
            {
                position.x = curMapPositionX * MAP_SPRITE_SIZE + MAP_SPRITE_SIZE_HALF;
                pacman.direction = Direction::NONE;
            }
        }
        break;
    case Direction::RIGHT:
        position.x += step;
        newMapPositionX = (static_cast<int>(position.x) + MAP_SPRITE_SIZE_HALF) / MAP_SPRITE_SIZE;
        if (newMapPositionX != curMapPositionX && newMapPositionX >= 0 && newMapPositionX <= GAME_MAP_WIDTH - 1)
        {
            if (map[curMapPositionY][newMapPositionX] > 2)
            {
                position.x = newMapPositionX * MAP_SPRITE_SIZE - MAP_SPRITE_SIZE_HALF;
                pacman.direction = Direction::NONE;
            }
        }
        break;
    case Direction::NONE:
        break;
    }
    pacman.position = position;

    pacman.timeIteration += elapsedTime;
    int stepsNum = static_cast<int>(pacman.timeIteration * pacman.curSpeed / 12.0);
    pacman.curSpriteNum = stepsNum % 2;
}

void updateEnemies(Hero &pacman, float elapsedTime, Enemy &enemy, const GameMap &map)
{
    const float step = enemy.curSpeed * elapsedTime; // весь путь на итерацию
    sf::Vector2f posStart = enemy.position;
    sf::Vector2f posFinish = posStart;

    switch (enemy.direction)
    {
    case Direction::UP:
        posFinish.y -= step;
        break;
    case Direction::DOWN:
        posFinish.y += step;
        break;
    case Direction::LEFT:
        posFinish.x -= step;
        break;
    case Direction::RIGHT:
        posFinish.x += step;
        break;
    }

    int posStartMapY = static_cast<int>(posStart.y) / MAP_SPRITE_SIZE;
    int posStartMapX = static_cast<int>(posStart.x) / MAP_SPRITE_SIZE;

    int spriteCenterX = posStartMapX * MAP_SPRITE_SIZE + MAP_SPRITE_SIZE_HALF;
    int spriteCenterY = posStartMapY * MAP_SPRITE_SIZE + MAP_SPRITE_SIZE_HALF;

    // вы€снение пересечение центра в результате хода
    bool intersected = false;

    // вы€снить, может старт уже в центре
    switch (enemy.direction)
    {
    case Direction::UP:
    case Direction::DOWN:
        intersected = static_cast<int>(posStart.y) == spriteCenterY;
        break;
    case Direction::RIGHT:
    case Direction::LEFT:
        intersected = static_cast<int>(posStart.x) == spriteCenterX;
        break;
    }

    // если не в центре, то может пересекаем в результате хода?
    if (!intersected)
    {
        switch (enemy.direction)
        {
        case Direction::UP:
        case Direction::DOWN:
            intersected = ((spriteCenterY - posStart.y) * (spriteCenterY - posFinish.y)) < 0;
            break;
        case Direction::RIGHT:
        case Direction::LEFT:
            intersected = ((spriteCenterX - posStart.x) * (spriteCenterX - posFinish.x)) < 0;
            break;
        }
    }

    if (intersected)
    {
        // 1.  уда идти дальше (исключаем назад и если впереди стенка - то и вперЄд)
        std::set<char> directions = getPossibleDirections(posStartMapX, posStartMapY, enemy.direction, map);

        // вычисл€ем направление по приоритетам
        Direction newDir = defineDirection(pacman, enemy, directions);

        // 2. ≈сли надо - сменить направление и пересчитать координаты
        if (newDir != enemy.direction)
        {
            // от старта до центра
            float beginStep;
            switch (enemy.direction)
            {
            case Direction::UP:
            case Direction::DOWN:
                beginStep = fabs(static_cast<float>(spriteCenterY) - posStart.y);
                break;
            case Direction::RIGHT:
            case Direction::LEFT:
                beginStep = fabs(static_cast<float>(spriteCenterX) - posStart.x);
                break;
            }

            // от центра до нового направлени€
            float endStep = step - beginStep;
            enemy.position.x = static_cast<float>(spriteCenterX);
            enemy.position.y = static_cast<float>(spriteCenterY);
            switch (newDir)
            {
            case Direction::UP:
                enemy.position.y -= endStep;
                break;
            case Direction::DOWN:
                enemy.position.y += endStep;
                break;
            case Direction::RIGHT:
                enemy.position.x += endStep;
                break;
            case Direction::LEFT:
                enemy.position.x -= endStep;
                break;
            }

            // сменить направление
            enemy.direction = newDir;
        }

        // if (!isNextStepPossible(posStartMapX, posStartMapY, enemy.direction, map))
        // {
        //     switch (enemy.direction)
        //     {
        //     case Direction::LEFT:
        //         enemy.direction = Direction::RIGHT;
        //         break;
        //     case Direction::RIGHT:
        //         enemy.direction = Direction::LEFT;
        //         break;
        //     }
        // }
        else
        {
            enemy.position = posFinish;
        }
    }
    else // 3. ≈сли оставл€ем прежнее направление - всЄ уже посчитано (только внести данные в объект).
    {
        enemy.position = posFinish;
    }

    enemy.timeIteration += elapsedTime;
    int stepsNum = static_cast<int>(enemy.timeIteration * enemy.curSpeed / 12.0);
    enemy.curSpriteNum = stepsNum % 2;
}

void increaseScore(Root &root, Game &game, int addon)
{
    game.score += addon;

    if (game.score > root.highScore)
    {
        root.highScore = game.score;
    }
}

void calcCollisionsItems(Root &root, Game &game, Hero &pacman, GameMap &map)
{
    /*
    найти место на карте, куда наступаем (текуща€ координата игрока + пол размера спрайта)
    если на этом месте спрайт 1(монета) или 2(звезда)
        если от центра спрайта до центра игрока менее 1/3 размера спрайта карты
            фиксируем вз€тие монеты или звезды
            на карту наносим пустое место
    */

    sf::Vector2f positionNext = pacman.position;

    switch (pacman.direction)
    {
    case Direction::UP:
        positionNext.y = pacman.position.y - static_cast<float>(MAP_SPRITE_SIZE_HALF);
        break;
    case Direction::DOWN:
        positionNext.y = pacman.position.y + static_cast<float>(MAP_SPRITE_SIZE_HALF);
        break;
    case Direction::LEFT:
        positionNext.x = pacman.position.x - static_cast<float>(MAP_SPRITE_SIZE_HALF);
        break;
    case Direction::RIGHT:
        positionNext.x = pacman.position.x + static_cast<float>(MAP_SPRITE_SIZE_HALF);
        break;
    }

    int nextMapPositionX = static_cast<int>(positionNext.x) / MAP_SPRITE_SIZE;
    int nextMapPositionY = static_cast<int>(positionNext.y) / MAP_SPRITE_SIZE;
    int nextSpriteValue = map[nextMapPositionY][nextMapPositionX];

    if (nextSpriteValue == 1 || nextSpriteValue == 2)
    {
        sf::Vector2f positionItemCenter = {
            x : static_cast<float>(nextMapPositionX * MAP_SPRITE_SIZE + MAP_SPRITE_SIZE_HALF),
            y : static_cast<float>(nextMapPositionY * MAP_SPRITE_SIZE + MAP_SPRITE_SIZE_HALF),
        };

        float MapSpriteSizeThirdPart = static_cast<float>(MAP_SPRITE_SIZE / 3);

        if (std::fabs(pacman.position.x - positionItemCenter.x) < MapSpriteSizeThirdPart && std::fabs(pacman.position.y - positionItemCenter.y) < MapSpriteSizeThirdPart)
        {
            if (nextSpriteValue == 1)
            {
                game.coins++;
                increaseScore(root, game, COIN_ITEM_COST);
            }
            else
            {
                game.stars++;
                increaseScore(root, game, STAR_ITEM_COST);
            }
            map[nextMapPositionY][nextMapPositionX] = 0;
        }
    }
}

void calcCollisionsEnemies(Hero &pacman, Enemy &enemy)
{
    float distance = Distance(pacman.position, enemy.position);
    if (static_cast<int>(distance) < 24)
    {
        pacman.catched = true;
    }
}

void update(sf::Clock &clock, Root &root, Game &game, Hero &pacman, Enemy &enemy, GameMap &map)
{
    const float elapsedTime = clock.getElapsedTime().asSeconds();
    clock.restart();
    if (!pacman.catched)
    {
        updatePacman(pacman, elapsedTime, map);
        updateEnemies(pacman, elapsedTime, enemy, map);
    }
    calcCollisionsItems(root, game, pacman, map);
    calcCollisionsEnemies(pacman, enemy);
}

int main(int, char *[])
{
    Root root;
    initRoot(root);

    sf::Image sprites_map_file;
    sf::Image sprites_hero_file;
    sf::Image sprites_enemy_file;
    sf::Image sprites_digits_file;
    sf::Image sprites_text_file;

    if (!sprites_map_file.loadFromFile("assets/sprites_map.png"))
    {
        std::cout << "Error loading map sprites!" << std::endl;
        return 1;
    }

    if (!sprites_hero_file.loadFromFile("assets/sprites_hero.png"))
    {
        std::cout << "Error loading hero sprites!" << std::endl;
        return 1;
    }

    if (!sprites_enemy_file.loadFromFile("assets/sprites_goomba.png"))
    {
        std::cout << "Error loading enemy sprites!" << std::endl;
        return 1;
    }

    if (!sprites_digits_file.loadFromFile("assets/sprites_digits.png"))
    {
        std::cout << "Error loading digits sprites!" << std::endl;
        return 1;
    }

    if (!sprites_text_file.loadFromFile("assets/sprites_text.png"))
    {
        std::cout << "Error loading text sprites!" << std::endl;
        return 1;
    }

    std::vector<Sprite *> sprites_map;
    initSpritesMap(sprites_map_file, sprites_map, MAP_SPRITES_COUNT);

    std::vector<Sprite *> sprites_hero;
    initSpritesHero(sprites_hero_file, sprites_hero, HERO_SPRITES_COUNT);

    std::vector<Sprite *> sprites_enemy;
    initSpritesEnemy(sprites_enemy_file, sprites_enemy, ENEMY_SPRITES_COUNT);

    std::vector<Sprite *> sprites_digits;
    initSpritesDigits(sprites_digits_file, sprites_digits, DIGITS_SPRITES_COUNT);

    std::vector<Sprite *> sprites_text;
    initSpritesText(sprites_text_file, sprites_text, TEXT_SPRITES_COUNT);

    Game game;
    initGame(game);

    GameMap map;
    initMap(map);

    Hero hero;
    initHero(hero);

    Enemy enemy;
    initEnemy(enemy);

    sf::RenderWindow window;
    createWindow(window);

    sf::Clock clock;
    while (window.isOpen())
    {
        handleEvents(window, hero);
        update(clock, root, game, hero, enemy, map);
        window.clear();

        renderMap(window, map, sprites_map);
        renderHero(window, sprites_hero, hero);
        renderEnemy(window, sprites_enemy, enemy);
        renderInfo(window, root, game, sprites_text, sprites_digits, sprites_hero);

        window.display();
    }

    clearSprites(sprites_map);
    clearSprites(sprites_hero);
    clearSprites(sprites_enemy);
    clearSprites(sprites_digits);
    clearSprites(sprites_text);

    return 0;
}
