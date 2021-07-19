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
#include "bhtree.h"
#include "resource_manager.h"
#include "sprite_renderer.h"

#include <glm/gtx/norm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/closest_point.hpp>

#include <iostream>
#include <vector>
#include <queue>
#include <math.h>
#include <algorithm>

// Game-related State data
SpriteRenderer* Renderer;
std::vector<Body> Bodies;

const int BODY_COUNT = 10000;
const float G_CONST = 6.67e-3;
const float E_CONST = 1e-20;

int ballId = 0;
int prev, after;
glm::vec3 Start, End, Mid;

bool Transitioning = false;
float TransitionProgress = 0.0f; 

float Camera_Distance = 2000.0f;

float FOV = glm::radians(70.0f);
float AspectRatio;

enum class TransitionState
{
    Straight,
    Curve 
};

std::queue<TransitionState> TransitionQueue;

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
    AspectRatio = static_cast<float>(this->Width) / static_cast<float>(this->Height);
    glm::mat4 projection = glm::perspective(FOV, AspectRatio, 0.01f, 10000.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Shader shader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(shader);
    // load textures
    ResourceManager::LoadTexture("textures/eden_ball3d.png", true, "body");

    // BIG CHUNGUS PLANET 
    // Bodies.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 0.0f, 0.0f), 1000.0f, ResourceManager::GetTexture("body"));
    
    for (int i = 0; i < BODY_COUNT; ++i) {
        Bodies.emplace_back(glm::sphericalRand(1000.0f), glm::vec3(0.0f, 0.0f, 0.0f), 5.0f, ResourceManager::GetTexture("body"));
    }
}

void Game::Update(float dt)
{
    if (!Paused && !Transitioning)
    {
        UpdateBarnesHut(dt);

    }

    CenterProjection(dt);
}
 
void Game::UpdateBruteForce(float dt)
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
}


void Game::UpdateBarnesHut(float dt)
{
    float min_coord = 0, max_coord = 0;

    for (Body& body : Bodies)
    {
        min_coord = std::min({ min_coord, body.Position.x, body.Position.y,  body.Position.z });
        max_coord = std::max({ max_coord, body.Position.x, body.Position.y,  body.Position.z });
    }

    float length = std::max(abs(min_coord), max_coord) + 69.0f;

    BHTree root(Oct{ glm::vec3(0.0f, 0.0f, 0.0f), length} );

    for (auto& body : Bodies)
    {
        root.Insert(&body);
    }

    for (auto& body : Bodies)
    {
        root.UpdateForce(&body);
    }

    for (auto& body : Bodies)
    {
        body.Position += body.Velocity * dt;
    }
}


void Game::ProcessInput()
{
    
       
}

void Game::Render()
{
    for (int i = 0; i < Bodies.size(); ++i)
    {
        Bodies[i].Draw(*Renderer);
    }
}



void Game::CenterProjection(float dt)
{   
 
    glm::vec3 target;

    if (Transitioning) 
    {
        glm::vec3 local_start, local_end; 
        
        if (TransitionProgress < 0.5f) 
        {
            local_start = Start;
            local_end = Mid;
        }
        else 
        {
            local_start = Mid;
            local_end = End;
        }

        float local_progress = TransitionProgress * 2.0f;
        local_progress -= floor(local_progress);

        if (TransitionProgress >= 0.5f && TransitionQueue.size() >= 2)
        {
            TransitionQueue.pop();
        }

        TransitionState CurrentState = TransitionQueue.front();
        target = local_start + ((local_end - local_start) * local_progress);
        
        if (CurrentState == TransitionState::Curve)
        {
            float diameter = glm::distance(local_start, local_end);
            float curve = sqrt(0.25 - pow(local_progress - 0.5, 2));

            target += glm::vec3(0, 0, curve * diameter);
        }

        TransitionProgress += dt;
        if (TransitionProgress >= 1.0f)
        {
            Transitioning = false;
            while (!TransitionQueue.empty()) 
            {
                TransitionQueue.pop();
            }
        }
    }
    else 
    {
        target = Bodies[ballId].Position;
    }

    target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 offset = glm::vec3(0.0f, 0.0f, Camera_Distance);

    glm::mat4 view = glm::lookAt(
        target + offset,
        target,
        glm::vec3(0.0f, 1.0f, 0.0f)

    );

   
    ResourceManager::GetShader("sprite").SetMatrix4("view", view);
}

void Game::Transition(int prev, int after) 
{
    ballId = after;
    Transitioning = true;
    TransitionProgress = 0.0f;


    Start = Bodies[prev].Position;
    End = Bodies[after].Position;

    if (Start.z > End.z) 
    {
        TransitionQueue.push(TransitionState::Curve);
        TransitionQueue.push(TransitionState::Straight);
        Mid = glm::vec3(End.x, End.y, Start.z);
    }
    else
    {
        TransitionQueue.push(TransitionState::Straight);
        TransitionQueue.push(TransitionState::Curve);
        Mid = glm::vec3(Start.x, Start.y, End.z);
    }
}

void Game::HandleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (action == GLFW_PRESS) 
    {
        if (key == GLFW_KEY_A)
        {
            Transition(ballId, ballId == 0 ? Bodies.size() - 1 : ballId - 1);

        }
        if (key == GLFW_KEY_D)
        {
            Transition(ballId, (ballId + 1) % Bodies.size());

        }
        if (key == GLFW_KEY_1)
        {
            Transition(ballId, 0);

        }
    }
    
}

void Game::HandleMouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) 
        {
            float x = (2.0f * MouseX) / static_cast<float>(Width) - 1.0f;
            float y = 1.0f - (2.0f * MouseY) / static_cast<float>(Height);

            float horizontal_fov = FOV;
            float vertical_fov = atan(tan(FOV) / AspectRatio);

            float horizontal_angle = atan(tan(horizontal_fov) * x);
            float vertical_angle = atan(tan(vertical_fov) * y);

            std::cout << glm::degrees(horizontal_angle) << "\t" << glm::degrees(vertical_angle) << std::endl;
        }
    }
}
