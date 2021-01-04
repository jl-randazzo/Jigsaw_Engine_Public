#ifndef _ETYPE_INFO_H_
#define _ETYPE_INFO_H_

#include <typeinfo>
#include <typeindex>
#include <functional>
#include <mutex>
#include <unordered_map>
#include "BYTE.h"

namespace Jigsaw {
	namespace Util {
		class etype_unsafe_functions {
		public:
			etype_unsafe_functions() {}

			/// <summary>
			/// Builds an array from the enclosed type.
			/// </summary>
			/// <param name="n">The number of elements in the array</param>
			/// <param name="out_array"></param>
			/// <returns>False if the enclosed type is not default constructible. True if a pointer to an array n items long was stored in out_array</returns>
			bool BuildArray(unsigned int n, BYTE** out_array) const;

			/// <summary>
			/// Builds an array of Unique pointers from the enclosed type.
			/// </summary>
			/// <param name="n">The number of elements in the array</param>
			/// <param name="out_array"></param>
			/// <returns>False if the enclosed type is not default constructible. True if a pointer to an array n items long was stored in out_array</returns>
			bool BuildPointerArray(unsigned int n, BYTE** out_ptr_array) const;

			/// <summary>
			/// Builds an std::vector out of an existing array, emplacing the content of that array in a new vector object. 
			/// </summary>
			/// <param name="array"></param>
			/// <param name="n"></param>
			/// <param name="out_vector"></param>
			/// <returns></returns>
			bool BuildVector(unsigned int n, void** out_vector) const;

			/// <summary>
			/// Builds an std::vector out of an existing array of pointers, emplacing the content of those unique pointers in a new vector object. 
			/// </summary>
			/// <param name="array"></param>
			/// <param name="n"></param>
			/// <param name="out_vector"></param>
			/// <returns></returns>
			bool BuildPointerVector(unsigned int n, void** out_vector) const;

			/// <summary>
			/// Returns a pointer to the raw data of an std::vector object matching this etype
			/// </summary>
			/// <param name="raw_vector"></param>
			/// <param name="is_pointer"></param>
			/// <param name="out_data"></param>
			/// <returns></returns>
			bool GetVectorRawData(void* raw_vector, bool is_pointer, void** out_data) const;

			/// <summary>
			/// Builds an array from the enclosed type.
			/// </summary>
			/// <param name="n">The number of elements in the array</param>
			/// <param name="out_array"></param>
			/// <returns>False if the enclosed type is not default constructible. True if a pointer to an array n items long was stored in out_array</returns>
			bool BuildUnique(void* arg, void** out_ptr) const;

			/// <summary>
			/// Counts the elements of a vector of elements of the specified etype or a vector of pointers to that etype
			/// </summary>
			/// <param name="raw_vector"></param>
			/// <param name="is_pointer"></param>
			/// <param name="out_number"></param>
			/// <returns>False if the etype is not copy constructible and therefore cannot be an std::vector type</returns>
			bool GetElementCount(void* raw_vector, bool is_pointer, unsigned int* out_number) const;

			/// <summary>
			/// Moves a Jigsaw::Unique<T> relevant to this etype_info into an array of these pointers at index i
			/// This is an unsafe operation and should only be called in cases for marshalling/unmarshalling
			/// </summary>
			/// <param name="array"></param>
			/// <param name="element"></param>
			/// <param name="i"></param>
			/// <returns></returns>
			bool MovePointerIntoPointerArray(void* array, void* element, unsigned int i) const;

			/// <summary>
			/// If the given type is movable, it is moved from src to dst. It is assumed that the passed in dst and src are pointers to valid instantiations of this type.
			/// </summary>
			/// <param name="dst"></param>
			/// <param name="src"></param>
			/// <returns>True if move was successful, false if the type is not move assignable</returns>
			bool Move(BYTE* dst, BYTE* src) const;

