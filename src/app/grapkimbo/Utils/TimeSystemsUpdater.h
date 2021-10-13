#pragma once


#include <aunteater/System.h> 

#include <ostream>

#include <chrono>


namespace ad {
namespace grapito {


template <class ... VT_inputState>
class TimeSystemsUpdater
{
    template <class T>
    std::string getTypeName(const T & aObject)
    {
        return boost::core::demangle(typeid(aObject).name());
    }

public:
    void start()
    {
        pre = initial = std::chrono::steady_clock::now();
    }

    void operator()(aunteater::System<VT_inputState...> & aSystem,
                    const aunteater::Timer aTime,
                    const VT_inputState & ... aInputState)
    {
        aSystem.update(aTime, aInputState...);
        const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        mTimings[getTypeName(aSystem)].addSample(
             std::chrono::duration_cast<std::chrono::microseconds>(now - pre));
        pre = now;
    }

    void finish()
    {
        const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        mTimings["EntityManager::removeEntities()"].addSample(
             std::chrono::duration_cast<std::chrono::microseconds>(now - pre));

        mTimings["Update Total"].addSample(
             std::chrono::duration_cast<std::chrono::microseconds>(now - initial));
    }

    void outputTimings(std::ostream &os) const;

private:
    struct SystemStats
    {
        void addSample(std::chrono::microseconds aDuration)
        {
            latest = aDuration;
            min = std::min(min, latest);
            max = std::max(max, latest);
            average = (average * (double)sampleCount / (sampleCount + 1))
                      + (double)latest.count() / (sampleCount + 1);
            ++sampleCount;
        }

        int sampleCount{0};
        std::chrono::microseconds latest{0};
        // Need to use the representation type,
        // see: https://stackoverflow.com/a/35575526/1027706
        std::chrono::microseconds min{
            std::numeric_limits<std::chrono::microseconds::rep>::max()};
        std::chrono::microseconds max{0};
        double average{0.};
    };

    std::chrono::steady_clock::time_point initial;
    std::chrono::steady_clock::time_point pre;

    std::map<std::string, SystemStats> mTimings;
};


template <class ... VT_inputState>
void TimeSystemsUpdater<VT_inputState...>::outputTimings(std::ostream & os) const
{
    for(auto timing : mTimings)
    {
        os << timing.first << ": " << timing.second.latest.count() << "us"
           << "(Average: " << timing.second.average
           <<" L: " << timing.second.min.count()
           <<" H: " << timing.second.max.count() << ")\n"
           ;
    }
}


} // namespace grapito
} // namespace ad
