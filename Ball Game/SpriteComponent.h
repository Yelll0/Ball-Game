#pragma once

class SpriteComponent : public Component
{
public:
	SpriteComponent(class Actor* owner, int order = 100);
	~SpriteComponent();

	virtual void Draw(class Shader* shader);
	virtual void SetTexture(class Texture* texture);

	int GetDrawOrder() const { return mDrawOrder; }
	int GetTexWidth() const { return mTexWidth; }
	int GetTexHeight() const { return mTexHeight; }

private:
	class Texture* mTexture;
	int mDrawOrder;
	int mTexWidth, mTexHeight;
};