			/// <summary>
			/// Copies the value at 'src' for the given type into the allocated destination 'dst'
			/// </summary>
			/// <param name="dst"></param>
			/// <param name="src"></param>
			/// <returns>False if the type is not copy assignable</returns>
			bool Copy(BYTE* dst, BYTE* src) const;

			/// <summary>
			/// If the given type is move assignable and default constructable, then a default construction will happen at the destination
			/// </summary>
			/// <param name="dst"></param>
			/// <returns>Returns false if the given type is not default constructible or move assignable</returns>
			bool ConstructInPlace(void* dst) const;

			/// <summary>
			/// Calls the destructor for the given type on the passed in destination. THIS DOES NOT DEALLOCATE MEMORY
			/// </summary>
			/// <param name="dst"></param>
			/// <returns>Returns false if the given type is not destructible</returns>
			bool DestructInPlace(void* dst) const;

			/// <summary>
			/// Calls the destructor for the given type on the passed in destination. Deallocates memory in the process
			/// </summary>
			/// <param name="dst"></param>
			/// <returns>Returns false if the given type is not destructible</returns>
			bool Delete(void* dst) const;

			void SetArrayBuilderFunction(std::function<BYTE* (unsigned int n)>&& array_builder);

			void SetVectorBuilderFunction(std::function<void* (unsigned int)>&& vector_builder);

			void SetPointerVectorBuilderFunction(std::function<void* (unsigned int)>&& pointer_vector_builder);

			void SetMoveFunction(std::function<void(void*, void*)>&& mover);

			void SetCopyFunction(std::function<void(BYTE*, BYTE*)>&& copier);

			void SetUniquePointerBuilder(std::function<void* (void*)>&& unique_ptr_builder);

			void SetPointerArrayBuilder(std::function<void* (unsigned int)>&& ptr_array_builder);

			void SetMovePointerIntoPointerArrayFunction(std::function<void(void*, void*, unsigned int n)>&& move_ptr_into_pointer_array_function);

			void SetVectorCountFunction(std::function<unsigned int(void*, bool)>&& vector_count_function);

			void SetVectorDataGetterFunction(std::function<void* (void*, bool)>&& vector_data_getter);

			void SetConstructInPlaceFunction(std::function<void(void*)>&& construct_in_place_function);

			void SetDestructInPlaceFunction(std::function<void(void*)>&& destruct_in_place_function);
			
			void SetDeleteFunction(std::function<void(void*)>&& delete_function);

		private:
			std::function<void(void*)> delete_function;
			std::function<BYTE* (unsigned int)> array_builder;
			std::function<void* (unsigned int)> vector_builder;
			std::function<void* (unsigned int)> pointer_vector_builder;
			std::function<void(void*, void*)> mover;
			std::function<void(BYTE*, BYTE*)> copier;
			std::function<void* (void*)> unique_ptr_builder;
			std::function<void* (unsigned int)> ptr_array_builder;
			std::function<void(void*, void*, unsigned int n)> move_ptr_into_pointer_array_function;
			std::function<unsigned int(void*, bool)> vector_count_function;
			std::function<void* (void*, bool)> vector_data_getter;
			std::function<void(void*)> construct_in_place_function;
			std::function<void(void*)> destruct_in_place_function;

		};

		struct etype_properties {
			bool is_enum;
		};

		/// <summary>
		/// An extension of C++'s "type_info." etype_info encapsulates additional data unique to one runtime type. This can include the type's
		/// flat size in memory. Default construction and move assignment of the given type are included if those functions are available for 
		/// the given type. 
		/// </summary>
		class etype_info {
		private:
			etype_info(const type_info& t_i, const std::string& name, const size_t size, const size_t align, const etype_properties& properties, const etype_unsafe_functions& unsafe_functions, const std::function<const Jigsaw::Util::etype_info* (void*)>&& instanceTypeFunction);

			etype_info(const etype_info& other) = delete;

			static std::mutex& GetLock();

			static std::unordered_map<std::string, const etype_info*>& GetQualifiedNameMap();

