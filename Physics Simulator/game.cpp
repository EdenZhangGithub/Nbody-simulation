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

#include <glm/gtx/norm.hpp>

#include <vector>


// Game-related State data
SpriteRenderer* Renderer;
std::vector<Body> Bodies;

const float G_CONST = 6.67e-3;
const float E_CONST = 1e-20;

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
        static_cast<float>(this->Height), 0.0f, -5.0f, 5.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Shader shader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(shader);
    // load textures
    ResourceManager::LoadTexture("textures/eden_ball3d.png", true, "body");

  
    Bodies.emplace_back(glm::vec2(800.0f, 500.0f), glm::vec2(50.0f, 1.0f), 90000.0f, ResourceManager::GetTexture("body"));
    Bodies.emplace_back(glm::vec2(1200.0f, 500.0f), glm::vec2(0.0f, 200.0f), 10000.0f, ResourceManager::GetTexture("body"));
    Bodies.emplace_back(glm::vec2(300.0f, 100.0f), glm::vec2(10.0f, 2.0f), 400.0f, ResourceManager::GetTexture("body"));
    Bodies.emplace_back(glm::vec2(100.0f, 1400.0f), glm::vec2(-20.0f, 5.0f), 900.0f, ResourceManager::GetTexture("body"));
    Bodies.emplace_back(glm::vec2(300.0f, 300.0f), glm::vec2(30.0f, 10.0f), 400.0f, ResourceManager::GetTexture("body"));
    Bodies.emplace_back(glm::vec2(1000.0f, 1200.0f), glm::vec2(-5.0f, -6.0f), 1600.0f, ResourceManager::GetTexture("body"));
    Bodies.emplace_back(glm::vec2(1300.0f, 100.0f), glm::vec2(-10.0f, -20.0f), 400.0f, ResourceManager::GetTexture("body"));
    Bodies.emplace_back(glm::vec2(900.0f, 900.0f), glm::vec2(-10.0f, -5.0f), 900.0f, ResourceManager::GetTexture("body"));
    Bodies.emplace_back(glm::vec2(100.0f, 1500.0f), glm::vec2(0.0f, -20.0f), 400.0f, ResourceManager::GetTexture("body"));
    Bodies.emplace_back(glm::vec2(1500.0f, 1200.0f), glm::vec2(-20.0f, -6.0f), 1600.0f, ResourceManager::GetTexture("body"));
}

void Game::Update(float dt)
{
    for (int i = 0; i < Bodies.size(); ++i)
    {
        for (int j = i + 1; j < Bodies.size(); ++j) 
        {
            float F = G_CONST * Bodies[i].Mass * Bodies[j].Mass /
                sqrt(glm::distance2(Bodies[i].Position, Bodies[j].Position) + E_CONST);

            glm::vec2 Direction = glm::normalize(Bodies[i].Position - Bodies[j].Position);

            Bodies[i].Velocity -= Direction * F / Bodies[i].Mass;
            Bodies[j].Velocity += Direction * F / Bodies[j].Mass;
        }
    }

    for (int i = 0; i < Bodies.size(); ++i)
    {
        Bodies[i].Position += Bodies[i].Velocity * dt;
    }

    CenterProjection();
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

int ballId = 0;
void Game::CenterProjection()
{
    
    glm::vec2 center = Bodies[ballId].Position;

    if (this->Keys[GLFW_KEY_A])
    {
        ballId = (ballId + 1) % 10;
    }
    if (this->Keys[GLFW_KEY_D]) 
    {
        if (ballId % 10 == 0) 
        {
            ballId = 9;
        }
        else 
        {
            ballId -= 1;
        }
    }


    glm::mat4 projection = glm::ortho(center.x - static_cast<float>(this->Width) / 2, center.x + static_cast<float>(this->Width) / 2,
        center.y + static_cast<float>(this->Height) / 2, center.y - static_cast<float>(this->Height) / 2, -1.0f, 1.0f);
    
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
}
