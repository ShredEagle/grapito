#include "EnvironmentCollision.h"

namespace ad {

EnvironmentCollision::EnvironmentCollision(aunteater::Engine & aEngine) :
    mEngine(aEngine),
    mColliders(mEngine),
    mEnvironments(mEngine)
{}


void EnvironmentCollision::update(const aunteater::Timer aTimer)
{
    for(auto environment : mEnvironments)
    {
        environment->get<EnvironmentCollisionBox>().collidingWith.clear();
    }

    for(auto collider : mColliders)
    {
        collider->get<EnvironmentCollisionBox>().collidingWith.clear();

        math::Rectangle<int> collTranslatedCollisionBox{
            collider->get<Position>().position + static_cast<math::Vec<2, int>>(collider->get<EnvironmentCollisionBox>().box.mPosition),
            collider->get<EnvironmentCollisionBox>().box.mDimension
        };

        for(auto environment : mEnvironments)
        {
            math::Rectangle<int> envTranslatedCollisionBox{
                environment->get<Position>().position + static_cast<math::Vec<2,int>>(environment->get<EnvironmentCollisionBox>().box.mPosition),
                environment->get<EnvironmentCollisionBox>().box.mDimension
            };

            if (
                    envTranslatedCollisionBox.contains(collTranslatedCollisionBox.bottomLeft())
                    || envTranslatedCollisionBox.contains(collTranslatedCollisionBox.bottomRight())
                    || envTranslatedCollisionBox.contains(collTranslatedCollisionBox.topLeft())
                    || envTranslatedCollisionBox.contains(collTranslatedCollisionBox.topRight())
                    || collTranslatedCollisionBox.contains(envTranslatedCollisionBox.bottomLeft())
                    || collTranslatedCollisionBox.contains(envTranslatedCollisionBox.bottomRight())
                    || collTranslatedCollisionBox.contains(envTranslatedCollisionBox.topLeft())
                    || collTranslatedCollisionBox.contains(envTranslatedCollisionBox.topRight())
            )
            {
                collider->get<EnvironmentCollisionBox>().collidingWith.push_back(environment);
                environment->get<EnvironmentCollisionBox>().collidingWith.push_back(collider);
            }
        }
    }
}
} // namespace ad

