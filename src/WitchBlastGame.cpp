/**  This file is part of Witch Blast.
  *
  *  Witch Blast is free software: you can redistribute it and/or modify
  *  it under the terms of the GNU General Public License as published by
  *  the Free Software Foundation, either version 3 of the License, or
  *  (at your option) any later version.
  *
  *  Witch Blast is distributed in the hope that it will be useful,
  *  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *  GNU General Public License for more details.
  *
  *  You should have received a copy of the GNU General Public License
  *  along with Witch Blast.  If not, see <http://www.gnu.org/licenses/>.
  */

#include "WitchBlastGame.h"
#include "sfml_game/SpriteEntity.h"
#include "sfml_game/TileMapEntity.h"
#include "DungeonMap.h"
#include "sfml_game/ImageManager.h"
#include "sfml_game/SoundManager.h"
#include "sfml_game/EntityManager.h"
#include "Constants.h"
#include "RatEntity.h"
#include "BlackRatEntity.h"
#include "GreenRatEntity.h"
#include "KingRatEntity.h"
#include "GiantSlimeEntity.h"
#include "BatEntity.h"
#include "SlimeEntity.h"
#include "ChestEntity.h"
#include "EvilFlowerEntity.h"
#include "ItemEntity.h"
#include "ArtefactDescriptionEntity.h"
#include "PnjEntity.h"
#include "TextEntity.h"

#include <iostream>
#include <sstream>
#include <fstream>

namespace {
WitchBlastGame* gameptr;
}

WitchBlastGame::WitchBlastGame(): Game(SCREEN_WIDTH, SCREEN_HEIGHT)
{
  gameptr = this;
  app->setTitle(APP_NAME + " V" + APP_VERSION);

  // loading resources
  ImageManager::getImageManager()->addImage((char*)"media/player_base.png");
  ImageManager::getImageManager()->addImage((char*)"media/player_equip.png");
  ImageManager::getImageManager()->addImage((char*)"media/player_collar.png");
  ImageManager::getImageManager()->addImage((char*)"media/bolt.png");
  ImageManager::getImageManager()->addImage((char*)"media/tiles.png");
  ImageManager::getImageManager()->addImage((char*)"media/rat.png");
  ImageManager::getImageManager()->addImage((char*)"media/minimap.png");
  ImageManager::getImageManager()->addImage((char*)"media/doors.png");
  ImageManager::getImageManager()->addImage((char*)"media/items.png");
  ImageManager::getImageManager()->addImage((char*)"media/items_equip.png");
  ImageManager::getImageManager()->addImage((char*)"media/chest.png");
  ImageManager::getImageManager()->addImage((char*)"media/bat.png");
  ImageManager::getImageManager()->addImage((char*)"media/evil_flower.png");
  ImageManager::getImageManager()->addImage((char*)"media/slime.png");
  ImageManager::getImageManager()->addImage((char*)"media/giant_slime.png");
  ImageManager::getImageManager()->addImage((char*)"media/king_rat.png");
  ImageManager::getImageManager()->addImage((char*)"media/blood.png");
  ImageManager::getImageManager()->addImage((char*)"media/corpses.png");
  ImageManager::getImageManager()->addImage((char*)"media/corpses_big.png");
  ImageManager::getImageManager()->addImage((char*)"media/star.png");
  ImageManager::getImageManager()->addImage((char*)"media/star2.png");
  ImageManager::getImageManager()->addImage((char*)"media/interface.png");
  ImageManager::getImageManager()->addImage((char*)"media/hud_shots.png");

  ImageManager::getImageManager()->addImage((char*)"media/pnj.png");
  ImageManager::getImageManager()->addImage((char*)"media/fairy.png");

  SoundManager::getSoundManager()->addSound((char*)"media/sound/step.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/blast00.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/blast01.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/door_closing.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/door_opening.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/chest_opening.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/impact.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/bonus.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/drink.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/eat.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/player_hit.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/player_die.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/ennemy_dying.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/coin.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/pay.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/wall_impact.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/big_wall_impact.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/king_rat_cry_1.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/king_rat_cry_2.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/king_rat_die.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/slime_jump.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/slime_impact.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/slime_impact_weak.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/slime_smash.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/ice_charge.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/select.ogg");
  SoundManager::getSoundManager()->addSound((char*)"media/sound/heart.ogg");

  if (font.loadFromFile("media/DejaVuSans-Bold.ttf"))
  {
    myText.setFont(font);
  }

  miniMap = NULL;
  currentMap = NULL;
  currentFloor = NULL;
  xGameState = xGameStateNone;
  isPausing = false;

  shotsSprite.setTexture(*ImageManager::getImageManager()->getImage(IMAGE_HUD_SHOTS));

  configureFromFile();
  srand(time(NULL));
}

WitchBlastGame::~WitchBlastGame()
{
  // cleaning all entities
  EntityManager::getEntityManager()->clean();

  // cleaning data
  if (miniMap != NULL) delete (miniMap);
  if (currentFloor != NULL) delete (currentFloor);
}

DungeonMap* WitchBlastGame::getCurrentMap()
{
  return currentMap;
}

PlayerEntity* WitchBlastGame::getPlayer()
{
  return player;
}

Vector2D WitchBlastGame::getPlayerPosition()
{
  return Vector2D(player->getX(), player->getY());
}

int WitchBlastGame::getLevel()
{
  return level;
}

void WitchBlastGame::onUpdate()
{
  float delta = getAbsolutTime() - lastTime;
  lastTime = getAbsolutTime();

  if (!isPausing)
  {

    EntityManager::getEntityManager()->animate(delta);

    if (xGameState != xGameStateNone)
    {
      xGameTimer -= delta;
      if (xGameTimer <= 0.0f)
      {
        if (xGameState == xGameStateFadeOut)
        {
          if (player->getPlayerStatus() == PlayerEntity::playerStatusGoingUp)
          {
            level++;
            startNewLevel();
          }
          else
            startNewGame(false);
        }

        else
          xGameState = xGameStateNone;
      }
    }

    if (isPlayerAlive)
    {
      if (player->getHp() <= 0)
      {
        isPlayerAlive = false;
        playMusic(MusicEnding);
      }
    }
  }
}

