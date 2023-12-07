#include <SFML/Graphics.hpp>
#include <iostream>

struct Sprite
{
    sf::Texture t;
    sf::Sprite s;
};

// -- объ€влени€ констант --
constexpr unsigned ANTIALIASING_LEVEL = 8;
constexpr unsigned WINDOW_WIDTH = 800;
constexpr unsigned WINDOW_HEIGHT = 800;
constexpr unsigned MAX_FPS = 60;
constexpr unsigned MAP_SPRITES_WIDTH = 8;
constexpr unsigned MAP_SPRITES_COUNT = 30;
constexpr unsigned SPRITE_SIZE = 24;
constexpr unsigned MAP_WIDTH = 28;

// -- предварительные объ€влени€ функций (всех, кроме main) --
void createWindow(sf::RenderWindow &);
void initMap(std::vector<int> &);
bool initSprites(sf::Image, std::vector<Sprite *> &, int);
void handleEvents(sf::RenderWindow &);
void renderMap(sf::RenderWindow &, std::vector<int>, std::vector<Sprite *>);
void clearSprites(std::vector<Sprite *> &);

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

bool initSprites(sf::Image sprites_file, std::vector<Sprite *> &sprites, int sprites_count)
{
    Sprite *sprite;

    for (int i = 0; i < sprites_count; i++)
    {
        int x = i % MAP_SPRITES_WIDTH * SPRITE_SIZE;
        int y = i / MAP_SPRITES_WIDTH * SPRITE_SIZE;
        sprite = new Sprite;
        sprites.push_back(sprite);
        sprites[i]->t.loadFromImage(sprites_file, sf::IntRect(x, y, SPRITE_SIZE, SPRITE_SIZE));
        sprites[i]->s.setTexture(sprites[i]->t);
    }

    return true;
}

// ‘ункци€ обрабатывает все событи€, скопившиес€ в очереди событий SFML.
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

// ‘ункци€ рисует карту
void renderMap(sf::RenderWindow &window, std::vector<int> map, std::vector<Sprite *> sprites)
{
    window.clear();

    int x, y;

    for (int i = 0; i < map.size(); i++)
    {
        x = i % MAP_WIDTH * SPRITE_SIZE;
        y = i / MAP_WIDTH * SPRITE_SIZE;
        sprites[map[i]]->s.setPosition(sf::Vector2f(x, y));
        window.draw(sprites[map[i]]->s);
    }

    window.display();
}

void initMap(std::vector<int> &map)
{
    std::array<int, 28 * 11> mapArray = {
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
    };

    for (int i = 0; i < mapArray.size(); i++)
    {
        map.push_back(mapArray[i]);
    }
}

void clearSprites(std::vector<Sprite *> &sprites)
{
    for (int i = 0; i < sprites.size(); i++)
    {
        delete sprites[i];
    }

    sprites.clear();
}

int main(int, char *[])
{
    sf::RenderWindow window;
    createWindow(window);

    sf::Image sprites_file;
    if (!sprites_file.loadFromFile("sprites.png"))
    {
        std::cout << "Error loading image!" << std::endl;
        return false;
    }

    std::vector<Sprite *> sprites;
    initSprites(sprites_file, sprites, MAP_SPRITES_COUNT);

    std::vector<int> map;
    initMap(map);

    sf::Clock clock;
    while (window.isOpen())
    {
        handleEvents(window);
        // update(clock, packman);
        renderMap(window, map, sprites);
    }

    clearSprites(sprites);

    return 0;
}
