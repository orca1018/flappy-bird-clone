#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace sf;

struct Audio {
  sf::SoundBuffer die_buffer;
  sf::SoundBuffer hit_buffer;
  sf::SoundBuffer point_buffer;
  sf::SoundBuffer swoosh_buffer;
  sf::SoundBuffer wing_buffer;
  sf::Sound die;
  sf::Sound hit;
  sf::Sound point;
  sf::Sound swoosh;
  sf::Sound wing;
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
  sounds.wing_buffer.loadFromFile("./audio/wing.wav");
  sounds.wing.setBuffer(sounds.wing_buffer);
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
  const float gravity = 0.25f;
  const float flap_strength = -4.5f;
  const float max_velocity = 10.0f;
  const float initial_x = 144.0f;
  const float initial_y = 256.0f;
};

void restart(float *x, float *y, Physics &physics, Game &game,
             std::vector<sf::Sprite> &pipes) {
  game.score = 0;
  game.frames = 0;
  game.game_state = waiting;
  physics.velocity = 0;
  *x = physics.initial_x;
  *y = physics.initial_y;
  pipes.clear(); // Clear the pipes
}

void generate_pipes(std::vector<sf::Sprite> &pipes, sf::Texture &pipe) {
  int r = rand() % 175 + 50;
  int gap = 100;
  int start = 288; // X position for both pipes

  // Lower pipe
  sf::Sprite lower_pipe;
  lower_pipe.setTexture(pipe);
  lower_pipe.setPosition(start, r + gap);

  // Upper pipe
  sf::Sprite upper_pipe;
  upper_pipe.setTexture(pipe);
  upper_pipe.setRotation(180);
  upper_pipe.setPosition(start + pipe.getSize().x, r); // Adjust position

  // Push to the array
  pipes.push_back(lower_pipe);
  pipes.push_back(upper_pipe);
}

bool collision(const sf::Sprite &flappy, const std::vector<sf::Sprite> &pipes) {
  sf::FloatRect flappy_bounds = flappy.getGlobalBounds();

  for (const auto &pipe : pipes) {
    sf::FloatRect pipe_bounds = pipe.getGlobalBounds();
    if (flappy_bounds.intersects(pipe_bounds)) {
      return true;
    }
  }
  return false;
}

bool check_passed_pipe(const sf::Sprite &flappy, const sf::Sprite &pipe) {
  return flappy.getPosition().x >
         pipe.getPosition().x + pipe.getGlobalBounds().width;
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

  sf::Font font;
  if (!font.loadFromFile("./font.ttf")) {
    std::cerr << "Error loading font!" << std::endl;
    return -1;
  }

  sf::Text score_text;
  score_text.setFont(font);
  score_text.setCharacterSize(48);
  score_text.setFillColor(sf::Color::White);
  score_text.setPosition(10, 10);

  // Create background and base sprites
  sf::Sprite background_s;
  background_s.setTexture(preference.background);
  background_s.setPosition(0, 0);

  sf::Sprite base_s;
  base_s.setTexture(preference.base);
  base_s.setPosition(0,
                     window.getSize().y -
                         preference.base.getSize().y); // Position at the bottom

  sf::Sprite flappy[3];
  flappy[0].setTexture(preference.flappy[0]);
  flappy[1].setTexture(preference.flappy[1]);
  flappy[2].setTexture(preference.flappy[2]);

  sf::Sprite message_s;
  message_s.setTexture(textures.message);
  message_s.setPosition(
      (window.getSize().x - message_s.getGlobalBounds().width) / 2,
      (window.getSize().y - message_s.getGlobalBounds().height) / 2);

  sf::Sprite gameover_s;
  gameover_s.setTexture(textures.gameover);
  gameover_s.setPosition(
      (window.getSize().x - gameover_s.getGlobalBounds().width) / 2,
      (window.getSize().y - gameover_s.getGlobalBounds().height) / 2);

  // Set initial position for Flappy Bird
  float x = physics.initial_x;
  float y = physics.initial_y;

  std::vector<sf::Sprite> pipes_vec;

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
          restart(&x, &y, physics, game, pipes_vec);
        }
      }
    }

    if (game.game_state == started) {
      physics.velocity += physics.gravity;
      if (physics.velocity > physics.max_velocity) {
        physics.velocity = physics.max_velocity;
      }
      y += physics.velocity;

      if (y < 0 || y + flappy[0].getGlobalBounds().height >=
                       window.getSize().y - base_s.getGlobalBounds().height) {
        // Bird hit the ground or flew off the screen
        y = window.getSize().y - base_s.getGlobalBounds().height -
            flappy[0].getGlobalBounds().height;
        sounds.hit.play();
        sounds.die.play();
        restart(&x, &y, physics, game, pipes_vec);
      }

      if (game.frames % 150 == 0) {
        generate_pipes(pipes_vec, preference.pipe);
      }

      // Move pipes
      for (auto &pipe : pipes_vec) {
        pipe.move(-1.0f, 0.0f); // Move towards left by 1 pixel/frame
      }

      int pipe_width = preference.pipe.getSize().x;
      // Remove pipes only after leaving the screen completely
      pipes_vec.erase(
          std::remove_if(pipes_vec.begin(), pipes_vec.end(),
                         [&](const sf::Sprite &pipe) {
                           return pipe.getPosition().x + pipe_width < 0;
                         }),
          pipes_vec.end());

      // Check for collisions
      for (auto &pipe : pipes_vec) {
        if (collision(flappy[0], pipes_vec)) {
          sounds.hit.play();
          sounds.die.play();
          restart(&x, &y, physics, game, pipes_vec);
        }
      }

      // Check for scoring based on generation frame and threshold
      for (auto &pipe : pipes_vec) {
        if (!pipe.getTextureRect().contains(0, 0)) {
          continue; // Skip lower pipes
        }
        int threshold = 150 + 144; // Adjust threshold based on desired timing
        if ((game.frames - 144) / 150 > game.score) {
          game.score++;
          sounds.point.play();
        }
      }
    }

        if (game.score > game.frames) {
            restart(&x, &y, physics, game, pipes_vec);
        }

    window.clear();
    window.draw(background_s);

    // Draw pipes
    for (const auto &pipe : pipes_vec) {
      window.draw(pipe);
    }

    window.draw(base_s);

    if (game.game_state == gameover) {
      window.draw(gameover_s);
    } else if (game.game_state == waiting) {
      window.draw(message_s);
    } else {
      int frame = (game.frames / 5) % 3;
      flappy[frame].setPosition(x, y);
      window.draw(flappy[frame]);
    }

    // Update and draw score
    if (game.game_state == started) {
      score_text.setString(std::to_string(game.score));
      window.draw(score_text);
    }

    window.display();

    game.frames++;
  }

  return 0;
}