void WitchBlastGame::startNewGame(bool fromSaveFile)
{
  gameState = gameStateInit;
  level = 1;

  // cleaning all entities
  EntityManager::getEntityManager()->clean();

  // cleaning data
  if (miniMap != NULL) delete (miniMap);
  if (currentFloor != NULL) delete (currentFloor);
  miniMap = NULL;
  currentFloor = NULL;

  // current map (tiles)
  currentTileMap = new TileMapEntity(ImageManager::getImageManager()->getImage(IMAGE_TILES), currentMap, 64, 64, 10);
  currentTileMap->setX(OFFSET_X);
  currentTileMap->setY(OFFSET_Y);

  // the interface
  SpriteEntity* interface = new SpriteEntity(ImageManager::getImageManager()->getImage(IMAGE_INTERFACE));
  interface->setZ(10000.0f);
  interface->removeCenter();
  interface->setType(0);

  // key symbol on the interface
  keySprite.setTexture(*ImageManager::getImageManager()->getImage(IMAGE_ITEMS_EQUIP));
  keySprite.setTextureRect(sf::IntRect(ITEM_WIDTH * EQUIP_BOSS_KEY, 0,  ITEM_WIDTH, ITEM_HEIGHT));
  keySprite.setPosition(326, 616);

  miniMap = new GameMap(FLOOR_WIDTH, FLOOR_HEIGHT);
  // minimap on the interface
  TileMapEntity* miniMapEntity = new TileMapEntity(ImageManager::getImageManager()->getImage(IMAGE_MINIMAP), miniMap, 15, 11, 10);
  miniMapEntity->setTileBox(16, 12);
  miniMapEntity->setX(407);
  miniMapEntity->setY(614);
  miniMapEntity->setZ(10001.0f);

  // doors
  doorEntity[0] = new DoorEntity(8);
  doorEntity[1] = new DoorEntity(4);
  doorEntity[2] = new DoorEntity(2);
  doorEntity[3] = new DoorEntity(6);

  if (fromSaveFile)
  {
    if (!loadGame())
      fromSaveFile = false;
    else
      playLevel();
  }
  if (!fromSaveFile)
  {
    // the player
    player = new PlayerEntity(OFFSET_X + (TILE_WIDTH * MAP_WIDTH * 0.5f),
                              OFFSET_Y + (TILE_HEIGHT * MAP_HEIGHT * 0.5f));

    startNewLevel();
  }
}

void WitchBlastGame::startNewLevel()
{
  // create the new level
  if (currentFloor != NULL) delete currentFloor;
  currentFloor = new GameFloor(level);
  currentFloor->createFloor();

  // center it
  floorX = FLOOR_WIDTH / 2;
  floorY = FLOOR_HEIGHT / 2;

  // move the player
  if (level == 1)
    player->moveTo(OFFSET_X + (TILE_WIDTH * MAP_WIDTH * 0.5f),
                              OFFSET_Y + (TILE_HEIGHT * MAP_HEIGHT * 0.5f));
  else
    player->moveTo(OFFSET_X + (TILE_WIDTH * MAP_WIDTH * 0.5f),
                              OFFSET_Y + (TILE_HEIGHT * MAP_HEIGHT - 3 * TILE_HEIGHT));

  // the boss room is closed
  bossRoomOpened = false;
  playLevel();
}

void WitchBlastGame::playLevel()
{
  isPlayerAlive = true;
  player->setVelocity(Vector2D(0.0f, 0.0f));
  player->setPlayerStatus(PlayerEntity::playerStatusPlaying);

  // generate the map
  refreshMap();
  // items from save
  currentMap->restoreMapObjects();

  // first map is open
  roomClosed = false;

  // game time counter an state
  lastTime = getAbsolutTime();
  gameState = gameStatePlaying;

  playMusic(MusicDungeon);

  // fade in
  xGameState = xGameStateFadeIn;
  xGameTimer = FADE_IN_DELAY;

  float x0 = OFFSET_X + MAP_WIDTH * 0.5f * TILE_WIDTH;
  float y0 = OFFSET_Y + MAP_HEIGHT * 0.5f * TILE_HEIGHT + 40.0f;

  std::ostringstream oss;
  oss << "Level " << level;

  TextEntity* text = new TextEntity(oss.str(), 30, x0, y0);
  text->setAlignment(ALIGN_CENTER);
  text->setLifetime(2.5f);
  text->setWeight(-36.0f);
  text->setZ(1000);
  text->setColor(TextEntity::COLOR_FADING_WHITE);
}

void WitchBlastGame::startGame()
{
    startNewGame(true);

    // Start game loop
    while (app->isOpen())
    {
        // Process events
        sf::Event event;

        while (app->pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed)
            {
              if (gameState == gameStatePlaying && !player->isDead() && currentMap->isCleared()) saveGame();
              app->close();
            }

            if (event.type == sf::Event::MouseWheelMoved)
            {
              if (gameState == gameStatePlaying && !isPausing) player->selectNextShotType();
            }

            if (event.type == sf::Event::KeyPressed)
            {
              if (event.key.code == sf::Keyboard::Escape)
              {
                if (gameState == gameStatePlaying && !isPausing) isPausing = true;
                else if (gameState == gameStatePlaying && isPausing) isPausing = false;
              }

              if (event.key.code == input[KeyFireSelect] || event.key.code == inputAlt[KeyFireSelect])
              {
                if (gameState == gameStatePlaying && !isPausing) player->selectNextShotType();
              }

              if (event.key.code == input[KeyFire] || event.key.code == inputAlt[KeyFire])
              {
                if (gameState == gameStatePlaying && !isPausing) firingDirection = player->getFacingDirection();
              }

              if (event.key.code == sf::Keyboard::X)
              {
                startNewGame(false);
              }
            }

            if (event.type == sf::Event::LostFocus)
              isPausing = true;
        }

        if (gameState == gameStatePlaying && !isPausing)
        {
          if (player->canMove()) player->setVelocity(Vector2D(0.0f, 0.0f));

          if (sf::Keyboard::isKeyPressed(input[KeyLeft]) || sf::Keyboard::isKeyPressed(inputAlt[KeyLeft]))
          {
            if (sf::Keyboard::isKeyPressed(input[KeyUp]) || sf::Keyboard::isKeyPressed(inputAlt[KeyUp]))
              player->move(7);
            else if (sf::Keyboard::isKeyPressed(input[KeyDown]) || sf::Keyboard::isKeyPressed(inputAlt[KeyDown]))
              player->move(1);
            else
              player->move(4);
          }
          else if (sf::Keyboard::isKeyPressed(input[KeyRight]) || sf::Keyboard::isKeyPressed(inputAlt[KeyRight]))
          {
            if (sf::Keyboard::isKeyPressed(input[KeyUp]) || sf::Keyboard::isKeyPressed(inputAlt[KeyUp]))
              player->move(9);
            else if (sf::Keyboard::isKeyPressed(input[KeyDown]) || sf::Keyboard::isKeyPressed(inputAlt[KeyDown]))
              player->move(3);
            else
              player->move(6);
          }
          else if (sf::Keyboard::isKeyPressed(input[KeyUp]) || sf::Keyboard::isKeyPressed(inputAlt[KeyUp]))
          {
            player->move(8);
          }
          else if (sf::Keyboard::isKeyPressed(input[KeyDown]) || sf::Keyboard::isKeyPressed(inputAlt[KeyDown]))
          {
            player->move(2);
          }

          // normal 4 directions gameplay
          if (sf::Keyboard::isKeyPressed(input[KeyFireLeft]) || sf::Keyboard::isKeyPressed(inputAlt[KeyFireLeft]))
            player->fire(4);
          else if (sf::Keyboard::isKeyPressed(input[KeyFireRight]) || sf::Keyboard::isKeyPressed(inputAlt[KeyFireRight]))
            player->fire(6);
          else if (sf::Keyboard::isKeyPressed(input[KeyFireUp]) || sf::Keyboard::isKeyPressed(inputAlt[KeyFireUp]))
            player->fire(8);
          else if (sf::Keyboard::isKeyPressed(input[KeyFireDown]) || sf::Keyboard::isKeyPressed(inputAlt[KeyFireDown]))
            player->fire(2);
          // alternative "one button" gameplay
          else if (sf::Keyboard::isKeyPressed(input[KeyFire]) || sf::Keyboard::isKeyPressed(inputAlt[KeyFire]))
          {
            player->fire(firingDirection);
          }
          // alternative "firing with the mouse" gameplay
          else if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
          {
            sf::Vector2i mousePosition = sf::Mouse::getPosition(*app);
            int xm = mousePosition.x - player->getX();
            int ym = mousePosition.y - player->getY();

            if (abs(xm) >= abs(ym))
            {
              if (xm > 0) player->fire(6);
              else player->fire(4);
            }
            else
            {
              if (ym > 0) player->fire(2);
              else player->fire(8);
            }
          }

          if (player->isDead() && xGameState == xGameStateNone && sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
          {
            xGameState = xGameStateFadeOut;
            xGameTimer = FADE_OUT_DELAY;
          }
        }

        onUpdate();
        EntityManager::getEntityManager()->sortByZ();
        onRender();

        verifyDoorUnlocking();

        if (roomClosed)
        {
          if (getEnnemyCount() == 0)
          {
            currentMap->setCleared(true);
            openDoors();
            if (currentMap->getRoomType() == roomTypeBoss)
              playMusic(MusicDungeon);
          }
        }
    }
    quitGame();
}

