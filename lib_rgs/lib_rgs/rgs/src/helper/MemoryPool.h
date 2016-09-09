#pragma once

#include <memory>
#include <ppl.h>
#include <concurrent_unordered_map.h>
#include <boost/pool/singleton_pool.hpp>

namespace rgs
{
	namespace memory
	{
		template<typename T> using sp = std::shared_ptr < T >;

		template<typename T, typename TLock>
		class PurgeMemory
		{
		private:
			template<typename T1, typename T2> using allocator_type = boost::singleton_pool < T1, sizeof(T1), boost::default_user_allocator_new_delete, T2 >;

		public:
			virtual ~PurgeMemory()
			{
				allocator_type<T, TLock>::purge_memory();
			}
		};

		/************ MemoryPool <T, TLock> *************/

		template<typename T, typename TLock = boost::details::pool::default_mutex>
		class MemoryPool
		{
		public:
			template<typename T1, typename T2> using allocator_type = boost::singleton_pool < T1, sizeof(T1), boost::default_user_allocator_new_delete, T2 >;
		
		public:
			static void* operator new(size_t size)
			{
				static PurgeMemory<T, TLock> purgeMemory;
				return allocator_type<T, TLock>::malloc();
			}

				static void* operator new(size_t size, void* ptr)
			{
				return ptr;
			}

				static void operator delete(void* p)
			{
				allocator_type<T, TLock>::free(p);
			}
		};
		

		/************ InheritMemoryPool <T, TLock> *************/

		template<bool B, typename T = void> using disable_if = std::enable_if<!B, T>;

		template<typename T, typename TLock = boost::details::pool::default_mutex>
		class InheritMemoryPool : public MemoryPool < InheritMemoryPool <T>, TLock >
		{
		public:
			static T* allocate();
			static bool deallocate(T*);

		public:
			InheritMemoryPool();

			//disable_if<...>의 ...가 true면 std::enable<false, void>이다.
			//std::enable<false, void>는 error-type이기 때문에 모호한 호출을 피할 수 있다.
			template<typename TArg, typename... TArgs, typename = typename
				disable_if<
				sizeof...(TArgs) == 0 &&
				std::is_same<typename
				std::remove_reference<TArg>::type,
				InheritMemoryPool<T, TLock>
				>::value
			>::type>
			InheritMemoryPool(TArg&&, TArgs&&...);

			T* operator ->()const;
			T* get()const;

		private:
			static concurrency::concurrent_unordered_map<T*, InheritMemoryPool<T, TLock>*> allocatedData;
			T data_;
		};
		template<typename T, typename TLock>
		concurrency::concurrent_unordered_map<T*, InheritMemoryPool<T, TLock>*> InheritMemoryPool<T, TLock>::allocatedData;

		template<typename T, typename TLock>
		T* InheritMemoryPool<T, TLock>::allocate()
		{
			InheritMemoryPool<T, TLock>* data = new InheritMemoryPool<T, TLock>();
			allocatedData[&data->data_] = data;
			return &data->data_;
		}

		template<typename T, typename TLock>
		bool InheritMemoryPool<T, TLock>::deallocate(T* t)
		{
			InheritMemoryPool<T, TLock>* data = allocatedData[t];
			if (data != NULL)
			{
				delete data;
				allocatedData[t] = NULL;
			}
			return data != NULL;
		}

		template<typename T, typename TLock>
		InheritMemoryPool<T, TLock>::InheritMemoryPool() {}

		template<typename T, typename TLock>
		template<typename TArg, typename... TArgs, typename>
		InheritMemoryPool<T, TLock>::InheritMemoryPool(TArg&& arg, TArgs&&... args) : data_(std::forward<TArg>(arg), std::forward<TArgs>(args)...){}

		template<typename T, typename TLock>
		T* InheritMemoryPool<T, TLock>::operator ->()const
		{
			return &data;
		}

		template<typename T, typename TLock>
		T* InheritMemoryPool<T, TLock>::get()const
		{
			return &data;
		}
	}
}