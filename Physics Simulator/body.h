#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"

class Body
{
public:
	glm::vec2 Position, Velocity;
	float Mass;
	glm::vec2 Size;

	Texture2D Sprite;

	Body();
	Body(glm::vec2 pos, glm::vec2 vel, float mass, Texture2D sprite);

	virtual void Draw(SpriteRenderer& renderer);
	virtual void Update(float dt);
};

#endif