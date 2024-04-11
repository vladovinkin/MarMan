#include <SFML/Graphics.hpp>
#include <iostream>
#include <array>

// -- объявления констант --
constexpr unsigned ANTIALIASING_LEVEL = 8;
constexpr unsigned WINDOW_WIDTH = 800;
constexpr unsigned WINDOW_HEIGHT = 800;
constexpr unsigned MAX_FPS = 60;
constexpr unsigned MAP_SPRITES_WIDTH = 8;
constexpr unsigned MAP_SPRITES_COUNT = 36;
constexpr unsigned MAP_SPRITE_SIZE = 24;
constexpr unsigned GAME_MAP_WIDTH = 28;
constexpr unsigned GAME_MAP_HEIGHT = 31;
constexpr unsigned HERO_SPRITES_COUNT = 6;
constexpr unsigned HERO_SPRITE_WIDTH = 24;
constexpr unsigned HERO_SPRITE_HEIGHT = 42;
constexpr unsigned PACKMAN_SPEED = 40.f;

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

// -- предварительные объявления функций (всех, кроме main) --
void createWindow(sf::RenderWindow &);
void initMap(GameMap &);
void initHero(Hero &);
bool initSpritesMap(sf::Image, std::vector<Sprite *> &, int);
bool initSpritesHero(sf::Image, std::vector<Sprite *> &, int);
void handleEvents(sf::RenderWindow &, Hero &);
void renderMap(sf::RenderWindow &, const GameMap &, Hero);
void renderHero(sf::RenderWindow &, std::vector<Sprite *>);
void clearSprites(std::vector<Sprite *> &);

// -- определения функций --

// Функция создаёт окно приложения.
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

// Функция обрабатывает все события, скопившиеся в очереди событий SFML.
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

// Функция рисует карту
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

// Функция рисует героя
void renderHero(sf::RenderWindow &window, std::vector<Sprite *> sprites, Hero hero)
{
    sprites[0]->s.setPosition(sf::Vector2f(hero.position.x, hero.position.y));
    sprites[0]->s.setScale(-1, 1);
    window.draw(sprites[0]->s);

    sf::Vertex point(sf::Vector2f(hero.position.x - 10, hero.position.y - 10), sf::Color::White);
    window.draw(&point, 1, sf::Points);
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
    const float step = PACKMAN_SPEED * elapsedTime;
    sf::Vector2f position = pacman.position;

    Direction dd = pacman.directionDesired;
    if (dd != Direction::NONE && dd != pacman.direction)
    {
        pacman.direction = dd;
    }

    int curMapPositionY = static_cast<int>(position.y) / MAP_SPRITE_SIZE;
    int curMapPositionX = static_cast<int>(position.x) / MAP_SPRITE_SIZE;
    int newMapPositionX, newMapPositionY;

    switch (pacman.direction)
    {
    case Direction::UP:
        position.y -= step;
        newMapPositionY = (static_cast<int>(position.y) - MAP_SPRITE_SIZE / 2) / MAP_SPRITE_SIZE;
        if (newMapPositionY != curMapPositionY && newMapPositionY >= 0 && newMapPositionY <= GAME_MAP_HEIGHT - 1)
        {
            if (map[newMapPositionY][curMapPositionX] > 2)
            {
                position.y = curMapPositionY * MAP_SPRITE_SIZE + MAP_SPRITE_SIZE / 2;
                pacman.direction = Direction::NONE;
            }
        }
        break;
    case Direction::DOWN:
        position.y += step;
        newMapPositionY = (static_cast<int>(position.y) + MAP_SPRITE_SIZE / 2) / MAP_SPRITE_SIZE;
        if (newMapPositionY != curMapPositionY && newMapPositionY >= 0 && newMapPositionY <= GAME_MAP_HEIGHT - 1)
        {
            if (map[newMapPositionY][curMapPositionX] > 2)
            {
                position.y = curMapPositionY * MAP_SPRITE_SIZE + MAP_SPRITE_SIZE / 2;
                pacman.direction = Direction::NONE;
            }
        }
        break;
    case Direction::LEFT:
        position.x -= step;
        newMapPositionX = (static_cast<int>(position.x) - MAP_SPRITE_SIZE / 2) / MAP_SPRITE_SIZE;
        if (newMapPositionX != curMapPositionX && newMapPositionX >= 0 && newMapPositionX <= GAME_MAP_WIDTH - 1)
        {
            if (map[curMapPositionY][newMapPositionX] > 2)
            {
                position.x = curMapPositionX * MAP_SPRITE_SIZE + MAP_SPRITE_SIZE / 2;
                pacman.direction = Direction::NONE;
            }
        }
        break;
    case Direction::RIGHT:
        position.x += step;
        newMapPositionX = (static_cast<int>(position.x) + MAP_SPRITE_SIZE / 2) / MAP_SPRITE_SIZE;
        if (newMapPositionX != curMapPositionX && newMapPositionX >= 0 && newMapPositionX <= GAME_MAP_WIDTH - 1)
        {
            if (map[curMapPositionY][newMapPositionX] > 2)
            {
                position.x = newMapPositionX * MAP_SPRITE_SIZE - MAP_SPRITE_SIZE / 2;
                pacman.direction = Direction::NONE;
            }
        }
        break;
    case Direction::NONE:
        break;
    }
    pacman.position = position;
}

void update(sf::Clock &clock, Hero &packman, GameMap &map)
{
    const float elapsedTime = clock.getElapsedTime().asSeconds();
    clock.restart();
    updatePackman(packman, elapsedTime, map);
}

int main(int, char *[])
{
    sf::RenderWindow window;
    createWindow(window);

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
