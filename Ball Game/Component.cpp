#include "stdafx.h"

Component::Component(class Actor* actor, int order) : mOwner(actor), mUpdateOrder(order) {}

Component::~Component() {}

void Component::ProcessInput(const uint8_t* keyState) {}

void Component::Update(float delta) {}