			static std::unordered_map<std::type_index, const etype_info*>& GetMap();

			/// <summary>
			/// Returns a function that will examine an instance and return the inherited or non-inherited type of the instance.
			/// It is assumed that the passed in 'instance' argument IS an instance of the given type argument 'T' associated with that
			/// base etype_info object. 
			/// </summary>
			/// <typeparam name="T"></typeparam>
			/// <returns></returns>
			template<typename T>
			static constexpr std::function<const Jigsaw::Util::etype_info* (void*)> GetInstanceTypeFunction() {
				return [](void* instance) -> const Jigsaw::Util::etype_info* {
					const Jigsaw::Util::etype_info* et_i = nullptr;
					T* t_instance = static_cast<T*>(instance);
					if (t_instance) {
						std::type_index t_index = typeid(*t_instance);
						auto iter = GetMap().find(t_index);
						et_i = iter == GetMap().end() ? et_i : (*iter).second;
					}
					return et_i;
				};
			}

			/// <summary>
			/// This internal method assigns a series of type-dependent functions to the 'unsafe_functions' facet of the etype_info object
			/// </summary>
			/// <typeparam name="T"></typeparam>
			/// <param name="unsafe_type_functions"></param>
			template<typename T>
			_Requires_lock_held_(this->GetLock) static constexpr void PopulateTypeFunctions(etype_unsafe_functions& unsafe_type_functions) {

				unsafe_type_functions.SetMovePointerIntoPointerArrayFunction(
					[](void* dst_array, void* pointer, unsigned int n) -> void {
						T** pointer_array = static_cast<T**>(dst_array);
						T* unique_element = static_cast<T*>(pointer);
						T** position = pointer_array + n;
						*position = unique_element;
					}
				);

				unsafe_type_functions.SetUniquePointerBuilder(
					[](void* arg) -> void* {
						T* t = static_cast<T*>(arg);
						return static_cast<void*>(new Jigsaw::Unique<T>(static_cast<T*>(t)));
					}
				);

				if constexpr (std::is_destructible<T>::value) {
					unsafe_type_functions.SetDestructInPlaceFunction(
						[](void* dst) -> void {
							T& t = *static_cast<T*>(dst);
							t.~T();
						}
					);

					unsafe_type_functions.SetDeleteFunction(
						[](void* dst) -> void {
							T* t = static_cast<T*>(dst);
							delete t;
						}
					);
				}

				if constexpr (std::is_default_constructible<T>::value) {
					unsafe_type_functions.SetConstructInPlaceFunction(
						[](void* dst) -> void {
							new(dst) T;
						}
					);

					unsafe_type_functions.SetArrayBuilderFunction(
						[](unsigned int n) -> BYTE* {
							return static_cast<BYTE*>(static_cast<void*>(new T[n]));
						}
					);

					unsafe_type_functions.SetPointerArrayBuilder(
						[](unsigned int n) -> void* {
							return new T * [n];
						}
					);

					unsafe_type_functions.SetPointerVectorBuilderFunction(
						[](unsigned int n) -> void* {
							std::vector<T*>* vector = new std::vector<T*>(n);
							return vector;
						}
					);

					if constexpr (!std::is_same<bool, T>::value) {
						unsafe_type_functions.SetVectorDataGetterFunction(
							[](void* raw_vector, bool is_pointer) -> void* {
								if (is_pointer) {
									std::vector<T*>& vector = *static_cast<std::vector<T*>*>(raw_vector);
									T** data = vector.data();
									void* ret_val = static_cast<void*>(data);
									return ret_val;
								}
								else {
									std::vector<T>& vector = *static_cast<std::vector<T>*>(raw_vector);
									T* data = vector.data();
									void* ret_val = static_cast<void*>(data);
									return ret_val;
								}
							}
						);
					}

					if constexpr (std::is_copy_assignable<T>::value) {
						unsafe_type_functions.SetCopyFunction(
							[](BYTE* dst, BYTE* src) -> void {
								T& _dst = *static_cast<T*>((void*)dst);
								T& _src = *static_cast<T*>((void*)src);
								_dst = _src;
							}
						);
					}

					if constexpr (std::is_copy_constructible<T>::value) {

						unsafe_type_functions.SetVectorBuilderFunction(
							[](unsigned int n) -> void* {
								std::vector<T>* vector = new std::vector<T>(n);
								return vector;
							}
						);

						unsafe_type_functions.SetVectorCountFunction(
							[](void* raw_vector, bool is_pointer) -> unsigned int {
								if (is_pointer) {
									std::vector<T*>* vector = static_cast<std::vector<T*>*>(raw_vector);
									return vector->size();
								}
								else {
									std::vector<T>* vector = static_cast<std::vector<T>*>(raw_vector);
									return vector->size();
								}
							}
						);
					} // END copy constructible
				} // END move assignable

				if constexpr (std::is_move_assignable<T>::value) {
					unsafe_type_functions.SetMoveFunction(
						[](void* dst, void* src) -> void {
							*static_cast<T*>(dst) = std::move(*static_cast<T*>(src));
						}
					);
				}

			}

