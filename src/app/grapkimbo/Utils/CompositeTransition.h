#pragma once


#include <math/Interpolation.h>

#include <functional>
#include <vector>


namespace ad {
namespace grapito {


/// \brief Implement the implicit "transition concept" (i.e. ability to advance() by time increments).
/// encapsulating a sequence of interpolations.
///
/// Currently, the components might either be Interpolations as defined in math, or constant values.
/// The API was designed to be convenient and avoid discontinuities in values:
/// when adding a component, only its target and durations are specified, the starting value is implicit.
///
/// \warning It is an undefined behaviour to advance an empty composite transition.
// Note: Maybe always create a zero duration constant at the beginning ?
template <class T_value, class T_parameter>
class CompositeTransition
{
    struct Component
    {
        std::function<T_value(T_parameter)> mEvaluator;
        T_parameter mLocalEndTime; // End time on the timeline of the overall composite transition.
    };

public:
    CompositeTransition(T_value aInitial) :
        mLastTarget{std::move(aInitial)}
    {}

    template <template <class> class TT_easeFunctor>
    CompositeTransition & pushInterpolation(const T_value & aTarget, T_parameter aDuration)
    {
        mComponents.push_back(Component{
            [evaluator = math::makeInterpolation<TT_easeFunctor>(mLastTarget, aTarget, aDuration)]
            (T_parameter aLocalInterpolationTime) mutable
            {
                return evaluator.advance(aLocalInterpolationTime);
            },
            getLastEndTime() + aDuration
        });
        mLastTarget = aTarget;
        return *this;
    }

    CompositeTransition & pushConstant(T_parameter aDuration)
    {
        mComponents.push_back({
            [constant = mLastTarget]
            (T_parameter)
            {
                return constant;
            },
            getLastEndTime() + aDuration
        });
        // mLastTarget is inchanged, since it remains... constant
        return *this;
    }

    T_value advance(T_parameter aIncrement)
    {
        mLocalTime += aIncrement;
        while (!isCompleted() && current().mLocalEndTime < mLocalTime)
        {
            // The parameter increment for the next component is only the time spent since
            // the start of its time window.
            aIncrement = mLocalTime - current().mLocalEndTime;
            advanceComponent();
        }
        return current().mEvaluator(aIncrement);
    }


    bool isCompleted() const
    {
        return mCompleted;
    }


private:
    T_parameter getLastEndTime()
    {
        if (mComponents.size())
        {
            return mComponents.back().mLocalEndTime;
        }
        return T_parameter{0};
    }

    Component & current()
    {
        return mComponents[mCurrent];
    }

    void advanceComponent()
    {
        ++mCurrent;
        if (mCurrent >= mComponents.size())
        {
            mCompleted = true;
            --mCurrent;
        }
    }

    T_value mLastTarget;
    std::vector<Component> mComponents;
    typename std::vector<Component>::size_type mCurrent{0};
    T_parameter mLocalTime{0};
    bool mCompleted{false};
};


} // namespace grapito
} // namespace ad