void WitchBlastGame::createFloor()
{
  // TODO : extracts from createNewGame
}

void WitchBlastGame::closeDoors()
{
  if (!currentMap->isCleared())
  {
    int i;
    for(i = 0; i < MAP_WIDTH; i++)
    {
      if (currentMap->getTile(i, 0) < 4) currentMap->setTile(i, 0, MAP_DOOR);
      if (currentMap->getTile(i, MAP_HEIGHT - 1) < 4) currentMap->setTile(i, MAP_HEIGHT - 1, MAP_DOOR);
    }
    for(i = 0; i < MAP_HEIGHT; i++)
    {
      if (currentMap->getTile(0, i) < 4) currentMap->setTile(0, i, MAP_DOOR);
      if (currentMap->getTile(MAP_WIDTH - 1, i) < 4) currentMap->setTile(MAP_WIDTH - 1, i, MAP_DOOR);
    }
    roomClosed = true;
  }
}

void WitchBlastGame::openDoors()
{
    int i, j;
    for(i = 0; i < MAP_WIDTH; i++)
      for(j = 0; j < MAP_WIDTH; j++)
        if (currentMap->getTile(i, j) == MAP_DOOR) currentMap->setTile(i, j, MAP_DOOR_OPEN);
  roomClosed = false;
  SoundManager::getSoundManager()->playSound(SOUND_DOOR_OPENING);

  if (currentMap->hasNeighbourUp() == 2 && !bossRoomOpened)
    currentMap->setTile(MAP_WIDTH/2, 0, MAP_DOOR);
  else
    doorEntity[0]->openDoor();

  if (currentMap->hasNeighbourLeft() == 2 && !bossRoomOpened)
    currentMap->setTile(0, MAP_HEIGHT / 2, MAP_DOOR);
  else
    doorEntity[1]->openDoor();

  if (currentMap->hasNeighbourDown() == 2 && !bossRoomOpened)
    currentMap->setTile(MAP_WIDTH / 2, MAP_HEIGHT - 1, MAP_DOOR);
  else
    doorEntity[2]->openDoor();

  if (currentMap->hasNeighbourRight() == 2 && !bossRoomOpened)
    currentMap->setTile(MAP_WIDTH - 1, MAP_HEIGHT / 2, MAP_DOOR);
  else
    doorEntity[3]->openDoor();
}

int WitchBlastGame::getEnnemyCount()
{
  int n=0;

  EntityManager::EntityList* entityList =EntityManager::getEntityManager()->getList();
  EntityManager::EntityList::iterator it;

	for (it = entityList->begin (); it != entityList->end ();)
	{
		GameEntity *e = *it;
		it++;

		if (e->getType() >= 20)
		{
		  n++;
		}
	}

	return n;
}

void WitchBlastGame::refreshMap()
{
  // clean the sprites from old map
  EntityManager::getEntityManager()->partialClean(10);

  // if new map, it has to be randomized
  bool generateMap = !(currentFloor->getMap(floorX, floorY)->isVisited());
  currentMap = currentFloor->getAndVisitMap(floorX, floorY);

  // load the map
  currentTileMap->setMap(currentMap);
  player->setMap(currentMap, TILE_WIDTH, TILE_HEIGHT, OFFSET_X, OFFSET_Y);
  refreshMinimap();

  if(generateMap)
    this->generateMap();
  else
  {
    if (currentMap->getRoomType() == roomTypeMerchant)
      new PnjEntity(OFFSET_X + (MAP_WIDTH / 2) * TILE_WIDTH + TILE_WIDTH / 2,
                  OFFSET_Y + (MAP_HEIGHT / 2 - 2) * TILE_HEIGHT,
                  0);
  }

  // check doors
  doorEntity[0]->setVisible(currentMap->hasNeighbourUp() > 0);
  if (currentMap->hasNeighbourUp() == 1) doorEntity[0]->setDoorType(0);
  if (currentMap->hasNeighbourUp() == 2 || currentMap->getRoomType()==roomTypeBoss) doorEntity[0]->setDoorType(1);
  if (currentMap->hasNeighbourUp() == 2 && !bossRoomOpened)
  {
    doorEntity[0]->setOpen(false);
    currentMap->setTile(MAP_WIDTH/2, 0, MAP_DOOR);
  }
  else
    doorEntity[0]->setOpen(true);

  doorEntity[3]->setVisible(currentMap->hasNeighbourRight() > 0);
  if (currentMap->hasNeighbourRight() == 1) doorEntity[3]->setDoorType(0);
  if (currentMap->hasNeighbourRight() == 2 || currentMap->getRoomType()==roomTypeBoss) doorEntity[3]->setDoorType(1);
  if (currentMap->hasNeighbourRight() == 2 && !bossRoomOpened)
  {
    doorEntity[3]->setOpen(false);
    currentMap->setTile(MAP_WIDTH - 1, MAP_HEIGHT / 2, MAP_DOOR);
  }
  else
    doorEntity[3]->setOpen(true);

  doorEntity[2]->setVisible(currentMap->hasNeighbourDown() > 0);
  if (currentMap->hasNeighbourDown() == 1) doorEntity[2]->setDoorType(0);
  if (currentMap->hasNeighbourDown() == 2 || currentMap->getRoomType()==roomTypeBoss) doorEntity[2]->setDoorType(1);
  if (currentMap->hasNeighbourDown() == 2 && !bossRoomOpened)
  {
    doorEntity[2]->setOpen(false);
    currentMap->setTile(MAP_WIDTH/2, MAP_HEIGHT - 1, MAP_DOOR);
  }
  else
    doorEntity[2]->setOpen(true);

  doorEntity[1]->setVisible(currentMap->hasNeighbourLeft() > 0);
  if (currentMap->hasNeighbourLeft() == 1) doorEntity[1]->setDoorType(0);
  if (currentMap->hasNeighbourLeft() == 2 || currentMap->getRoomType()==roomTypeBoss) doorEntity[1]->setDoorType(1);
  if (currentMap->hasNeighbourLeft() == 2 && !bossRoomOpened)
  {
    doorEntity[1]->setOpen(false);
    currentMap->setTile(0, MAP_HEIGHT / 2, MAP_DOOR);
  }
  else
    doorEntity[1]->setOpen(true);
}

