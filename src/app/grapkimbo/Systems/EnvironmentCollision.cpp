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

        math::Rectangle<double> collTranslatedCollisionBox{
            collider->get<Position>().position + collider->get<EnvironmentCollisionBox>().box.mPosition.as<math::Vec>(),
            collider->get<EnvironmentCollisionBox>().box.mDimension
        };

        for(auto environment : mEnvironments)
        {
            math::Rectangle<double> envTranslatedCollisionBox{
                environment->get<Position>().position + environment->get<EnvironmentCollisionBox>().box.mPosition.as<math::Vec>(),
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

