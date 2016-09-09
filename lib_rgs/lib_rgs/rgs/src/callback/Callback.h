#pragma once

#include <utility>
#include <functional>

namespace rgs
{
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
			void operator()()const; //�̸� �Ű�����(Ts...)�� �Ѱ��� �� ȣ���Ѵ�.
			
			//��ȣ�� ȣ���� ���ϱ� ���� std::enable_if ���
			template<typename... Args, typename = typename std::enable_if<sizeof...(Args) != 0>::type>
			void operator()(Args&&...)const; //���߿� �Ű������� �ѱ� �� ȣ���Ѵ�.
			void operator()(std::tuple<Ts...>&)const;

			void operator=(const Callback<Ts...>&);

			template <typename F>
			void setCallback(F&&);

			template <typename F, typename... Args, typename = typename std::enable_if<sizeof...(Args) != 0>::type>
			void setCallback(F&&, Args&&...);

		private:
			//std::tuple<Ts...>�� std::function<void(Ts...)>�� �Ű������� ��ȯ�Ѵ�.
			template <typename... Args, int... Is>
			void toVariadic(const std::tuple<Args...>&, helper::tuple::seq<Is...>)const;

			template <typename... Args>
			void toVariadic(const std::tuple<Args...>&)const;

		private:
			bool isInitialized_; //args�� �ʱ�ȭ�Ǿ����� ����.
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

		//operator()(std::tuple<Ts...>&)
		template<typename... Ts>
		void Callback<Ts...>::operator()(std::tuple<Ts...>& args)const
		{
			toVariadic(args);
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
			//sizeof...()�� variadic�� ���� template parameter ������ �˷��ش�.
			toVariadic(tup, helper::tuple::gen_seq < sizeof...(Args) > {});
		}
	}
}