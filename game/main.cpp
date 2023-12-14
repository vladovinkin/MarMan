#include <SFML/Graphics.hpp>
#include <iostream>

struct Sprite
{
    sf::Texture t;
    sf::Sprite s;
};

// -- объявления констант --
constexpr unsigned ANTIALIASING_LEVEL = 8;
constexpr unsigned WINDOW_WIDTH = 800;
constexpr unsigned WINDOW_HEIGHT = 800;
constexpr unsigned MAX_FPS = 60;
constexpr unsigned MAP_SPRITES_WIDTH = 8;
constexpr unsigned MAP_SPRITES_COUNT = 36;
constexpr unsigned MAP_SPRITE_SIZE = 24;
constexpr unsigned MAP_WIDTH = 28;
constexpr unsigned HERO_SPRITES_COUNT = 6;
constexpr unsigned HERO_SPRITE_WIDTH = 24;
constexpr unsigned HERO_SPRITE_HEIGHT = 42;

// -- предварительные объявления функций (всех, кроме main) --
void createWindow(sf::RenderWindow &);
void initMap(std::vector<int> &);
bool initSpritesMap(sf::Image, std::vector<Sprite *> &, int);
bool initSpritesHero(sf::Image, std::vector<Sprite *> &, int);
void handleEvents(sf::RenderWindow &);
void renderMap(sf::RenderWindow &, std::vector<int>, std::vector<Sprite *>);
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
void handleEvents(sf::RenderWindow &window)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }
    }
}

// Функция рисует карту
void renderMap(sf::RenderWindow &window, std::vector<int> map, std::vector<Sprite *> sprites)
{
    int x, y;

    for (int i = 0; i < map.size(); i++)
    {
        x = i % MAP_WIDTH * MAP_SPRITE_SIZE;
        y = i / MAP_WIDTH * MAP_SPRITE_SIZE;
        sprites[map[i]]->s.setPosition(sf::Vector2f(x, y));
        window.draw(sprites[map[i]]->s);
    }
}

// Функция рисует героя
void renderHero(sf::RenderWindow &window, std::vector<Sprite *> sprites)
{
    // int coordX = MAP_SPRITE_SIZE * 14;
    // int coordY = MAP_SPRITE_SIZE * 17 + MAP_SPRITE_SIZE / 2;

    

    sprites[0]->s.setPosition(sf::Vector2f(coordX, coordY));
    window.draw(sprites[0]->s);
}

void initMap(std::vector<int> &map)
{
    std::array<int, 28 * 31> map_array = {
         4, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,16,17, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 5,
         8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,28,26, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,10,
         8, 1,15,29,29,14, 1,15,29,29,29,14, 1,28,26, 1,15,29,29,29,14, 1,15,29,29,14, 1,10,
         8, 2,28, 0, 0,26, 1,28, 0, 0, 0,26, 1,28,26, 1,28, 0, 0, 0,26, 1,28, 0, 0,26, 2,10,
         8, 1,13,27,27,12, 1,13,27,27,27,12, 1,13,12, 1,13,27,27,27,12, 1,13,27,27,12, 1,10,
         8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,10,
         8, 1,15,29,29,14, 1,15,14, 1,15,29,29,29,29,29,29,14, 1,15,14, 1,15,29,29,14, 1,10,
         8, 1,13,27,27,12, 1,28,26, 1,13,27,27,23,22,27,27,12, 1,28,26, 1,13,27,27,12, 1,10,
         8, 1, 1, 1, 1, 1, 1,28,26, 1, 1, 1, 1,28,26, 1, 1, 1, 1,28,26, 1, 1, 1, 1, 1, 1,10,
         6,11,11,11,11,14, 1,28,25,29,29,14, 0,28,26, 0,15,29,29,24,26, 1,15,11,11,11,11, 7,
         0, 0, 0, 0, 0, 8, 1,28,22,27,27,12, 0,13,12, 0,13,27,27,23,26, 1,10, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 8, 1,28,26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,28,26, 1,10, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 8, 1,28,26, 0,30,11,34, 3, 3,35,11,31, 0,28,26, 1,10, 0, 0, 0, 0, 0,
         9, 9, 9, 9, 9,12, 1,13,12, 0,10, 0, 0, 0, 0, 0, 0, 8, 0,13,12, 1,13, 9, 9, 9, 9, 9,
         0, 0, 0, 0, 0, 0, 1, 0, 0, 0,10, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
        11,11,11,11,11,14, 1,15,14, 0,10, 0, 0, 0, 0, 0, 0, 8, 0,15,14, 1,15,11,11,11,11,11,
         0, 0, 0, 0, 0, 8, 1,28,26, 0,32, 9, 9, 9, 9, 9, 9,33, 0,28,26, 1,10, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 8, 1,28,26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,28,26, 1,10, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 8, 1,28,26, 0,15,29,29,29,29,29,29,14, 0,28,26, 1,10, 0, 0, 0, 0, 0,
         4, 9, 9, 9, 9,12, 1,13,12, 0,13,27,27,23,22,27,27,12, 0,13,12, 1,13, 9, 9, 9, 9, 5,
         8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,28,26, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,10,
         8, 1,15,29,29,14, 1,15,29,29,29,14, 1,28,26, 1,15,29,29,29,14, 1,15,29,29,14, 1,10,
         8, 1,13,27,23,26, 1,13,27,27,27,12, 1,13,12, 1,13,27,27,27,12, 1,28,22,27,12, 1,10,
         8, 2, 1, 1,28,26, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,28,26, 1, 1, 2,10,
        18,29,14, 1,28,26, 1,15,14, 1,15,29,29,29,29,29,29,14, 1,15,14, 1,28,26, 1,15,29,19,
        20,27,12, 1,13,12, 1,28,26, 1,13,27,27,23,22,27,27,12, 1,28,26, 1,13,12, 1,13,27,21,
         8, 1, 1, 1, 1, 1, 1,28,26, 1, 1, 1, 1,28,26, 1, 1, 1, 1,28,26, 1, 1, 1, 1, 1, 1,10,
         8, 1,15,29,29,29,29,24,25,29,29,14, 1,28,26, 1,15,29,29,24,25,29,29,29,29,14, 1,10,
         8, 1,13,27,27,27,27,27,27,27,27,12, 1,13,12, 1,13,27,27,27,27,27,27,27,27,12, 1,10,
         8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,10,
         6,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11, 7,
    };

    for (int i = 0; i < map_array.size(); i++)
    {
        map.push_back(map_array[i]);
    }
}

void clearSprites(std::vector<Sprite *> &sprites_map)
{
    for (int i = 0; i < sprites_map.size(); i++)
    {
        delete sprites_map[i];
    }

    sprites_map.clear();
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
        return false;
    }

    if (!sprites_hero_file.loadFromFile("sprites_hero.png"))
    {
        std::cout << "Error loading hero sprites!" << std::endl;
        return false;
    }

    std::vector<Sprite *> sprites_map;
    initSpritesMap(sprites_map_file, sprites_map, MAP_SPRITES_COUNT);

    std::vector<Sprite *> sprites_hero;
    initSpritesHero(sprites_hero_file, sprites_hero, HERO_SPRITES_COUNT);

    std::vector<int> map;
    initMap(map);

    sf::Clock clock;
    while (window.isOpen())
    {
        handleEvents(window);
        // update(clock, packman);
        window.clear();

        renderMap(window, map, sprites_map);
        renderHero(window, sprites_hero);

        window.display();
    }

    clearSprites(sprites_map);

    return 0;
}
