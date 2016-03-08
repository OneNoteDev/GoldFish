#pragma once

#include <tuple>
#include <type_traits>

namespace goldfish
{
	namespace details
	{
		template <class... Lambdas> struct info {};

		template <class T, class... Args> struct is_callable
		{
			struct yes {};
			struct no {};
			template <class U> static yes test(decltype(std::declval<U>()(std::declval<Args>()...))*) { return{}; }
			template <class U> static no test(...) { return{}; }
			enum { value = std::is_same<yes, decltype(test<T>(nullptr))>::value };
		};

		template <class Head, class... Tail> struct info<Head, Tail...>
		{
			template <bool /*is_head_callable*/, class... Args> struct first_callable_helper {};
			template <class... Args> struct first_callable
			{
				enum { value = first_callable_helper<is_callable<Head, Args...>::value, Args...>::value };
			};

			template <class... Args> struct first_callable_helper<true, Args...> { enum { value = 0 }; };
			template <class... Args> struct first_callable_helper<false, Args...> { enum { value = 1 + info<Tail...>::first_callable<Args...>::value }; };
		};
	}

	template <class... Lambdas>
	auto first_match(Lambdas&&... lambdas)
	{
		return [lambdas = std::make_tuple(std::forward<Lambdas>(lambdas)...)](auto&&... args)
		{
			return std::get<details::info<Lambdas...>::first_callable<decltype(args)...>::value>(lambdas)(std::forward<decltype(args)>(args)...);
		};
	}
}