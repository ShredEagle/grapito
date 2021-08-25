#include "EnvironmentCollision.h"

namespace ad {

EnvironmentCollision::EnvironmentCollision(aunteater::EntityManager & aEntityManager) :
    mEntityManager{aEntityManager},
    mColliders{mEntityManager},
    mEnvironments{mEntityManager}
{}


void EnvironmentCollision::update(const aunteater::Timer aTimer, const GameInputState &)
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
                    envTranslatedCollisionBox.x() < collTranslatedCollisionBox.x() + collTranslatedCollisionBox.width()
                    && envTranslatedCollisionBox.x() + envTranslatedCollisionBox.width() > collTranslatedCollisionBox.x()
                    && envTranslatedCollisionBox.y() < collTranslatedCollisionBox.y() + collTranslatedCollisionBox.height()
                    && envTranslatedCollisionBox.y() + envTranslatedCollisionBox.height() > collTranslatedCollisionBox.y()
            )
            {
                collider->get<EnvironmentCollisionBox>().collidingWith.push_back(environment);
                environment->get<EnvironmentCollisionBox>().collidingWith.push_back(collider);
            }
        }
    }
}
} // namespace ad

