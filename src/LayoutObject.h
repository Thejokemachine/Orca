#ifndef __ORCA_LAYOUTOBJECT__
#define __ORCA_LAYOUTOBJECT__

#include "ILayoutObject.h"

#include <SFML/Graphics/VertexBuffer.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include <optional>
#include <memory>
#include <vector>

namespace orca
{
	class CLayoutObject : public ILayoutObject, public std::enable_shared_from_this<CLayoutObject>
	{
	public:

		virtual ~CLayoutObject() {};

		virtual const std::string& GetId() override;
		virtual ILayoutObject* GetParent() override;
		virtual bool GetVisible() override;
		virtual const sf::Transform& GetLayoutTransform() const override;
		virtual sf::FloatRect GetLocalBounds() const override;

		virtual void SetId(std::string_view Id) override;
		virtual void SetParent(ILayoutObject* parent) override;
		virtual void SetVisible(bool visible) override;

		virtual float GetWidth() override;
		virtual float GetHeight() override;
		virtual float GetX() override;
		virtual float GetY() override;
		virtual float GetRotation() override;
		virtual sf::Vector2f GetPivot() override;
		virtual const sf::Color& GetColor() override;

		virtual void SetWidth(float width) override;
		virtual void SetHeight(float height) override;
		virtual void SetX(float x) override;
		virtual void SetY(float y) override;
		virtual void SetRotation(float r) override;
		virtual void SetPivot(const sf::Vector2f& pivot) override;
		virtual void SetColor(const sf::Color& color) override;

		virtual bool ParseAttributes(const pugi::xml_node& attributes) override;
		virtual void AddChild(const std::shared_ptr<ILayoutObject>& layoutObject) override;
		virtual void OnLayout() override;
		virtual void Update(int32_t forceFlags = 0) override;

		virtual void OnHoverBegin() override;
		virtual void OnHoverEnd() override;

		virtual std::optional<std::weak_ptr<ILayoutObject>> HandleInput(const sf::Vector2i& mousePosition) override;
		/*
		* sf::Drawable
		*/
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	protected:

		enum class EPropertyFlags : int32_t
		{
			WIDTH = 1 << 0,
			HEIGHT = 1 << 1,
			POS_X = 1 << 2,
			POS_Y = 1 << 3,
			ROTATION = 1 << 4,
			COLOR = 1 << 5,
			PIVOT = 1 << 6,
			NONE = 0,
			ALL = std::numeric_limits<int32_t>::max(),
		};

		struct SValueProperty
		{
			float value{ 0.f };
			std::string expression;
		};

		struct SVertexData
		{
			std::vector<sf::Vertex> vertices;
			sf::VertexBuffer buffer;
		};

		SVertexData* GetDrawable();
		static bool GetAttribute(const pugi::xml_node& node, std::string_view name, pugi::xml_attribute& attribute);

		std::string mId;
		ILayoutObject* mParent{ nullptr };
		bool mVisible{ true };

		std::vector<std::weak_ptr<ILayoutObject>> mChildren;

		int mPropertyFlags{ static_cast<int>(EPropertyFlags::ALL) };

		SValueProperty mWidth;
		SValueProperty mHeight;
		SValueProperty mPosX;
		SValueProperty mPosY;
		SValueProperty mRotation;
		SValueProperty mPivotX;
		SValueProperty mPivotY;

		sf::Color mColor{ sf::Color::Transparent };
		
		bool mTouchable{ false };
		bool mHovered{ false };
		sf::Color mColorHover;

		sf::Transform mLayoutTransform;
		sf::Transformable mDrawableTransform;
		sf::FloatRect mRect;
		sf::Color mDrawableColor;

		std::unique_ptr<SVertexData> mVertexData;
	};
}

#endif