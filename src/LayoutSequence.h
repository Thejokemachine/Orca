#ifndef __ORCA_LAYOUTSEQUENCE__
#define __ORCA_LAYOUTSEQUENCE__

#include "LayoutObject.h"

namespace orca
{
	class CLayoutSequence : public CLayoutObject
	{
	public:

		virtual void Update(int32_t forceFlags = 0) override;
		virtual bool ParseAttributes(const pugi::xml_node& node) override;

	protected:

		float mSpacing{ 0.f };

		enum class EDirection
		{
			LEFT,
			RIGHT,
			UP,
			DOWN,
		} mDirection{ EDirection::LEFT };
	};
}

#endif