			const etype_properties properties;
			const std::string name;
			const std::function<const Jigsaw::Util::etype_info* (void*)> instanceTypeFunction;
			const etype_unsafe_functions& unsafe_functions;
			const type_info& base_inf;

		public:

			/// <summary>
			/// The 'Id' function returns the etype_info of the type parameter. Unlike reflection/introspection in other languages,
			/// this is done at the moment that the given type is requested. That means that the 'etype_info' objects stored in the Map
			/// can and do change throughout the execution lifetime. Once a type is registered, though, it is registered forever, and it's immutable. 
			/// 
			/// Since retrieving types by fully-qualified name is restricted to types that have already been registered through the execution
			/// of this function, it is recommended that any type you are intending to retrieve by qualified name be registered statically by calling
			/// this method at the boot time of your program. 
			/// 
			/// Retrieving by fully-qualified name (GetByQualifiedName) during the static initialization of the program yields completely undefined behavior.
			///
			/// Default-constructible types are given an array_builder function. Move-assignable types are given a move_function. 
			/// </summary>
			/// <typeparam name="T"></typeparam>
			/// <returns></returns>
			template<typename T>
			static const etype_info& Id() {
				typedef std::remove_const<T>::type T_;
				std::scoped_lock<std::mutex> lock(etype_info::GetLock());

				const type_info& inf = typeid(T_);
				std::type_index ind(inf);

				auto iter = GetMap().find(ind);
				if (iter == GetMap().end()) {
					etype_unsafe_functions* unsafe_type_functions = new etype_unsafe_functions;
					PopulateTypeFunctions<T_>(*unsafe_type_functions);

					std::string full_name(inf.name());
					int substr = full_name.find_first_of(' ') + 1;
					full_name = full_name.substr(substr);

					etype_properties properties;
					properties.is_enum = std::is_enum_v<T>;

					auto func = GetInstanceTypeFunction<T_>();
					const etype_info* et_i = new etype_info(inf, full_name, sizeof(T_), alignof(T_), properties, *unsafe_type_functions, std::move(func));
					GetMap().insert(std::make_pair(ind, et_i));
					GetQualifiedNameMap().insert(std::make_pair(full_name, et_i));
				}

				return *GetMap().at(inf);
			};

			/// <summary>
			/// Returns the structured object for calling unsafe initialization, movement, and array building functions
			/// </summary>
			/// <returns></returns>
			const etype_unsafe_functions& GetUnsafeFunctions() const;

			/// <summary>
			/// Fetches the properties inherent to this type
			/// </summary>
			/// <returns></returns>
			const etype_properties& GetProperties() const;

			/// <summary>
			/// This function is available after boot time. Any classes you are expecting to fetch by fully-qualified name need to have
			/// registrations that take place statically at boot time. 
			/// </summary>
			/// <param name="name"></param>
			/// <returns></returns>
			static const etype_info& GetByQualifiedName(const std::string& name);

