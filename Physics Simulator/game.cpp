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
#include <glm/gtc/random.hpp>

#include <vector>


// Game-related State data
SpriteRenderer*     Renderer;
std::vector<Body>   Bodies;

const int           BODY_COUNT = 100;
const float         G_CONST = 6.67e-3;
const float         E_CONST = 1e-20;

int                 ballId = 0;
float               Camera_Distance = 200.0f;
bool                Paused = false;

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
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 
        static_cast<float>(this->Width) / static_cast<float>(this->Height), 
        0.01f, 10000.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Shader shader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(shader);
    // load textures
    ResourceManager::LoadTexture("textures/eden_ball3d.png", true, "body");

    // BIG CHUNGUS PLANET
    Bodies.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 0.0f, 0.0f), 1000.0f, ResourceManager::GetTexture("body"));
    
    for (int i = 0; i < BODY_COUNT; ++i) {
        Bodies.emplace_back(glm::sphericalRand(150.0f), glm::sphericalRand(2.0f), 5.0f, ResourceManager::GetTexture("body"));
    }
}

void Game::Update(float dt)
{
    for (int i = 0; i < Bodies.size(); ++i)
    {
        for (int j = i + 1; j < Bodies.size(); ++j) 
        {
            float F = G_CONST * Bodies[i].Mass * Bodies[j].Mass /
                sqrt(glm::distance2(Bodies[i].Position, Bodies[j].Position) + E_CONST);

            glm::vec3 Direction = glm::normalize(Bodies[i].Position - Bodies[j].Position);

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
    if (this->Keys[GLFW_KEY_A])
    {
        ballId = (ballId + 1) % Bodies.size();
    }
    if (this->Keys[GLFW_KEY_D])
    {
        if (ballId % Bodies.size() == 0)
        {
            ballId = Bodies.size() - 1;
        }
        else
        {
            ballId -= 1;
        }
    }
    if (this->Keys[GLFW_KEY_1])
    {
        ballId = 0;
    }
}

void Game::Render()
{
    for (int i = 0; i < Bodies.size(); ++i)
    {
        Bodies[i].Draw(*Renderer);
    }
}



void Game::CenterProjection()
{   
    
    glm::vec3 target = Bodies[ballId].Position;
    glm::vec3 offset = glm::vec3(0.0f, 0.0f, Camera_Distance);


    glm::mat4 view = glm::lookAt(
        target + offset,
        target,
        glm::vec3(0.0f, 1.0f, 0.0f)

    );

   


    ResourceManager::GetShader("sprite").SetMatrix4("view", view);
}
