#include "LayoutObject.h"

#include <functional>

#include "StringUtilities.h"
#include "Layout.h"

#include <SFML/Graphics/RenderTarget.hpp>

using namespace orca;

const std::string& CLayoutObject::GetId()
{
	return mId;
}

ILayoutObject* CLayoutObject::GetParent()
{
	return mParent;
}

bool CLayoutObject::GetVisible()
{
	return mVisible;
}

void CLayoutObject::SetId(const std::string& Id)
{
	mId = Id;
}

void CLayoutObject::SetParent(ILayoutObject* parent)
{
	mParent = parent;
}

void CLayoutObject::SetVisible(bool visible)
{
	mVisible = visible;
}

float CLayoutObject::GetWidth()
{
	return mWidth.value;
}

float CLayoutObject::GetHeight()
{
	return mHeight.value;
}

float CLayoutObject::GetX()
{
	return mPosX.value;
}

float CLayoutObject::GetY()
{
	return mPosY.value;
}

sf::Vector2f orca::CLayoutObject::GetPivot()
{
	return sf::Vector2f(mPivotX.value, mPivotY.value);
}

const sf::Color& orca::CLayoutObject::GetColor()
{
	return mColor;
}

void CLayoutObject::SetWidth(float width)
{
	mPropertyFlags |= static_cast<int32_t>(EPropertyFlags::WIDTH);
	mWidth.value = width;
}

void CLayoutObject::SetHeight(float height)
{
	mPropertyFlags |= static_cast<int32_t>(EPropertyFlags::HEIGHT);
	mHeight.value = height;
}

void CLayoutObject::SetX(float x)
{
	mPropertyFlags |= static_cast<int32_t>(EPropertyFlags::POS_X);
	mPosX.value = x;
}

void CLayoutObject::SetY(float y)
{
	mPropertyFlags |= static_cast<int32_t>(EPropertyFlags::POS_Y);
	mPosY.value = y;
}

void orca::CLayoutObject::SetPivot(const sf::Vector2f& pivot)
{
	mPropertyFlags |= static_cast<int32_t>(EPropertyFlags::PIVOT);
	mPivotX.value = pivot.x;
	mPivotY.value = pivot.y;
}

void orca::CLayoutObject::SetColor(const sf::Color& color)
{
	mPropertyFlags |= static_cast<int32_t>(EPropertyFlags::COLOR);
	mColor = color;
}

bool CLayoutObject::ParseAttributes(const pugi::xml_node& node)
{
	auto getAttribute = [&node](const std::string& name, pugi::xml_attribute& attribute) -> bool {
		attribute = node.attribute(name);
		return !attribute.empty();
	};

	pugi::xml_attribute attr;
	if (getAttribute("id", attr))
	{
		SetId(attr.as_string());
	}
	if (getAttribute("width", attr))
	{
		mWidth.expression = attr.as_string();
	}
	if (getAttribute("height", attr))
	{
		mHeight.expression = attr.as_string();
	}
	if (getAttribute("x", attr))
	{
		mPosX.expression = attr.as_string();
	}
	if (getAttribute("y", attr))
	{
		mPosY.expression = attr.as_string();
	}
	if (getAttribute("pivot", attr))
	{
		std::string raw = attr.as_string();
		auto split = raw.find(',');
		mPivotX.expression = raw.substr(0, split);
		mPivotY.expression = raw.substr(split+1);
	}
	if (getAttribute("color", attr))
	{
		mColor = sf::Color(std::stoul(attr.as_string(), 0, 16));
	}

	return true;
}

void CLayoutObject::AddChild(const std::shared_ptr<ILayoutObject>& layoutObject)
{
	layoutObject->SetParent(this);
	mChildren.emplace_back(layoutObject);
}

void CLayoutObject::OnLayout()
{
	auto evaluateProperty = [](const SValueProperty& property, float percentageModifier)->float {
		const auto& expression = property.expression;
		float value = property.value;
		if (!expression.empty())
		{
			if (expression.ends_with('%'))
			{
				value = percentageModifier * std::stof(expression.substr(0, expression.length() - 1));
			}
			else if (expression.starts_with('{') && expression.ends_with('}'))
			{
				// Is actual expression
			}
			else
			{
				value = std::stof(expression);
			}
		}
		return value;
	};

	const float parentWidthModifier = mParent ? mParent->GetWidth() / 100.f : 0.f;
	const float parentHeightModifier = mParent ? mParent->GetHeight() / 100.f : 0.f;
	SetWidth(evaluateProperty(mWidth, parentWidthModifier));
	SetHeight(evaluateProperty(mHeight, parentHeightModifier));
	SetX(evaluateProperty(mPosX, parentWidthModifier));
	SetY(evaluateProperty(mPosY, parentHeightModifier));

	const float selfWidthModifier = GetWidth() / 100.f;
	const float selfHeightModifier = GetHeight() / 100.f;
	SetPivot(sf::Vector2f(evaluateProperty(mPivotX, selfWidthModifier), evaluateProperty(mPivotY, selfHeightModifier)));

	for (auto& wkChild : mChildren)
	{
		if (auto child = wkChild.lock(); child && child->GetVisible())
		{
			child->OnLayout();
		}
	}
}

void CLayoutObject::Update()
{
	if (mPropertyFlags != 0)
	{
		auto hasFlag = [flags=mPropertyFlags](int32_t flag) {
			return flags & static_cast<int32_t>(flag);
		};

		if (hasFlag(static_cast<int32_t>(EPropertyFlags::WIDTH) | static_cast<int32_t>(EPropertyFlags::HEIGHT)))
		{
			sf::Vector2f size;
			size.x = mWidth.value;
			size.y = mHeight.value;
			mRect.setSize(size);
		}
		if (hasFlag(static_cast<int32_t>(EPropertyFlags::POS_X) | static_cast<int32_t>(EPropertyFlags::POS_Y)))
		{
			sf::Vector2f position;
			position.x = mPosX.value;
			position.y = mPosY.value;
			mRect.setPosition(position);
		}
		if (hasFlag(static_cast<int32_t>(EPropertyFlags::COLOR)))
		{
			mRect.setFillColor(mColor);
		}
		if (hasFlag(static_cast<int32_t>(EPropertyFlags::PIVOT)))
		{
			mRect.setOrigin(sf::Vector2f(mPivotX.value, mPivotY.value));
		}

		mPropertyFlags = 0;
	}

	for (auto& wkChild : mChildren)
	{
		if (auto child = wkChild.lock(); child && child->GetVisible())
		{
			child->Update();
		}
	}
}

void CLayoutObject::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mRect, states);
}
