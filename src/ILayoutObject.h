#ifndef __ORCA_ILAYOUTOBJECT__
#define __ORCA_ILAYOUTOBJECT__

#include <string>
#include <memory>
#include <cstdint>

#include <pugixml.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Transform.hpp>

namespace orca
{
	class ILayoutObject : public sf::Drawable
	{
	public:

		virtual const std::string& GetId() = 0;
		virtual ILayoutObject* GetParent() = 0;
		virtual bool GetVisible() = 0;
		virtual const sf::Transform& GetLayoutTransform() const = 0;
		virtual sf::FloatRect GetLocalBounds() const = 0;

		virtual void SetId(std::string_view Id) = 0;
		virtual void SetParent(ILayoutObject* parent) = 0;
		virtual void SetVisible(bool visible) = 0;

		virtual float GetWidth() = 0;
		virtual float GetHeight() = 0;
		virtual float GetX() = 0;
		virtual float GetY() = 0;
		virtual float GetRotation() = 0;
		virtual sf::Vector2f GetPivot() = 0;
		virtual const sf::Color& GetColor() = 0;

		virtual void SetWidth(float width) = 0;
		virtual void SetHeight(float height) = 0;
		virtual void SetX(float x) = 0;
		virtual void SetY(float y) = 0;
		virtual void SetRotation(float r) = 0;
		virtual void SetPivot(const sf::Vector2f& pivot) = 0;
		virtual void SetColor(const sf::Color& color) = 0;

		virtual bool ParseAttributes(const pugi::xml_node& attributes) = 0;
		virtual void AddChild(const std::shared_ptr<ILayoutObject>& layoutObject) = 0;
		virtual void OnLayout() = 0;
		virtual void Update(int32_t forceFlags) = 0;

		virtual void OnHoverBegin() = 0;
		virtual void OnHoverEnd() = 0;

		virtual std::optional<std::weak_ptr<ILayoutObject>> HandleInput(const sf::Vector2i& mousePosition) = 0;

		/*
		* sf::Drawable
		*/
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
	};
}

#endif