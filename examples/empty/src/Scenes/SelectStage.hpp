// SelectStage.hpp
#pragma once
#include "Common.hpp"

class SelectStage : public App::Scene
{
public:
    SelectStage(const InitData& init);
    ~SelectStage();

    void update() override;
    void draw() const override;

private:
    Texture textureStageOden;
    Texture textureComingSoon;
    const Audio bgm{U"example/sound/selectstage.mp3", Loop::Yes};
    const Audio cursor{U"example/sound/cursor.mp3"};
    const Audio confirm{U"example/sound/confirm.mp3"};
};
