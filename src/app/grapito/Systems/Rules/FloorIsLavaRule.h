#pragma once

#include "RuleBase.h"

#include <array>


namespace ad {
namespace grapito {


class FloorIsLavaRule : public RuleBase
{
    enum Phase
    {
        InitialInvicible,
        FloorIsLava, 
        Death,
        FadeOut,
        FadeIn,

        _End/*
    */};

    using PhasesArray = std::array<std::shared_ptr<State>, _End>;

    friend PhasesArray setupGamePhases(std::shared_ptr<Context>, FloorIsLavaRule &);

    template <class>
    friend class PhaseBase;
    template <class>
    friend class MessagePhase;
    friend class InitialInviciblePhase;
    friend class FloorIsLavaPhase;
    friend class LavaFadeOutPhase;
    friend class LavaFadeInPhase;

public:
    FloorIsLavaRule(aunteater::EntityManager & aEntityManager,
                    std::shared_ptr<Context> aContext,
                    std::shared_ptr<Control> aControlSystem,
                    std::shared_ptr<RenderToScreen> aRenderToScreenSystem);

    void update(const GrapitoTimer aTimer, const GameInputState & aInput) override;

    aunteater::weak_entity player()
    {
        return mPlayer;
    }

private:
    void addPlayer();
    void resetPlayer();

    void setHeight(int aHeight);

    aunteater::weak_entity mPlayer{nullptr};
    aunteater::weak_entity mHudHeightText{nullptr};
    aunteater::weak_entity mHudBestScoreText{nullptr};

    PhasesArray mPhases;
    StateMachine mPhaseMachine;

    int mBestHeight{0};
};


} // namespace grapito
} // namespace ad