			/// <summary>
			/// Returns the etype_info associated with an object instance. If it is an inherited type, it will be returned
			/// </summary>
			/// <param name="instance"></param>
			/// <returns></returns>
			const Jigsaw::Util::etype_info* GetInstanceType(void* instance) const;

			/// <summary>
			/// Return the qualified name of this etype_info
			/// </summary>
			/// <returns></returns>
			const std::string GetQualifiedName() const;

			bool operator<(const etype_info& other) const noexcept {
				return base_inf.before(other.base_inf);
			}

			const type_info& GetBase() const noexcept {
				return base_inf;
			}

			const bool operator==(const etype_info& other) const {
				return base_inf == other.base_inf;
			}

			const bool operator!=(const etype_info& other) const {
				return base_inf != other.base_inf;
			}

			const size_t size;
			const size_t align;

		};



		// typeindex standard header

		// Copyright (c) Microsoft Corporation.
		// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
				/**
				* This implementation is an adaptation of Microsoft's type_index implementation to wrap etype_info which contains additional
				* details relevant to the type. I take no credit for the majority of the work done here; it is just a small extension.
				*/
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#if _HAS_CXX20
#include <compare>
#endif // _HAS_CXX20

		class etype_index { // wraps a typeinfo for indexing
		public:
			/// <summary>
			/// Default assignment yields an invalid type.
			/// </summary>
			/// <returns></returns>
			etype_index() noexcept;

			etype_index(const etype_info& _Tinfo) noexcept : _Tptr(&_Tinfo) {}

			etype_index(const etype_index& other) noexcept : _Tptr(other._Tptr) {}

			/// <summary>
			/// 
			/// </summary>
			/// <param name="other"></param>
			/// <returns></returns>
			etype_index& operator=(const etype_index& other) {
				_Tptr = other._Tptr;
				return *this;
			}

			_NODISCARD size_t hash_code() const noexcept {
				return (_Tptr->GetBase()).hash_code();
			}

			_NODISCARD const char* name() const noexcept {
				return (_Tptr->GetBase()).name();
			}

			_NODISCARD bool operator==(const etype_index& _Right) const noexcept {
				return (_Tptr->GetBase()) == (_Right._Tptr->GetBase());
			}

			_NODISCARD bool operator!=(const etype_index& _Right) const noexcept {
				return !(*this == _Right);
			}

			_NODISCARD bool operator<(const etype_index& _Right) const noexcept {
				return *_Tptr < *_Right._Tptr;
			}

			_NODISCARD bool operator>=(const etype_index& _Right) const noexcept {
				return !(*this < _Right);
			}

			_NODISCARD bool operator>(const etype_index& _Right) const noexcept {
				return _Right < *this;
			}

			_NODISCARD bool operator<=(const etype_index& _Right) const noexcept {
				return !(_Right < *this);
			}

			inline operator const etype_info& () {
				return Get();
			}

			const etype_info& Get() const {
				return *_Tptr;
			}

			operator const etype_info& () const {
				return Get();
			}

		private:
			const etype_info* _Tptr;
		};

		std::string* etype_index_ToString(etype_index* index);
		void etype_index_FromString(etype_index* index, std::string* string);

		_STL_RESTORE_CLANG_WARNINGS
#endif // _STL_COMPILER_PREPROCESSOR
	}
}

// STRUCT TEMPLATE SPECIALIZATION hash
_STD_BEGIN
template <>
struct hash<Jigsaw::Util::etype_index> {
	_CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef Jigsaw::Util::etype_index _ARGUMENT_TYPE_NAME;
	_CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef size_t _RESULT_TYPE_NAME;

	_NODISCARD size_t operator()(const Jigsaw::Util::etype_index& _Keyval) const noexcept {
		return _Keyval.hash_code();
	}
};
_STD_END


#endif // !_ETYPE_INFO_H_
