#ifndef CAULDRON_ENTITY_H
#define CAULDRON_ENTITY_H

#include "EnemyEntity.h"

class CauldronEntity : public EnemyEntity
{
  public:
    CauldronEntity(float x, float y);
    virtual void animate(float delay);
    virtual void calculateBB();
  protected:
    virtual void collideMapRight();
    virtual void collideMapLeft();
    virtual void collideMapTop();
    virtual void collideMapBottom();

    virtual void collideWithEnnemy(GameEntity* collidingEntity);
    virtual void readCollidingEntity(CollidingSpriteEntity* entity);
    virtual void inflictsRecoilTo(BaseCreatureEntity* targetEntity);

	  virtual void dying();
	  virtual void drop();
  private:
    float invokeDelay, bubbleDelay;
};

#endif // CAULDRON_ENTITY_H