void WitchBlastGame::refreshMinimap()
{
  for (int j=0; j < FLOOR_HEIGHT; j++)
    for (int i=0; i < FLOOR_WIDTH; i++)
    {
      int n = currentFloor->getRoom(i, j);
      if (n > 0 && currentFloor->getMap(i, j)->isVisited())
      {
        if (currentFloor->getRoom(i, j) == roomTypeStarting
            || currentFloor->getRoom(i, j) == roomTypeBonus
            || currentFloor->getRoom(i, j) == roomTypeKey)
          miniMap->setTile(i, j, roomTypeStandard);
        else
          miniMap->setTile(i, j, currentFloor->getRoom(i, j));
      }
      else if (n > 0 && currentFloor->getMap(i, j)->isKnown())
      {
        if (currentFloor->getRoom(i, j) == roomTypeBoss)
          miniMap->setTile(i, j, 7);
        else
          miniMap->setTile(i, j, 9);
      }
      else
        miniMap->setTile(i, j, 0);
    }
  miniMap->setTile(floorX, floorY, 8);
}

void WitchBlastGame::checkEntering()
{
  if (!currentMap->isCleared())
  {
    player->setEntering();
    SoundManager::getSoundManager()->playSound(SOUND_DOOR_CLOSING);
    for (int i=0; i<4; i++)
      doorEntity[i]->closeDoor();
  }
}

void WitchBlastGame::saveMapItems()
{
  EntityManager::EntityList* entityList = EntityManager::getEntityManager()->getList();
  EntityManager::EntityList::iterator it;

	for (it = entityList->begin (); it != entityList->end ();)
	{
		GameEntity* e = *it;
		it++;

    ItemEntity* itemEntity = dynamic_cast<ItemEntity*>(e);
    ChestEntity* chestEntity = dynamic_cast<ChestEntity*>(e);

		if (itemEntity != NULL)
		{
      currentMap->addItem(itemEntity->getItemType(), itemEntity->getX(), itemEntity->getY(), itemEntity->getMerchandise());
		} // endif
		else if (chestEntity != NULL)
		{
      currentMap->addChest(chestEntity->getChestType(), chestEntity->getOpened(), chestEntity->getX(), chestEntity->getY());
		} // endif
		else
    {
      SpriteEntity* spriteEntity = dynamic_cast<SpriteEntity*>(e);
      if (spriteEntity != NULL && (e->getType() == ENTITY_BLOOD || e->getType() == ENTITY_CORPSE ) )
      {
        int spriteFrame = spriteEntity->getFrame();
        if (spriteEntity->getWidth() == 128) spriteFrame += FRAME_CORPSE_KING_RAT;
        currentMap->addSprite(e->getType(), spriteFrame, e->getX(), e->getY(), spriteEntity->getScaleX());
      }

    }
	} // end for
}

void WitchBlastGame::moveToOtherMap(int direction)
{
  // stairs to next level
  if (direction == 8 && currentMap->getRoomType() == roomTypeExit)
  {
    if (player->getPlayerStatus() != PlayerEntity::playerStatusGoingUp)
    {
      player->setLeavingLevel();
      xGameState = xGameStateFadeOut;
      xGameTimer = FADE_OUT_DELAY;
      player->setVelocity(Vector2D(0.0f, - INITIAL_PLAYER_SPEED / 2));
    }
  }
  // go to another room
  else
  {
    saveMapItems();
    switch (direction)
    {
      case (4): floorX--;  player->moveTo((OFFSET_X + MAP_WIDTH * TILE_WIDTH), player->getY()); player->move(4);  break;
      case (6): floorX++;  player->moveTo(OFFSET_X, player->getY()); player->move(6); break;
      case (8): floorY--;  player->moveTo(player->getX(), OFFSET_Y + MAP_HEIGHT * TILE_HEIGHT - 20); player->move(8); break;
      case (2): floorY++;  player->moveTo(player->getX(), OFFSET_Y);  break;
    }
    refreshMap();
    checkEntering();
    currentMap->restoreMapObjects();
  }
}

