#include "LayoutSequence.h"

using namespace orca;

void orca::CLayoutSequence::Update(int32_t forceFlags)
{
	sf::Vector2f nextPosition;
	sf::Vector2f directionModifiers;
	sf::Vector2f spacing;

	switch (mDirection)
	{
	case EDirection::LEFT:
		directionModifiers.x = -1;
		break;
	case EDirection::RIGHT:
		directionModifiers.x = 1;
		break;
	case EDirection::UP:
		directionModifiers.y = -1;
		break;
	case EDirection::DOWN:
		directionModifiers.y = 1;
		break;
	}

	spacing.x = directionModifiers.x * mSpacing;
	spacing.y = directionModifiers.y * mSpacing;

	for (auto wkChild : mChildren)
	{
		if (auto child = wkChild.lock())
		{
			child->SetX(nextPosition.x);
			child->SetY(nextPosition.y);

			sf::Vector2f size;
			size.x = child->GetWidth();
			size.y = child->GetHeight();

			nextPosition += size.componentWiseMul(directionModifiers) + spacing;
		}
	}

	CLayoutObject::Update(forceFlags);
}

bool CLayoutSequence::ParseAttributes(const pugi::xml_node& node)
{
	pugi::xml_attribute attr;
	if (GetAttribute(node, "spacing", attr))
	{
		mSpacing = attr.as_float();
	}
	if (GetAttribute(node, "direction", attr))
	{
		const auto str = attr.as_string();
		if (!strcmp(str, "left"))
		{
			mDirection = EDirection::LEFT;
		}
		else if (!strcmp(str, "right"))
		{
			mDirection = EDirection::RIGHT;
		}
		else if (!strcmp(str, "up"))
		{
			mDirection = EDirection::UP;
		}
		else if (!strcmp(str, "down"))
		{
			mDirection = EDirection::DOWN;
		}
	}

	return CLayoutObject::ParseAttributes(node);
}