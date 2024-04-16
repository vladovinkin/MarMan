#include <SFML/Graphics.hpp>
#include <iostream>
#include <array>
#include <cmath>

// -- объ€влени€ констант --
constexpr unsigned ANTIALIASING_LEVEL = 8;
constexpr unsigned WINDOW_WIDTH = 800;
constexpr unsigned WINDOW_HEIGHT = 800;
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
constexpr unsigned PACKMAN_SPEED = 100.f;
constexpr unsigned LIMIT_PIXELS_TO_TURN = 3;

static const sf::Vector2f HERO_INITIAL_POSITION = {MAP_SPRITE_SIZE * 14, MAP_SPRITE_SIZE * 17 + MAP_SPRITE_SIZE / 2};

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

struct Hero
{
    sf::Vector2f position;
    Direction direction;
    Direction directionDesired;
};

struct Game
{
    int unsigned lives;
    int unsigned score;
    int unsigned highScore;
};

// -- предварительные объ€влени€ функций (всех, кроме main) --
void createWindow(sf::RenderWindow &);
void initMap(GameMap &);
void initHero(Hero &);
void initGame(Game &);
bool initSpritesMap(sf::Image, std::vector<Sprite *> &, int);
bool initSpritesHero(sf::Image, std::vector<Sprite *> &, int);
void handleEvents(sf::RenderWindow &, Hero &);
void renderMap(sf::RenderWindow &, const GameMap &, Hero);
void renderHero(sf::RenderWindow &, std::vector<Sprite *>);
void clearSprites(std::vector<Sprite *> &);
void updatePackman(Hero &, float, const GameMap &);
void calcCollisions(Hero &, GameMap &);

// -- определени€ функций --

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
        sprite = sprites[0];
        sprite->s.setScale(-1, 1);
        break;
    case Direction::RIGHT:
        sprite = sprites[0];
        sprite->s.setScale(1, 1);
        break;
    case Direction::UP:
        sprite = sprites[3];
        // sprite->s.setScale(1, 1);
        break;
    case Direction::DOWN:
        sprite = sprites[2];
        // sprite->s.setScale(1, 1);
        break;
    default:
        sprite = sprites[4];
        break;
    }

    sprite->s.setPosition(sf::Vector2f(hero.position.x, hero.position.y));
    window.draw(sprite->s);

    sprite = NULL;
    delete sprite;
}

void initGame(Game &game)
{
    game.lives = 3;
    game.score = 0;
    game.highScore = 0;
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
}

void clearSprites(std::vector<Sprite *> &sprites_map)
{
    for (int i = 0; i < sprites_map.size(); i++)
    {
        delete sprites_map[i];
    }

    sprites_map.clear();
}

void updatePackman(Hero &pacman, float elapsedTime, const GameMap &map)
{
    const float step = PACKMAN_SPEED * elapsedTime; // весь путь на итерацию
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
}

void calcCollisions(Hero &pacman, GameMap &map)
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

        if (std::fabs(pacman.position.x - positionItemCenter.x) < MapSpriteSizeThirdPart 
            && std::fabs(pacman.position.y - positionItemCenter.y) < MapSpriteSizeThirdPart)
        {
            std::cout << (nextSpriteValue == 1 ? "Coin" : "Star") << '\n';
            map[nextMapPositionY][nextMapPositionX] = 0;
        }
    }
}

void update(sf::Clock &clock, Hero &packman, GameMap &map)
{
    const float elapsedTime = clock.getElapsedTime().asSeconds();
    clock.restart();
    updatePackman(packman, elapsedTime, map);
    calcCollisions(packman, map);
}

int main(int, char *[])
{
    Game game;
    initGame(game);

    sf::Image sprites_map_file;
    sf::Image sprites_hero_file;

    if (!sprites_map_file.loadFromFile("sprites.png"))
    {
        std::cout << "Error loading map sprites!" << std::endl;
        return 1;
    }

    if (!sprites_hero_file.loadFromFile("sprites_hero.png"))
    {
        std::cout << "Error loading hero sprites!" << std::endl;
        return 1;
    }

    std::vector<Sprite *> sprites_map;
    initSpritesMap(sprites_map_file, sprites_map, MAP_SPRITES_COUNT);

    std::vector<Sprite *> sprites_hero;
    initSpritesHero(sprites_hero_file, sprites_hero, HERO_SPRITES_COUNT);

    GameMap map;
    initMap(map);

    Hero hero;
    initHero(hero);

    sf::RenderWindow window;
    createWindow(window);

    sf::Clock clock;
    while (window.isOpen())
    {
        handleEvents(window, hero);
        update(clock, hero, map);
        window.clear();

        renderMap(window, map, sprites_map);
        renderHero(window, sprites_hero, hero);

        window.display();
    }

    clearSprites(sprites_map);

    return 0;
}