void WitchBlastGame::onRender()
{
    // clear the view
    app->clear(sf::Color(32, 32, 32));


    if (xGameState == xGameStateShake)
    {
      sf::View view = app->getDefaultView();
      sf::View viewSave = app->getDefaultView();
      view.move(-4 + rand() % 9, -4 + rand() % 9);
      app->setView(view);

      EntityManager::getEntityManager()->renderUnder(app, 5000);

      app->setView(viewSave);
      EntityManager::getEntityManager()->renderAfter(app, 5000);
    }
    else
    {
      // render the game objects
      EntityManager::getEntityManager()->render(app);
    }

    myText.setColor(sf::Color(255, 255, 255, 255));

    myText.setCharacterSize(18);
    std::ostringstream oss;
    oss << player->getGold();
    myText.setString(oss.str());
    myText.setPosition(690, 612);
    app->draw(myText);

    myText.setColor(sf::Color(0, 0, 0, 255));
    myText.setCharacterSize(16);

    oss.str("");
    oss << "Level " << level;
    myText.setString(oss.str());
    myText.setPosition(410, 692);
    app->draw(myText);

    sf::RectangleShape rectangle(sf::Vector2f(200, 25));
    // life

    if (gameState == gameStatePlaying)
    {
      // life and mana
      rectangle.setFillColor(sf::Color(190, 20, 20));
      rectangle.setPosition(sf::Vector2f(90, 622));
      rectangle.setSize(sf::Vector2f(200.0f * (float)(player->getHpDisplay()) / (float)(player->getHpMax()) , 25));
      app->draw(rectangle);

      rectangle.setFillColor(sf::Color(255, 190, 190));
      rectangle.setPosition(sf::Vector2f(90, 625));
      rectangle.setSize(sf::Vector2f(200.0f * (float)(player->getHpDisplay()) / (float)(player->getHpMax()) , 2));
      app->draw(rectangle);

      rectangle.setFillColor(sf::Color(20, 20, 190));
      rectangle.setPosition(sf::Vector2f(90, 658));
      rectangle.setSize(sf::Vector2f(200.0f * player->getPercentFireDelay() , 25));
      app->draw(rectangle);

      rectangle.setFillColor(sf::Color(190, 190, 255));
      rectangle.setPosition(sf::Vector2f(90, 661));
      rectangle.setSize(sf::Vector2f(200.0f * player->getPercentFireDelay() , 2));
      app->draw(rectangle);

      // drawing the key on the interface
      if (player->isEquiped(EQUIP_BOSS_KEY)) app->draw(keySprite);

      // render the shots
      renderHudShots(app);

      if (isPausing)
      {
        rectangle.setFillColor(sf::Color(0, 0, 0, 160));
        rectangle.setPosition(sf::Vector2f(OFFSET_X, OFFSET_Y));
        rectangle.setSize(sf::Vector2f(MAP_WIDTH * TILE_WIDTH, MAP_HEIGHT * TILE_HEIGHT));
        app->draw(rectangle);

        float x0 = OFFSET_X + (MAP_WIDTH / 2) * TILE_WIDTH + TILE_WIDTH / 2;
        int fade = 50 + 205 * (1.0f + cos(3.0f * getAbsolutTime())) * 0.5f;
        myText.setColor(sf::Color(255, 255, 255, fade));
        myText.setCharacterSize(40);
        myText.setString("PAUSE");
        myText.setPosition(x0 - myText.getLocalBounds().width / 2, 300);
        app->draw(myText);
      }

      if (player->isDead())
      {
        float x0 = OFFSET_X + (MAP_WIDTH / 2) * TILE_WIDTH + TILE_WIDTH / 2;
        int fade = 255 * (1.0f + cos(2.0f * getAbsolutTime())) * 0.5f;

        myText.setColor(sf::Color(255, 255, 255, 255));
        myText.setCharacterSize(25);
        myText.setString("GAME OVER");
        myText.setPosition(x0 - myText.getLocalBounds().width / 2, 400);
        app->draw(myText);

        myText.setColor(sf::Color(255, 255, 255, fade));
        myText.setCharacterSize(20);
        myText.setString("Press [ENTER] to play again !");
        myText.setPosition(x0 - myText.getLocalBounds().width / 2, 440);
        app->draw(myText);
      }
      else if (currentMap->getRoomType() == roomTypeExit && level > 1)
      {
        float x0 = OFFSET_X + (MAP_WIDTH / 2) * TILE_WIDTH + TILE_WIDTH / 2;
        myText.setColor(sf::Color(255, 255, 255, 255));
        myText.setCharacterSize(25);
        myText.setString("CONGRATULATIONS !\nYou've challenged this demo and\nmanaged to kill the boss !\nSee you soon for new adventures !");
        myText.setPosition(x0 - myText.getLocalBounds().width / 2, 220);
        app->draw(myText);
      }

      if (xGameState == xGameStateFadeIn)
      {
        // fade in
        rectangle.setFillColor(sf::Color(0, 0, 0, 255 - ((FADE_IN_DELAY - xGameTimer) / FADE_IN_DELAY) * 255));
        rectangle.setPosition(sf::Vector2f(OFFSET_X, OFFSET_Y));
        rectangle.setSize(sf::Vector2f(MAP_WIDTH * TILE_WIDTH , MAP_HEIGHT * TILE_HEIGHT));
        app->draw(rectangle);
      }
      else if (xGameState == xGameStateFadeOut)
      {
        // fade out
        rectangle.setFillColor(sf::Color(0, 0, 0, ((FADE_IN_DELAY - xGameTimer) / FADE_IN_DELAY) * 255));
        rectangle.setPosition(sf::Vector2f(OFFSET_X, OFFSET_Y));
        rectangle.setSize(sf::Vector2f(MAP_WIDTH * TILE_WIDTH , MAP_HEIGHT * TILE_HEIGHT));
        app->draw(rectangle);
      }
    }

    app->display();
}

void WitchBlastGame::renderHudShots(sf::RenderTarget* app)
{
  int xHud = 640;
  int yHud = 650;
  int index = 0;

  for (int i = 0; i < SPECIAL_SHOT_SLOTS; i++)
  {
    if (i == 0 || player->getShotType(i) != ShotTypeStandard)
    {
      int type_shot = player->getShotType(i);
      shotsSprite.setPosition(xHud + 48 * index, yHud);
      if (index == player->getShotIndex())
      {
        shotsSprite.setTextureRect(sf::IntRect(0, 0,  48, 48));
        app->draw(shotsSprite);
      }
      shotsSprite.setTextureRect(sf::IntRect(48 * ( 1 + type_shot), 0,  48, 48));
      app->draw(shotsSprite);
      index++;
    }
  }
}

void WitchBlastGame::generateBlood(float x, float y, BaseCreatureEntity::enumBloodColor bloodColor)
{
  // double blood if the "Blood Snake3 object is equipped
  int nbIt;
  if (player->isEquiped(EQUIP_BLOOD_SNAKE))
    nbIt = 2;
  else
    nbIt = 1;

  for (int i=0; i < nbIt; i++)
  {
    SpriteEntity* blood = new SpriteEntity(ImageManager::getImageManager()->getImage(IMAGE_BLOOD), x, y, 16, 16, 6);
    blood->setZ(OFFSET_Y - 1);
    int b0 = 0;
    if (bloodColor == BaseCreatureEntity::bloodGreen) b0 += 6;
    blood->setFrame(b0 + rand()%6);
    blood->setType(ENTITY_BLOOD);
    blood->setVelocity(Vector2D(rand()%250));
    blood->setViscosity(0.95f);

    float bloodScale = 1.0f + (rand() % 10) * 0.1f;
    blood->setScale(bloodScale, bloodScale);
  }
}

void WitchBlastGame::showArtefactDescription(enumItemType itemType)
{
  new ArtefactDescriptionEntity(itemType); //, &font);
}

