#include "FallingRockEntity.h"
#include "PlayerEntity.h"
#include "sfml_game/SpriteEntity.h"
#include "sfml_game/ImageManager.h"
#include "sfml_game/SoundManager.h"
#include "Constants.h"
#include "WitchBlastGame.h"

FallingRockEntity::FallingRockEntity(float x, float y, int rockType)
  : EnemyEntity (ImageManager::getImageManager()->getImage(IMAGE_CYCLOP), x, y)
{
  imagesProLine = 20;
  type = ENTITY_ENNEMY;
  movingStyle = movFlying;
  bloodColor = bloodNone; // stones don't bleed

  age = 0.0f;
  h = 1800 + rand() % 1000;
  hp = 24;
  jumping = false;
  hVelocity = 0.0f;

  this->rockType = rockType;
  enemyType = EnemyTypeRockFalling;

  switch (rockType)
  {
  case 0: meleeDamages = 8; frame = 18; break;
  case 1: meleeDamages = 10; frame = 38; break;
  case 2: meleeDamages = 12; frame = 58; break;
  }
}

void FallingRockEntity::animate(float delay)
{
  if (jumping)
  {
    hVelocity -= 700.0f * delay;
    h += hVelocity * delay;

    if (h <= 0.0f) dying();
  }
  else
  {
    h -= delay * 750.0f;
    if (canCollide()) testSpriteCollisions();
    if (h <= 0.0f)
    {
      hVelocity = 250.0f;
      jumping = true;
    }
  }

}

void FallingRockEntity::render(sf::RenderTarget* app)
{
  int nx = frame % imagesProLine;
  int ny = frame / imagesProLine;

  // shadow
  if (h <= 1600)
  {
    int f = 1600 - h;
    if (f > 255) f = 255;

    sprite.setColor(sf::Color(255, 255, 255, f));
    sprite.setPosition(x, y);
    sprite.setTextureRect(sf::IntRect((nx + 1) * width, ny * height, width, height));
    app->draw(sprite);
    sprite.setColor(sf::Color(255, 255, 255, 255));
  }

  sprite.setPosition(x, y - h);
  sprite.setTextureRect(sf::IntRect(nx * width, ny * height, width, height));
  app->draw(sprite);

  #ifdef SHOW_BOUNDING_BOX
  sf::Vertex line[] =
  {
      sf::Vertex(sf::Vector2f(boundingBox.left, boundingBox.top)),
      sf::Vertex(sf::Vector2f(boundingBox.left + boundingBox.width, boundingBox.top)),
      sf::Vertex(sf::Vector2f(boundingBox.left + boundingBox.width, boundingBox.top)),
      sf::Vertex(sf::Vector2f(boundingBox.left + boundingBox.width, boundingBox.top + boundingBox.height)),
      sf::Vertex(sf::Vector2f(boundingBox.left + boundingBox.width, boundingBox.top + boundingBox.height)),
      sf::Vertex(sf::Vector2f(boundingBox.left, boundingBox.top + boundingBox.height)),
      sf::Vertex(sf::Vector2f(boundingBox.left, boundingBox.top + boundingBox.height)),
      sf::Vertex(sf::Vector2f(boundingBox.left, boundingBox.top))
  };
  app->draw(line, 8, sf::Lines);
  #endif
}

void FallingRockEntity::calculateBB()
{
  int w = 30;
  if (rockType == 1) w = 40;
  else if (rockType == 2) w = 50;

  boundingBox.left = (int)x - w / 2;
  boundingBox.width = w;
  boundingBox.top = (int)y - w / 2;
  boundingBox.height =  w;
}

bool FallingRockEntity::canCollide()
{
  return h < 70;
}

void FallingRockEntity::dying()
{
  isDying = true;
  game().addKilledEnemy(enemyType);
  SoundManager::getSoundManager()->playSound(SOUND_ROCK_IMPACT);
  game().makeShake(0.1f);

  for (int i = 0; i < 4; i++)
  {
    SpriteEntity* blood = new SpriteEntity(ImageManager::getImageManager()->getImage(IMAGE_BLOOD), x, y, 16, 16, 6);
    blood->setZ(OFFSET_Y - 1);

    blood->setFrame(12 + rand()%6);
    blood->setType(ENTITY_BLOOD);
    blood->setVelocity(Vector2D(rand()%150));
    blood->setViscosity(0.95f);

    float bloodScale = 1.0f + (rand() % 10) * 0.1f;
    blood->setScale(bloodScale, bloodScale);
  }
}
