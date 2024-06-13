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

void loadAudio(Audio &sounds) {
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
  Texture flappy[3];
  Texture base;
  Texture gameover;
  Texture message;
  Texture background;
  Texture pipe;
};

int main() {
  RenderWindow window(VideoMode(288, 512), "Flappy Bird");
  window.setFramerateLimit(120);
  window.setKeyRepeatEnabled(false);
  window.setVerticalSyncEnabled(true);

  srand(time(0));

  // load sounds to RAM
  Audio sounds;
  loadAudio(sounds);

  // load textures to RAM
  Textures textures;
  textures.flappy[0].loadFromFile("./sprites/yellowbird-upflap.png");
  textures.flappy[1].loadFromFile("./sprites/yellowbird-midflap.png");
  textures.flappy[2].loadFromFile("./sprites/yellowbird-downflap.png");
  /* red bird
    textures.flappy[0].loadFromFile("./sprites/redbird-downflap.png");
    textures.flappy[1].loadFromFile("./sprites/redbird-downflap.png");
    textures.flappy[2].loadFromFile("./sprites/redbird-downflap.png");
  */
  /* blue bird
    textures.flappy[0].loadFromFile("./sprites/bluebird-downflap.png");
    textures.flappy[1].loadFromFile("./sprites/bluebird-downflap.png");
    textures.flappy[2].loadFromFile("./sprites/bluebird-downflap.png");
  */
  textures.base.loadFromFile("./sprites/base.png");
  textures.gameover.loadFromFile("./sprites/gameover.png");
  textures.message.loadFromFile("./sprites/message.png");
  textures.background.loadFromFile("./sprites/background-day.png");
  // textures.background.loadFromFile("./sprites/background-night.png");
  textures.pipe.loadFromFile("./sprites/pipe-green.png");
  // textures.pipe.loadFromFile("./sprites/pipe-red.png");

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