void WitchBlastGame::generateMap()
{
  if (currentMap->getRoomType() == roomTypeStandard)
    generateStandardMap();
  else if (currentMap->getRoomType() == roomTypeBonus)
  {
    currentMap->setCleared(true);
    Vector2D v = currentMap->generateBonusRoom();
    int bonusType = getRandomEquipItem(false);
    if (bonusType == EQUIP_FAIRY)
    {
      new ChestEntity(v.x, v.y, CHEST_FAIRY, false);
    }
    else
    {
      new ItemEntity( (enumItemType)(FirstEquipItem + bonusType), v.x ,v.y);
    }
  }
  else if (currentMap->getRoomType() == roomTypeKey)
  {
    Vector2D v = currentMap->generateKeyRoom();
    new ItemEntity( (enumItemType)(ItemBossKey), v.x ,v.y);
    initMonsterArray();
    int x0 = MAP_WIDTH / 2;
    int y0 = MAP_HEIGHT / 2;
    monsterArray[x0][y0] = true;
    findPlaceMonsters(MONSTER_RAT, 5);
    findPlaceMonsters(MONSTER_BAT, 5);
  }
  else if (currentMap->getRoomType() == roomTypeMerchant)
  {
    currentMap->generateMerchantRoom();

    ItemEntity* item1 = new ItemEntity(
      itemHealth,
      OFFSET_X + (MAP_WIDTH / 2 - 3) * TILE_WIDTH + TILE_WIDTH / 2,
      OFFSET_Y + (MAP_HEIGHT / 2) * TILE_HEIGHT);
    item1->setMerchandise(true);

    ItemEntity* item3 = new ItemEntity(
      itemHealth,
      OFFSET_X + (MAP_WIDTH / 2 + 3) * TILE_WIDTH + TILE_WIDTH / 2,
      OFFSET_Y + (MAP_HEIGHT / 2) * TILE_HEIGHT);
    item3->setMerchandise(true);

    int bonusType = getRandomEquipItem(true);
    ItemEntity* item2 = new ItemEntity(
      (enumItemType)(FirstEquipItem + bonusType),
      OFFSET_X + (MAP_WIDTH / 2) * TILE_WIDTH + TILE_WIDTH / 2,
      OFFSET_Y + (MAP_HEIGHT / 2) * TILE_HEIGHT);
    item2->setMerchandise(true);

    new PnjEntity(OFFSET_X + (MAP_WIDTH / 2) * TILE_WIDTH + TILE_WIDTH / 2,
                  OFFSET_Y + (MAP_HEIGHT / 2 - 2) * TILE_HEIGHT,
                  0);

    currentMap->setCleared(true);
  }
  else if (currentMap->getRoomType() == roomTypeBoss)
  {
    currentMap->generateRoom(0);

    if (level == 1)
      new GiantSlimeEntity(OFFSET_X + (MAP_WIDTH / 2 - 2) * TILE_WIDTH + TILE_WIDTH / 2,
                      OFFSET_Y + (MAP_HEIGHT / 2 - 2) * TILE_HEIGHT + TILE_HEIGHT / 2);
    else
      new KingRatEntity(OFFSET_X + (MAP_WIDTH / 2 - 2) * TILE_WIDTH + TILE_WIDTH / 2,
                      OFFSET_Y + (MAP_HEIGHT / 2 - 2) * TILE_HEIGHT + TILE_HEIGHT / 2);

    playMusic(MusicBoss);
  }
  else if (currentMap->getRoomType() == roomTypeStarting)
  {
    currentMap->generateRoom(0);
    currentMap->setCleared(true);
    int bonusType = getRandomEquipItem(false);
    if (level == 1)
    {
      if (bonusType == EQUIP_FAIRY)
      {
        new ChestEntity(OFFSET_X + (TILE_WIDTH * MAP_WIDTH * 0.5f),
                                             OFFSET_Y + 120.0f + (TILE_HEIGHT * MAP_HEIGHT * 0.5f),
                                             CHEST_FAIRY, false);
      }
      else
      {
        new ItemEntity( (enumItemType)(FirstEquipItem + bonusType),
                            OFFSET_X + (TILE_WIDTH * MAP_WIDTH * 0.5f),
                            OFFSET_Y + 120.0f + (TILE_HEIGHT * MAP_HEIGHT * 0.5f));
      }
    }
  }
  else if (currentMap->getRoomType() == roomTypeExit)
  {
    currentMap->generateExitRoom();
    currentMap->setCleared(true);

    new ChestEntity(OFFSET_X + (TILE_WIDTH * MAP_WIDTH * 0.5f),
                    OFFSET_Y + (TILE_HEIGHT * MAP_HEIGHT * 0.5f),
                    CHEST_EXIT, false);
  }
  else
    currentMap->randomize(currentMap->getRoomType());
}

void WitchBlastGame::Write(std::string str, int size, float x, float y, int align, sf::Color color, sf::RenderTarget* app)
{
  myText.setString(str);
  myText.setCharacterSize(size);
  myText.setColor(color);

  if (align == ALIGN_CENTER)
    myText.setPosition(x - myText.getLocalBounds().width / 2, y);
  else
    myText.setPosition(x, y);

  app->draw(myText);
}

void WitchBlastGame::initMonsterArray()
{
  for (int i = 0; i < MAP_WIDTH; i++)
    for (int j = 0; j < MAP_HEIGHT; j++)
      monsterArray[i][j] = false;
}

void WitchBlastGame::addMonster(monster_type_enum monsterType, float xm, float ym)
{
  switch (monsterType)
  {
    case MONSTER_RAT: new RatEntity(xm, ym - 2); break;
    case MONSTER_BLACK_RAT: new BlackRatEntity(xm, ym - 5); break;
    case MONSTER_BAT: new BatEntity(xm, ym); break;
    case MONSTER_EVIL_FLOWER: new EvilFlowerEntity(xm, ym - 2); break;
    case MONSTER_SLIME: new SlimeEntity(xm, ym, false); break;

    case MONSTER_KING_RAT: new KingRatEntity(xm, ym); break;
  }
}

void WitchBlastGame::findPlaceMonsters(monster_type_enum monsterType, int amount)
{
  // find a suitable place
  bool isMonsterFlying = monsterType == MONSTER_BAT;

  bool bOk;
  int xm, ym;
  float xMonster, yMonster;

  for (int index = 0; index < amount; index++)
  {
    bOk = false;

    while (!bOk)
    {
      bOk = true;
      xm = 1 +rand() % (MAP_WIDTH - 3);
      ym = 1 +rand() % (MAP_HEIGHT - 3);
      if (monsterArray[xm][ym])
      {
        bOk = false;
      }
      if (bOk && !isMonsterFlying && !currentMap->isWalkable(xm, ym))
      {
        bOk = false;
      }
      if (bOk)
      {
        xMonster = OFFSET_X + xm * TILE_WIDTH + TILE_WIDTH * 0.5f;
        yMonster = OFFSET_Y + ym * TILE_HEIGHT+ TILE_HEIGHT * 0.5f;

        float dist2 = (xMonster - player->getX())*(xMonster - player->getX()) + (yMonster - player->getY())*(yMonster - player->getY());
        if ( dist2 < 75000.0f)
        {
          bOk = false;
        }
        else
        {
          addMonster(monsterType, xMonster, yMonster);
          monsterArray[xm][ym] = true;
        }
      }
    }
  }
}

void WitchBlastGame::generateStandardMap()
{
  initMonsterArray();

  int random = rand() % (level == 1 ? 95 : 110);

  if (random < 16)
  {
    currentMap->generateRoom(rand()%4);
    findPlaceMonsters(MONSTER_RAT,4);
  }
  else if (random < 32)
  {
    currentMap->generateRoom(rand()%5);
    findPlaceMonsters(MONSTER_BAT,4);
  }
  else if (random < 48)
  {
    currentMap->generateRoom(rand()%5);
    findPlaceMonsters(MONSTER_EVIL_FLOWER,4);
  }
  else if (random < 64)
  {
    Vector2D v = currentMap->generateBonusRoom();
    new ChestEntity(v.x, v.y, CHEST_BASIC, false);
    currentMap->setCleared(true);
  }
  else if (random < 80)
  {
    currentMap->generateRoom(rand()%4);
    findPlaceMonsters(MONSTER_RAT,3);
    findPlaceMonsters(MONSTER_BAT,3);
  }
  else if (random < 95)
  {
    currentMap->generateRoom(4);
    findPlaceMonsters(MONSTER_BLACK_RAT,6);
  }
  else
  {
    currentMap->generateRoom(rand()%4);
    currentMap->addRandomGrids(4);
    findPlaceMonsters(MONSTER_SLIME,8 + rand() % 5);
  }
}

