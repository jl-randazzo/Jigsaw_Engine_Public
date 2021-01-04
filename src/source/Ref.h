#ifndef _REF_H_
#define _REF_H_
#include <memory>
#include <functional>

namespace Jigsaw {
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template <class T, class... _Types>
	inline Ref<T> MakeRef(_Types&& ... Args) {
		return std::make_shared<T, _Types...>(std::forward<_Types>(Args)...);
	}

	template<typename T>
	using Weak = std::weak_ptr<T>;

	template<typename T>
	using Unique = std::unique_ptr<T>;

	template <class T, class... _Types>
	Unique<T> MakeUnique(_Types&& ... Args) {
		return std::make_unique<T, _Types...>(Args...);
	}

	template <class T>
	Ref<T> MakeRef(Unique<T>& source) {
		return Ref<T>(std::move(source));
	}

	template <class T>
	Ref<T> MakeRef(Unique<T>&& source) {
		return Ref<T>(std::move(source));
	}

	template <class T, class V>
	Unique<V[]> UniqueMap(const Unique<T[]>& original, const unsigned int count, std::function<V(T&)> functor) {
		V* v_arr = new V[count];
		for (int i = 0; i < count; i++) {
			v_arr[i] = functor(original.get()[i]);
		}
		return Unique<V[]>(v_arr);
	}

	template<class T>
	Unique<T[]> UniqueSplit(const Unique<T[]>& original, const unsigned int start_index, const unsigned int count) {
		T* l_arr = original.get();
		T* r_arr = new T[count];
		for (int i = 0; i < count; i++) {
			r_arr[i] = l_arr[start_index + i];
		}
		return Unique<T[]>(r_arr);
	}
}
#endif
