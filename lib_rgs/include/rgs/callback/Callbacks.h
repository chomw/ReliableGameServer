#pragma once

#include <ppl.h>
#include <concurrent_unordered_map.h>

#include "Callback.h"

namespace rgs
{
	namespace cb
	{
		template <typename... Ts>
		class Callbacks
		{
		public:
			//����� callback�� ȣ���Ѵ�.
			void operator()(int)const;

			template<typename... Args, typename = typename std::enable_if<sizeof...(Args) != 0>::type>
			void operator()(int, Args&&...)const;

			//rgs::cb::callback<>�� �����Ѵ�.
			void add(int, const Callback<Ts...>&);

			//callback�� args�� ���� �����Ѵ�.
			template<typename F>
			void add(int, F&&, Ts&&...);

			//����� callback�� ȣ���Ѵ�.
			void call(int)const;

			template<typename... Args, typename = typename std::enable_if<sizeof...(Args) != 0>::type>
			void call(int, Args&&...)const;

			unsigned int size()const;

		private:
			mutable concurrency::concurrent_unordered_multimap<int, std::shared_ptr<Callback<Ts...>>> callbacks_;
		};

		template<typename... Ts>
		inline void Callbacks<Ts...>::operator()(int key)const
		{
			call(key);
		}

		template<typename... Ts>
		template<typename... Args, typename>
		inline void Callbacks<Ts...>::operator()(int key, Args&&... args)const
		{
			call(key, std::forward<Args>(args)...);
		}

		template <typename... Ts>
		void Callbacks<Ts...>::add(int key, const rgs::cb::Callback<Ts...>& callback)
		{
			callbacks_.insert(std::make_pair(
				key,
				std::make_shared<rgs::cb::Callback<Ts...>>(callback)
				));
		}

		template <typename... Ts>
		template<typename F>
		void Callbacks<Ts...>::add(int key, F&& func, Ts&&... args)
		{
			auto callback = std::make_shared<rgs::cb::Callback<Ts...>>(
				std::forward<F>(func),
				std::tuple<Ts...>(std::forward<Ts>(args)...)
				);

			callbacks_.insert(std::make_pair(key, callback));
		}

		template <typename... Ts>
		inline void Callbacks<Ts...>::call(int key)const
		{
			auto iterators = callbacks_.equal_range(key);

			//callback�� ȣ���Ѵ�.
			for (auto iterator = iterators.first; iterator != iterators.second; ++iterator)
			{
				(*iterator->second)();
			}
		}

		template <typename... Ts>
		template<typename... Args, typename>
		inline void Callbacks<Ts...>::call(int key, Args&&... args)const
		{
			//key�� �ش��ϴ� callback�� ���Ѵ�.
			auto iterators = callbacks_.equal_range(key);

			//���� callback�� ��� ȣ���Ѵ�.
			for (auto iterator = iterators.first; iterator != iterators.second; ++iterator)
			{
				(*iterator->second)(std::forward<Args>(args)...);
			}
		}

		template <typename... Ts>
		unsigned int Callbacks<Ts...>::size()const
		{
			return callbacks_.size();
		}
	}
}