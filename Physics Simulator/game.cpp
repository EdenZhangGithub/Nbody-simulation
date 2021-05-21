/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "body.h"
#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"

#include <vector>


// Game-related State data
SpriteRenderer* Renderer;
std::vector<Body> Bodies;


Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
    delete Renderer;
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Shader shader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(shader);
    // load textures
    ResourceManager::LoadTexture("textures/eden_ball.png", true, "body");

    Bodies.emplace_back(glm::vec2(124.0f, 210.0f), glm::vec2(10.f, 0.0f), 400.0f, ResourceManager::GetTexture("body"));
    Bodies.emplace_back(glm::vec2(300.0f, 540.0f), glm::vec2(-5.0f, 0.0f), 10000.0f, ResourceManager::GetTexture("body"));
    Bodies.emplace_back(glm::vec2(330.0f, 410.0f), glm::vec2(10.0f, 0.0f), 800.0f, ResourceManager::GetTexture("body"));
    Bodies.emplace_back(glm::vec2(30.0f, 40.0f), glm::vec2(100.0f, 0.0f), 1500.0f, ResourceManager::GetTexture("body"));
    Bodies.emplace_back(glm::vec2(390.0f, 5.0f), glm::vec2(5.0f, 0.0f), 67218.0f, ResourceManager::GetTexture("body"));
}

void Game::Update(float dt)
{
    for (int i = 0; i < Bodies.size(); ++i)
    {
        Bodies[i].Update(dt);
    }
}

void Game::ProcessInput(float dt)
{

}

void Game::Render()
{
    for (int i = 0; i < Bodies.size(); ++i)
    {
        Bodies[i].Draw(*Renderer);
    }
}