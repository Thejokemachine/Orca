#include "LayoutObject.h"

#include <functional>

#include "StringUtilities.h"
#include "Layout.h"
#include "Logger.h"

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

const sf::Transform& CLayoutObject::GetLayoutTransform() const
{
	return mLayoutTransform;
}

sf::FloatRect orca::CLayoutObject::GetLocalBounds() const
{
	return mRect;
}

void CLayoutObject::SetId(std::string_view Id)
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

float orca::CLayoutObject::GetRotation()
{
	return mRotation.value;
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

void orca::CLayoutObject::SetRotation(float r)
{
	mPropertyFlags |= static_cast<int32_t>(EPropertyFlags::ROTATION);
	mRotation.value = r;
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
	pugi::xml_attribute attr;
	if (GetAttribute(node, "id", attr))
	{
		SetId(attr.as_string());
	}
	if (GetAttribute(node, "width", attr))
	{
		mWidth.expression = attr.as_string();
	}
	if (GetAttribute(node, "height", attr))
	{
		mHeight.expression = attr.as_string();
	}
	if (GetAttribute(node, "x", attr))
	{
		mPosX.expression = attr.as_string();
	}
	if (GetAttribute(node, "y", attr))
	{
		mPosY.expression = attr.as_string();
	}
	if (GetAttribute(node, "rot", attr))
	{
		mRotation.expression = attr.as_string();
	}
	if (GetAttribute(node, "pivot", attr))
	{
		std::string raw = attr.as_string();
		auto split = raw.find(',');
		mPivotX.expression = raw.substr(0, split);
		mPivotY.expression = raw.substr(split+1);
	}
	if (GetAttribute(node, "color", attr))
	{
		mColor = sf::Color(std::stoul(attr.as_string(), 0, 16));
	}
	if (GetAttribute(node, "color_hovered", attr))
	{
		mColorHover = sf::Color(std::stoul(attr.as_string(), 0, 16));
	}
	if (GetAttribute(node, "touchable", attr))
	{
		mTouchable = attr.as_bool();
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
	SetRotation(evaluateProperty(mRotation, 0.f));

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

void CLayoutObject::Update(int32_t forceFlags)
{
	mPropertyFlags |= forceFlags;
	if (mPropertyFlags != 0)
	{
		auto hasFlag = [flags=mPropertyFlags](int32_t flag) {
			return flags & static_cast<int32_t>(flag);
		};

		bool updateBuffer = false;

		if (hasFlag(static_cast<int32_t>(EPropertyFlags::POS_X) | static_cast<int32_t>(EPropertyFlags::POS_Y)))
		{
			sf::Vector2f position;
			position.x = mPosX.value;
			position.y = mPosY.value;
			mDrawableTransform.setPosition(position);
		}
		if (hasFlag(static_cast<int32_t>(EPropertyFlags::ROTATION)))
		{
			mDrawableTransform.setRotation(sf::degrees(mRotation.value));
		}
		if (hasFlag(static_cast<int32_t>(EPropertyFlags::PIVOT)))
		{
			mDrawableTransform.setOrigin(sf::Vector2f(mPivotX.value, mPivotY.value));
		}
		if (hasFlag(static_cast<int32_t>(EPropertyFlags::COLOR)))
		{
			mDrawableColor = mHovered ? mColorHover : mColor;
			if (auto drawable = GetDrawable())
			{
				for (int i = 0; i < mVertexData->vertices.size(); ++i)
				{
					mVertexData->vertices[i].color = mDrawableColor;
				}
				updateBuffer = true;;
			}
		}
		if (hasFlag(static_cast<int32_t>(EPropertyFlags::WIDTH) | static_cast<int32_t>(EPropertyFlags::HEIGHT)))
		{
			sf::Vector2f size;
			size.x = mWidth.value;
			size.y = mHeight.value;
			mRect.size = size;
			if (auto drawable = GetDrawable())
			{
				auto& vertices = mVertexData->vertices;
				vertices[0].position = { 0,				 0 };
				vertices[1].position = { size.x,		 0 };
				vertices[2].position = { size.x, size.y };
				vertices[3].position = { 0,		 size.y };
				updateBuffer = true;
			}
		}

		if (updateBuffer)
		{
			if (auto drawable = GetDrawable())
			{
				mVertexData->buffer.update(mVertexData->vertices.data());
			}
		}
	}

	mLayoutTransform = mDrawableTransform.getTransform();
	if (mParent)
	{
		mLayoutTransform = mParent->GetLayoutTransform() * mLayoutTransform;
	}

	for (auto& wkChild : mChildren)
	{
		if (auto child = wkChild.lock(); child && child->GetVisible())
		{
			child->Update(mPropertyFlags);
		}
	}

	mPropertyFlags = 0;
}

void orca::CLayoutObject::OnHoverBegin()
{
	mHovered = true;
	mPropertyFlags |= static_cast<int32_t>(EPropertyFlags::COLOR);
}

void orca::CLayoutObject::OnHoverEnd()
{
	mHovered = false;
	mPropertyFlags |= static_cast<int32_t>(EPropertyFlags::COLOR);
}

std::optional<std::weak_ptr<ILayoutObject>> orca::CLayoutObject::HandleInput(const sf::Vector2i& mousePosition)
{
	mHovered = false;
	mPropertyFlags |= static_cast<int32_t>(EPropertyFlags::COLOR);

	for (auto wkObj : mChildren)
	{
		if (auto obj = wkObj.lock())
		{
			auto handled = obj->HandleInput(mousePosition);
			if (handled.has_value())
			{
				return handled;
			}
		}
	}

	if (mTouchable)
	{
		sf::Vector2f positionObjSpace = GetLayoutTransform().getInverse().transformPoint(sf::Vector2f(mousePosition));
		if (GetLocalBounds().contains(positionObjSpace))
		{
			DebugLogF("Input consumed: {}", GetId());
			mHovered = true;
			mPropertyFlags |= static_cast<int32_t>(EPropertyFlags::COLOR);
			return weak_from_this();
		}
	}

	return std::nullopt;
}

void CLayoutObject::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= mDrawableTransform.getTransform();
	states.coordinateType = sf::CoordinateType::Pixels;
	if (mVertexData)
	{
		target.draw(mVertexData->buffer, states);
	}
	for (const auto& layoutObj : mChildren)
	{
		if (auto ptr = layoutObj.lock(); ptr && ptr->GetVisible())
		{
			target.draw(*ptr, states);
		}
	}
}

CLayoutObject::SVertexData* orca::CLayoutObject::GetDrawable()
{
	if (mVisible && mDrawableColor.a > 0 && !mVertexData)
	{
		mVertexData = std::make_unique<SVertexData>();
		auto& vertices = mVertexData->vertices;
		vertices.resize(4);
		auto& buffer = mVertexData->buffer;
		buffer.setPrimitiveType(sf::PrimitiveType::TriangleFan);
		buffer.setUsage(sf::VertexBuffer::Usage::Dynamic);
		buffer.create(vertices.size());
	}

	return mVertexData.get();
}

bool orca::CLayoutObject::GetAttribute(const pugi::xml_node& node, std::string_view name, pugi::xml_attribute& attribute)
{
	attribute = node.attribute(name);
	return !attribute.empty();
}
