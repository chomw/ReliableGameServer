#pragma once

#include <utility>
#include <functional>

namespace rgs
{
	/**
	@namespace cb
	@brief callback 관련 네임스페이스
	@since 1.0.0
	*/
	namespace cb
	{
		template <typename... Ts>
		class Callback 
		{
		public:
			Callback();
			Callback(const Callback<Ts...>&);

			template <typename F, typename... Args>
			Callback(F&&, Args&&...);

			template <typename F>
			Callback(F&&);

			template <typename F>
			Callback(F&&, const std::tuple<Ts...>&);

		public:
			void operator()()const; //미리 매개변수(Ts...)를 넘겼을 때 호출한다.
			
			//모호한 호출을 피하기 위해 std::enable_if 사용
			template<typename... Args, typename = typename std::enable_if<sizeof...(Args) != 0>::type>
			void operator()(Args&&...)const; //나중에 매개변수를 넘길 때 호출한다.

			void operator=(const Callback<Ts...>&);

			template <typename F>
			void setCallback(F&&);

			template <typename F, typename... Args, typename = typename std::enable_if<sizeof...(Args) != 0>::type>
			void setCallback(F&&, Args&&...);

		private:
			//std::tuple<Ts...>를 std::function<void(Ts...)>의 매개변수로 변환한다.
			template <typename... Args, int... Is>
			void toVariadic(const std::tuple<Args...>&, helper::tuple::seq<Is...>)const;

			template <typename... Args>
			void toVariadic(const std::tuple<Args...>&)const;

		private:
			bool isInitialized_; //args가 초기화되었는지 여부.
			std::function<void(Ts...)> func_;
			std::tuple<Ts...> args_;
		};

		//constructors (copy constructor>
		template <typename... Ts>
		Callback<Ts...>::Callback(){}

		template <typename... Ts>
		Callback<Ts...>::Callback(const Callback<Ts...>& other)
		{
			this->isInitialized_ = other.isInitialized_;
			this->args_ = other.args_;
			this->func_ = other.func_;
		}

		template <typename... Ts>
		template <typename F, typename... Args>
		Callback<Ts...>::Callback(F&& func, Args&&... args) :
			isInitialized_(true),
			func_(std::forward<F>(func)),
			args_(std::forward<Args>(args)...) {
		}

		template<typename... Ts>
		template<typename F>
		Callback<Ts...>::Callback(F&& func) :
			isInitialized_(false),
			func_(std::forward<F>(func)) {}

		template<typename... Ts>
		template<typename F>
		Callback<Ts...>::Callback(F&& func, const std::tuple<Ts...>& args) :
			isInitialized_(true),
			func_(std::forward<F>(func)),
			args_(args) {
		}

		//operator()()
		template <typename... Ts>
		inline void Callback<Ts...>::operator()()const
		{
			if (isInitialized_ || sizeof...(Ts) == 0)
			{
				toVariadic(args_);
			}
		}

		//operator()(Ts&&... _args)
		template<typename... Ts>
		template<typename... Args, typename>
		void Callback<Ts...>::operator()(Args&&... args)const
		{
			func_(std::forward<Args>(args)...);
		}

		template <typename... Ts>
		void Callback<Ts...>::operator=(const Callback<Ts...>& other)
		{
			this->isInitialized_ = other.isInitialized_;
			this->args_ = other.args_;
			this->func_ = other.func_;
		}

		template <typename... Ts>
		template <typename F>
		void Callback<Ts...>::setCallback(F&& func)
		{
			func_ = func;
		}

		template <typename... Ts>
		template <typename F, typename... Args, typename>
		void Callback<Ts...>::setCallback(F&& func, Args&&... args)
		{
			func_ = func;
			args_ = std::tuple<Args...>(std::forward<Args>(args)...);
		}

		//toVariadic(,)
		template <typename... Ts>
		template <typename... Args, int... Is>
		inline void Callback<Ts...>::toVariadic(const std::tuple<Args...>& tup, helper::tuple::seq<Is...>)const
		{
			if (func_)
			{
				func_(std::get<Is>(tup)...);
			}
		}

		//toVariadic()
		template<typename... Ts>
		template<typename... Args>
		inline void Callback<Ts...>::toVariadic(const std::tuple<Args...>& tup)const
		{
			//sizeof...()는 variadic이 가진 template parameter 개수를 알려준다.
			toVariadic(tup, helper::tuple::gen_seq < sizeof...(Args) > {});
		}
	}
}