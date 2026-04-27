#ifndef __ORCA_LAYOUTOBJECT__
#define __ORCA_LAYOUTOBJECT__

#include "ILayoutObject.h"

#include <SFML/Graphics/RectangleShape.hpp>

#include <optional>
#include <memory>
#include <vector>

namespace orca
{
	class CLayoutObject : public ILayoutObject
	{
	public:

		virtual ~CLayoutObject() {};

		virtual const std::string& GetId() override;
		virtual ILayoutObject* GetParent() override;
		virtual bool GetVisible() override;

		virtual void SetId(const std::string& Id) override;
		virtual void SetParent(ILayoutObject* parent) override;
		virtual void SetVisible(bool visible) override;

		virtual float GetWidth() override;
		virtual float GetHeight() override;
		virtual float GetX() override;
		virtual float GetY() override;
		virtual sf::Vector2f GetPivot() override;
		virtual const sf::Color& GetColor() override;

		virtual void SetWidth(float width) override;
		virtual void SetHeight(float height) override;
		virtual void SetX(float x) override;
		virtual void SetY(float y) override;
		virtual void SetPivot(const sf::Vector2f& pivot) override;
		virtual void SetColor(const sf::Color& color) override;

		virtual bool ParseAttributes(const pugi::xml_node& attributes) override;
		virtual void AddChild(const std::shared_ptr<ILayoutObject>& layoutObject) override;
		virtual void OnLayout() override;
		virtual void Update() override;

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
			COLOR = 1 << 4,
			PIVOT = 1 << 5,
			NONE = 0,
			ALL = std::numeric_limits<int32_t>::max(),
		};

		std::string mId;
		ILayoutObject* mParent{ nullptr };
		bool mVisible{ true };

		std::vector<std::weak_ptr<ILayoutObject>> mChildren;

		int mPropertyFlags{ static_cast<int>(EPropertyFlags::ALL) };

		struct SValueProperty
		{
			float value{ 0.f };
			std::string expression;
		};

		SValueProperty mWidth;
		SValueProperty mHeight;
		SValueProperty mPosX;
		SValueProperty mPosY;
		SValueProperty mPivotX;
		SValueProperty mPivotY;

		sf::Color mColor{ sf::Color::White };

		sf::RectangleShape mRect;
	};
}

#endif