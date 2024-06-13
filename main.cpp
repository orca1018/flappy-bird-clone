#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

// remove namespaces
using namespace sf;
using namespace std;

struct Audio {
  SoundBuffer die_buffer;
  SoundBuffer hit_buffer;
  SoundBuffer point_buffer;
  SoundBuffer swoosh_buffer;
  SoundBuffer wind_buffer;
  Sound die;
  Sound hit;
  Sound point;
  Sound swoosh;
  Sound wind;
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
  Texture flappy[3][3];
  Texture base;
  Texture gameover;
  Texture message;
  Texture background[2];
  Texture pipe[2];
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
    unsigned short int highscore = 0;
    unsigned int frames = 0;
    game_state game_state = waiting;
};

int main() {
  RenderWindow window(VideoMode(288, 512), "Flappy Bird");
  window.setFramerateLimit(120);
  window.setKeyRepeatEnabled(false);
  window.setVerticalSyncEnabled(true);

  srand(time(0));

  Audio sounds;
  load_audio(sounds);

  Textures textures;
  load_textures(textures);

  // Set up bird sprite
  Sprite birdSprite;
  birdSprite.setTexture(textures.flappy[0]);

  float x = 144.0f;
  float y = 256.0f;

  // will calibrate these value later
  float velocity = 0.0f;
  const float gravity = 0.35f;
  const float flapStrength = -6.0f;
  const float maxVelocity = 15.0f; // Maximum falling speed

  while (window.isOpen()) {
    Event event;
    while (window.pollEvent(event)) {
      if (event.type == Event::Closed) {
        window.close();
      }
      // Development phase shortcut to close the window
      if (event.type == Event::KeyPressed && event.key.code == Keyboard::Q) {
        window.close();
      }
      if (event.type == Event::KeyPressed &&
          event.key.code == Keyboard::Space) {
        velocity = flapStrength;
        sounds.swoosh.play();
      }
    }

    velocity += gravity;
    if (velocity > maxVelocity) {
      velocity = maxVelocity;
    }

    y += velocity;

    if (y < 0) {
      y = 0;
      velocity = 0;
    } else if (y > window.getSize().y - birdSprite.getGlobalBounds().height) {
      y = window.getSize().y - birdSprite.getGlobalBounds().height;
      velocity = 0;
    }

    window.clear();
    birdSprite.setPosition(x, y);
    window.draw(birdSprite);
    window.display();
  }

  return 0;
}