item_equip_enum WitchBlastGame::getRandomEquipItem(bool toSale = false)
{
  std::vector<int> bonusSet;
  int setSize = 0;
  for (int i = 0; i < NUMBER_EQUIP_ITEMS; i++)
  {
    bool itemOk = true;
    int eq = i + FirstEquipItem;

    if (player->isEquiped(i)) itemOk = false;
    // TODO item already in floor
    if (itemOk && toSale && !items[eq].canBeSold) itemOk = false;
    if (itemOk && !toSale && !items[eq].canBeFound) itemOk = false;
    if (itemOk && items[eq].level > level) itemOk = false;
    if (itemOk && items[eq].requirement >= FirstEquipItem
        && !player->isEquiped(i)) itemOk = false;

    if (itemOk && player->getShotType(SPECIAL_SHOT_SLOTS_STANDARD) != ShotTypeStandard
        && (items[eq].specialShot != ShotTypeStandard && items[eq].level < 4))
          itemOk = false;

    if (itemOk)
    {
      int n = 0;
      switch (items[eq].rarity)
      {
        case RarityCommon:  n = 4; break;
        case RarityUnommon: n = 2; break;
        case RarityRare:    n = 1; break;
      }
      for (int j = 0; j < n; j++)
      {
        bonusSet.push_back(i);
        setSize++;
      }
    }
  }
  int bonusType = 0;
  if (setSize > 0) bonusType = bonusSet[rand() % setSize];

  return (item_equip_enum) bonusType;
}

void WitchBlastGame::verifyDoorUnlocking()
{
  int collidingDirection = (player->getCollidingDirection());

  if (collidingDirection > 0 && currentMap->isCleared() && !bossRoomOpened && player->isEquiped(EQUIP_BOSS_KEY))
  {
    int xt = (player->getX() - OFFSET_X) / TILE_WIDTH;
    int yt = (player->getY() - OFFSET_Y) / TILE_HEIGHT;

    if (yt <= 1 && xt >= MAP_WIDTH / 2 - 1 && xt <= MAP_WIDTH / 2 + 1 && currentMap->hasNeighbourUp() == 2)
    {
      doorEntity[0]->openDoor();
      currentMap->setTile(MAP_WIDTH / 2, 0, 0);
      SoundManager::getSoundManager()->playSound(SOUND_DOOR_OPENING);
      player->useBossKey();
      bossRoomOpened = true;
    }
    if (yt >= MAP_HEIGHT - 2 && xt >= MAP_WIDTH / 2 - 1 &&xt <= MAP_WIDTH / 2 + 1 && currentMap->hasNeighbourDown() == 2)
    {
      doorEntity[2]->openDoor();
      currentMap->setTile(MAP_WIDTH / 2, MAP_HEIGHT - 1, 0);
      SoundManager::getSoundManager()->playSound(SOUND_DOOR_OPENING);
      player->useBossKey();
      bossRoomOpened = true;
    }
    if (xt <= 1 && yt >= MAP_HEIGHT / 2 - 1 && yt <= MAP_HEIGHT / 2 + 1 && currentMap->hasNeighbourLeft() == 2)
    {

      doorEntity[1]->openDoor();
      currentMap->setTile(0, MAP_HEIGHT / 2, 0);
      SoundManager::getSoundManager()->playSound(SOUND_DOOR_OPENING);
      player->useBossKey();
      bossRoomOpened = true;
    }
    if (xt >= MAP_WIDTH - 2 && yt >= MAP_HEIGHT / 2 - 1 && yt <= MAP_HEIGHT / 2 + 1 && currentMap->hasNeighbourRight() == 2)
    {
      doorEntity[3]->openDoor();
      currentMap->setTile(MAP_WIDTH - 1, MAP_HEIGHT / 2, 0);
      SoundManager::getSoundManager()->playSound(SOUND_DOOR_OPENING);
      player->useBossKey();
      bossRoomOpened = true;
    }
  }
}

void WitchBlastGame::playMusic(musicEnum musicChoice)
{
  music.stop();
  music.setLoop(true);
  bool ok = false;

  switch (musicChoice)
  {
  case MusicDungeon:
    ok = music.openFromFile("media/sound/track00.ogg");
    music.setVolume(75);
    break;

  case MusicEnding:
    ok = music.openFromFile("media/sound/track_ending.ogg");
    music.setVolume(35);
    break;

  case MusicBoss:
    ok = music.openFromFile("media/sound/track_boss.ogg");
    music.setVolume(90);
    break;
  }

  if (ok)
    music.play();
}

void WitchBlastGame::makeShake(float duration)
{
  xGameState = xGameStateShake;
  xGameTimer = duration;
}

void WitchBlastGame::saveGame()
{
  ofstream file("game.sav", ios::out | ios::trunc);

  int i, j, k, l;

  if (file)
  {
    // floor
    file << level << std::endl;

    int nbRooms = 0;
    for (j = 0; j < FLOOR_HEIGHT; j++)
    {
      for (i = 0; i < FLOOR_WIDTH; i++)
      {
        file << currentFloor->getRoom(i,j) << " ";
        if (currentFloor->getRoom(i,j) > 0) nbRooms++;
      }
      file << std::endl;
    }

    // maps
    saveMapItems();

    file << nbRooms << std::endl;
    for (j = 0; j < FLOOR_HEIGHT; j++)
    {
      for (i = 0; i < FLOOR_WIDTH; i++)
      {
        if (currentFloor->getRoom(i,j) > 0)
        {
          file << i << " " << j << " "
          << currentFloor->getMap(i, j)->getRoomType() << " "
          << currentFloor->getMap(i, j)->isKnown() << " "
          << currentFloor->getMap(i, j)->isVisited() << " "
          << currentFloor->getMap(i, j)->isCleared() << std::endl;
          if (currentFloor->getMap(i, j)->isVisited())
          {
            for (l = 0; l < MAP_HEIGHT; l++)
            {
              for (k = 0; k < MAP_WIDTH; k++)
              {
                file << currentFloor->getMap(i, j)->getTile(k, l) << " ";
              }
              file << std::endl;
            }
            // items, etc...
            std::list<DungeonMap::itemListElement> itemList = currentFloor->getMap(i, j)->getItemList();
            file << itemList.size() << std::endl;
            std::list<DungeonMap::itemListElement>::iterator it;
            for (it = itemList.begin (); it != itemList.end ();)
            {
              DungeonMap::itemListElement ilm = *it;
              it++;

              file << ilm.type << " " << ilm.x << " " << ilm.y << " " << ilm.merch << std::endl;
            }

            // chests
            std::list<DungeonMap::chestListElement> chestList = currentFloor->getMap(i, j)->getChestList();
            file << chestList.size() << std::endl;
            std::list<DungeonMap::chestListElement>::iterator itc;
            for (itc = chestList.begin (); itc != chestList.end ();)
            {
              DungeonMap::chestListElement ilm = *itc;
              itc++;

              file << ilm.type << " " << ilm.x << " " << ilm.y << " " << ilm.state << std::endl;
            }

            // sprites
            std::list<DungeonMap::spriteListElement> spriteList = currentFloor->getMap(i, j)->getSpriteList();
            file << spriteList.size() << std::endl;
            std::list<DungeonMap::spriteListElement>::iterator its;
            for (its = spriteList.begin (); its != spriteList.end ();)
            {
              DungeonMap::spriteListElement ilm = *its;
              its++;

              file << ilm.type << " " << ilm.frame << " " << ilm.x << " " << ilm.y << " " << ilm.scale << std::endl;
            }
          }
        }
      }
      file << std::endl;
    }

    // game
    file << floorX << " " << floorY << std::endl;
    file << bossRoomOpened << std::endl;
    // boss door !

    // player
    file << player->getHp() << " " << player->getHpMax() << " " << player->getGold() << std::endl;
    for (i = 0; i < NUMBER_EQUIP_ITEMS; i++) file << player->isEquiped(i) << " ";
    file << std::endl;
    file << player->getX() << " " << player->getY() << std::endl;
    file << player->getShotIndex();
    for (i = 0; i < SPECIAL_SHOT_SLOTS; i++) file << " " << player->getShotType(i);

    file.close();
  }
  else
  {
    cerr << "[ERROR] Saving the game..." << endl;
  }
}

