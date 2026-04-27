// std::expected 
#include <expected>
void ExpectedExample()
{
	enum class SuccessValue
	{
		SUCCESS
	};

	enum class ErrorValue
	{
		ERROR
	};

	auto returnErrorOnNegativeInput = [](int input)->std::expected<SuccessValue, ErrorValue> {
		if (input >= 0)
		{
			return SuccessValue::SUCCESS;
		}
		return std::unexpected(ErrorValue::ERROR);
	};

	if (auto rv = returnErrorOnNegativeInput(5); rv.has_value())
	{
		// rv.value() is populated with SuccessValue::SUCCESS;
	}
	else
	{
		// rv.error() is populated with ErrorValue::ERROR;
	}
}

// std::optional
#include <optional>
void OptionalExample()
{
	auto returnEmptyOnUnevenInput = [](int input)->std::optional<int> {
		if (input % 2 == 0)
		{
			return std::make_optional<int>(input);
		}
		else
		{
			return std::nullopt;
		}
	};

	if (auto rv = returnEmptyOnUnevenInput(2); rv.has_value())
	{
		// rv.value() is populated with 2
	}
	else
	{
		// rv.value() is empty
	}
}