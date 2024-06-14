#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;

struct Audio {
  sf::SoundBuffer die_buffer;
  sf::SoundBuffer hit_buffer;
  sf::SoundBuffer point_buffer;
  sf::SoundBuffer swoosh_buffer;
  sf::SoundBuffer wind_buffer;
  sf::Sound die;
  sf::Sound hit;
  sf::Sound point;
  sf::Sound swoosh;
  sf::Sound wind;
};

void load_audio(Audio &sounds) {
  sounds.die_buffer.loadFromFile("./audio/die.wav");
  sounds.die.setBuffer(sounds.die_buffer);
  sounds.hit_buffer.loadFromFile("./audio/hit.wav");
  sounds.hit.setBuffer(sounds.hit_buffer);
  sounds.point_buffer.loadFromFile("./audio/point.wav");
  sounds.point.setBuffer(sounds.point_buffer);
  sounds.swoosh_buffer.loadFromFile("./audio/swoosh.wav");
  sounds.swoosh.setBuffer(sounds.swoosh_buffer);
  sounds.wind_buffer.loadFromFile("./audio/wind.wav");
  sounds.wind.setBuffer(sounds.wind_buffer);
}

struct Textures {
  sf::Texture flappy[3][3];
  sf::Texture base;
  sf::Texture gameover;
  sf::Texture message;
  sf::Texture background[2];
  sf::Texture pipe[2];
};

void load_textures(Textures &textures) {
  // yellow bird
  textures.flappy[0][0].loadFromFile("./sprites/yellowbird-upflap.png");
  textures.flappy[0][1].loadFromFile("./sprites/yellowbird-midflap.png");
  textures.flappy[0][2].loadFromFile("./sprites/yellowbird-downflap.png");
  // red bird
  textures.flappy[1][0].loadFromFile("./sprites/redbird-downflap.png");
  textures.flappy[1][1].loadFromFile("./sprites/redbird-downflap.png");
  textures.flappy[1][2].loadFromFile("./sprites/redbird-downflap.png");
  // blue bird
  textures.flappy[2][0].loadFromFile("./sprites/bluebird-downflap.png");
  textures.flappy[2][1].loadFromFile("./sprites/bluebird-downflap.png");
  textures.flappy[2][2].loadFromFile("./sprites/bluebird-downflap.png");
  textures.base.loadFromFile("./sprites/base.png");
  textures.gameover.loadFromFile("./sprites/gameover.png");
  textures.message.loadFromFile("./sprites/message.png");
  // day background
  textures.background[0].loadFromFile("./sprites/background-day.png");
  // night background
  textures.background[1].loadFromFile("./sprites/background-night.png");
  // green pipe
  textures.pipe[0].loadFromFile("./sprites/pipe-green.png");
  // red pipe
  textures.pipe[1].loadFromFile("./sprites/pipe-red.png");
}

enum game_state { waiting, started, gameover };

struct Game {
  unsigned short int score = 0;
  unsigned int frames = 0;
  game_state game_state = waiting;
};

struct Preference {
  sf::Texture flappy[3];
  sf::Texture background;
  sf::Texture pipe;
  sf::Texture base;
};

void settings(Preference &preference, const Textures &textures) {
  preference.flappy[0] = textures.flappy[0][0];
  preference.flappy[1] = textures.flappy[0][1];
  preference.flappy[2] = textures.flappy[0][2];
  preference.background = textures.background[0];
  preference.pipe = textures.pipe[0];
  preference.base = textures.base;
}

struct Physics {
  double velocity = 0.0f;
  const float gravity = 0.35f;
  const float flap_strength = -6.0f;
  const float max_velocity = 15.0f;
  const float initial_x = 144.0f;
  const float initial_y = 256.0f;
};

void restart(float *x, float *y, Physics &physics, Game &game) {
  game.score = 0;
  game.frames = 0;
  game.game_state = waiting;
  physics.velocity = 0;
  *x = physics.initial_x;
  *y = physics.initial_y;
}

int main() {
  sf::RenderWindow window(sf::VideoMode(288, 512), "Flappy Bird");
  window.setFramerateLimit(60);
  window.setKeyRepeatEnabled(false);
  window.setVerticalSyncEnabled(true);

  srand(time(0));

  Audio sounds;
  load_audio(sounds);

  Textures textures;
  load_textures(textures);

  Game game;

  Preference preference;
  settings(preference, textures);

  Physics physics;

  // Create background and base sprites
  sf::Sprite background;
  background.setTexture(preference.background);
  background.setPosition(0, 0);

  sf::Sprite base;
  base.setTexture(preference.base);
  base.setPosition(0,
                   window.getSize().y -
                       preference.base.getSize().y); // Position at the bottom

  sf::Sprite flappy[3];
  flappy[0].setTexture(preference.flappy[0]);
  flappy[1].setTexture(preference.flappy[1]);
  flappy[2].setTexture(preference.flappy[2]);

  // Set initial position for Flappy Bird
  float x = physics.initial_x;
  float y = physics.initial_y;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == Event::Closed) {
        window.close();
      } else if (event.type == Event::KeyPressed &&
                 event.key.code == Keyboard::Space) {
        if (game.game_state == waiting) {
          game.game_state = started;
        } else if (game.game_state == started) {
          physics.velocity = physics.flap_strength;
          sounds.swoosh.play();
        } else if (game.game_state == gameover) {
          restart(&x, &y, physics, game);
        }
      }
    }

    if (game.game_state == started) {
      physics.velocity += physics.gravity;
      if (physics.velocity > physics.max_velocity) {
        physics.velocity = physics.max_velocity;
      }
      y += physics.velocity;

      if (y < 0) {
        y = 0;
        physics.velocity = 0;
      } else if (y > window.getSize().y - base.getGlobalBounds().height -
                         flappy[0].getGlobalBounds().height) {
        y = window.getSize().y - base.getGlobalBounds().height -
            flappy[0].getGlobalBounds().height;
        physics.velocity = 0;
        game.game_state = gameover;
        sounds.hit.play();
        sounds.die.play();
      }
    }

    window.clear();
    window.draw(background);
    window.draw(base);
    flappy[0].setPosition(x, y);
    window.draw(flappy[0]);
    window.display();
  }

  return 0;
}
