// Story.cpp
#include "Story.hpp"

Story::Story(const InitData& init) : IScene{ init }{}

Story::~Story(){}

void Story::update()
{
    if (storyAnimation.posSec() >= storyAnimation.lengthSec() or getData().p1_input.Confirm.down() or getData().p1_input.Up.down() or getData().p1_input.Down.down() or getData().p1_input.Left.down() or getData().p1_input.Right.down())
    {
        changeScene(State::SelectEquipment);
    }
}

void Story::draw() const
{
    storyAnimation.advance();
    storyAnimation.scaled(0.68).drawAt(Scene::Center());
}

