/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

extern float Camera_Distance;
extern bool Paused;
extern int ballId;
extern int prev, after;

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState State;
    bool Keys[1024];
    unsigned int Width, Height;
    double MouseX, MouseY;

    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput();
    void Update(float dt);
    void UpdateBruteForce(float dt);
    void UpdateBarnesHut(float dt);
    void Render();

    void CenterProjection(float dt);
    void Transition(int prev, int after);

    void HandleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mode);
    void HandleMouseButtonEvent(GLFWwindow* window, int button, int action, int mods);

};

#endif