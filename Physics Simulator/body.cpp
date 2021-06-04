#include "body.h"

Body::Body()
:	Position(0.0f, 0.0f, 0.0f)
,	Velocity(0.0f, 0.0f, 0.0f)
,	Mass(1.0f)
,	Sprite()
,   Size(1.0f, 1.0f)
{}

Body::Body(glm::vec3 pos, glm::vec3 vel, float mass, Texture2D sprite)
:	Position(pos)
,	Velocity(vel)
,	Mass(mass)
,	Size(sqrt(mass), sqrt(mass))
,	Sprite(sprite)
{}

void Body::Draw(SpriteRenderer & renderer)
{
	renderer.DrawSprite(this->Sprite, this->Position, Size);
}

void Body::Update(float dt)
{
	
}