bool WitchBlastGame::loadGame()
{
  ifstream file("game.sav", ios::in);

  if (file)
  {
    int i, j, k, n;

    // floor
    file >> level;
    currentFloor = new GameFloor(level);
    for (j = 0; j < FLOOR_HEIGHT; j++)
    {
      for (i = 0; i < FLOOR_WIDTH; i++)
      {
        int n;
        file >> n;
        currentFloor->setRoom(i, j, n);
      }
    }

    // maps
    int nbRooms;
    file >> nbRooms;

    for (k = 0; k < nbRooms; k++)
    {
      file >> i;
      file >> j;
      file >> n;
      DungeonMap* iMap = new DungeonMap(currentFloor, i, j);
      currentFloor->setMap(i, j, iMap);
      iMap->setRoomType((roomTypeEnum)n);
      bool flag;
      file >> flag;
      iMap->setKnown(flag);
      file >> flag;
      iMap->setVisited(flag);
      file >> flag;
      iMap->setCleared(flag);

      if (iMap->isVisited())
      {
        for (j = 0; j < MAP_HEIGHT; j++)
        {
          for (i = 0; i < MAP_WIDTH; i++)
          {
            file >> n;
            iMap->setTile(i, j, n);
          }
        }
        // items int the map
        file >> n;
        for (i = 0; i < n; i++)
        {
          int t;
          float x, y;
          bool merc;
          file >> t >> x >> y >> merc;
          iMap->addItem(t, x, y, merc);
        }
        // chests in the map
        file >> n;
        for (i = 0; i < n; i++)
        {
          int t;
          float x, y;
          bool state;
          file >> t >> x >> y >> state;
          iMap->addChest(t, state, x, y);
        }
        // sprites in the map
        file >> n;
        for (i = 0; i < n; i++)
        {
          int t, f;
          float x, y, scale;
          file >> t >> f >> x >> y >> scale;
          iMap->addSprite(t, f, x, y, scale);
        }
      }
    }

    // game
    file >> floorX >> floorY;
    currentMap = currentFloor->getMap(floorX, floorY);
    file >> bossRoomOpened;

    // player
    int hp, hpMax, gold;
    file >> hp >> hpMax >> gold;
    player = new PlayerEntity(OFFSET_X + (TILE_WIDTH * MAP_WIDTH * 0.5f),
                              OFFSET_Y + (TILE_HEIGHT * MAP_HEIGHT * 0.5f));
    player->setHp(hp);
    player->setHpMax(hpMax);
    player->setGold(gold);
    for (i = 0; i < NUMBER_EQUIP_ITEMS; i++)
    {
      bool eq;
      file >> eq;
      player->setEquiped(i, eq);
    }
    float x, y;
    file >> x >> y;

    player->moveTo(x, y);


    file >> n;
    player->setShotIndex(n);

    for (i = 0; i < SPECIAL_SHOT_SLOTS; i++)
    {
      file >> n;
      player->setShotType(i, (enumShotType)n);
    }

    file.close();
    remove("game.sav");
  }
  else
  {
    return false;
  }

  return true;
}

void WitchBlastGame::addKey(int logicInput, std::string key, bool alt = false)
{
  int iKey = config.findInt(key);
  if (iKey >= 0)
  {

    sf::Keyboard::Key k = (sf::Keyboard::Key)iKey;
    if (alt)
      inputAlt[logicInput] = k;
    else
      input[logicInput] = k;
  }
}

void WitchBlastGame::configureFromFile()
{
  // default
  input[KeyUp]    = sf::Keyboard::Z;
  input[KeyDown]  = sf::Keyboard::S;
  input[KeyLeft]  = sf::Keyboard::Q;
  input[KeyRight] = sf::Keyboard::D;
  input[KeyFireUp]    = sf::Keyboard::Up;
  input[KeyFireDown]  = sf::Keyboard::Down;
  input[KeyFireLeft]  = sf::Keyboard::Left;
  input[KeyFireRight] = sf::Keyboard::Right;
  input[KeyFire] = sf::Keyboard::Space;
  input[KeyFireSelect] = sf::Keyboard::Tab;

  inputAlt[KeyUp]    = sf::Keyboard::W;
  inputAlt[KeyDown]  = sf::Keyboard::S;
  inputAlt[KeyLeft]  = sf::Keyboard::A;
  inputAlt[KeyRight] = sf::Keyboard::D;
  inputAlt[KeyFireUp]    = sf::Keyboard::Up;
  inputAlt[KeyFireDown]  = sf::Keyboard::Down;
  inputAlt[KeyFireLeft]  = sf::Keyboard::Left;
  inputAlt[KeyFireRight] = sf::Keyboard::Right;
  inputAlt[KeyFire] = sf::Keyboard::Space;
  inputAlt[KeyFireSelect] = sf::Keyboard::Tab;

  // from file
  addKey(KeyUp, "keyboard_move_up");
  addKey(KeyDown, "keyboard_move_down");
  addKey(KeyLeft, "keyboard_move_left");
  addKey(KeyRight, "keyboard_move_right");
  addKey(KeyFireUp, "keyboard_fire_up");
  addKey(KeyFireDown, "keyboard_fire_down");
  addKey(KeyFireLeft, "keyboard_fire_left");
  addKey(KeyFireRight, "keyboard_fire_right");
  addKey(KeyFire, "keyboard_fire");
  addKey(KeyFireSelect, "keyboard_fire_select");

  addKey(KeyUp, "keyboard_move_up_alt", true);
  addKey(KeyDown, "keyboard_move_down_alt", true);
  addKey(KeyLeft, "keyboard_move_left_alt", true);
  addKey(KeyRight, "keyboard_move_right_alt", true);
  addKey(KeyFireUp, "keyboard_fire_up_alt", true);
  addKey(KeyFireDown, "keyboard_fire_down_alt", true);
  addKey(KeyFireLeft, "keyboard_fire_left_alt", true);
  addKey(KeyFireRight, "keyboard_fire_right_alt", true);
  addKey(KeyFire, "keyboard_fire_alt", true);
  addKey(KeyFireSelect, "keyboard_fire_select_alt", true);
}

WitchBlastGame &game()
{
    return *gameptr;
}
