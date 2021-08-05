#pragma once

#include <Components/Printable.h>
#include <aunteater/Entity.h>

#include <string>


namespace ad {
namespace entities {


inline aunteater::Entity makePrint(std::string aString)
{
    return aunteater::Entity().add<Printable>(aString);
}

} // namespace entities
} // namespace ad
