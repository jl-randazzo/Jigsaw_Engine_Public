#include "etype_info.h"
#include "Marshalling/JigsawMarshalling.h"

namespace Jigsaw {
	namespace Util {
		bool etype_unsafe_functions::BuildArray(unsigned int n, BYTE** out_array) const {
			if (array_builder) {
				*out_array = array_builder(n);
				return true;
			}
			return false;
		}

		bool etype_unsafe_functions::BuildPointerArray(unsigned int n, BYTE** out_ptr_array) const {
			if (array_builder) {
				*out_ptr_array = static_cast<BYTE*>(ptr_array_builder(n));
				return true;
			}
			return false;
		}

		bool etype_unsafe_functions::BuildVector(unsigned int n, void** out_vector) const {
			if (vector_builder) {
				*out_vector = vector_builder(n);
				return true;
			}
			return false;
		}

		bool etype_unsafe_functions::BuildPointerVector(unsigned int n, void** out_vector) const {
			if (pointer_vector_builder) {
				*out_vector = pointer_vector_builder(n);
				return true;
			}
			return false;
		}

		bool etype_unsafe_functions::GetVectorRawData(void* raw_vector, bool is_pointer, void** out_data) const {
			if (vector_data_getter) {
				*out_data = vector_data_getter(raw_vector, is_pointer);
				return true;
			}
			return false;
		}

		bool etype_unsafe_functions::BuildUnique(void* arg, void** out_ptr) const {
			if (unique_ptr_builder) {
				*out_ptr = unique_ptr_builder(arg);
				return true;
			}
			return false;
		}

		bool etype_unsafe_functions::GetElementCount(void* raw_vector, bool is_pointer, unsigned int* out_number) const {
			if (vector_count_function) {
				*out_number = vector_count_function(raw_vector, is_pointer);
				return true;
			}
			return false;
		}

		bool etype_unsafe_functions::MovePointerIntoPointerArray(void* array, void* element, unsigned int i) const {
			if (move_ptr_into_pointer_array_function) {
				move_ptr_into_pointer_array_function(array, element, i);
				return true;
			}
			return false;
		}

		bool etype_unsafe_functions::Move(BYTE* dst, BYTE* src) const {
			if (mover) {
				mover(dst, src);
				return true;
			}
			return false;
		}

		bool etype_unsafe_functions::Copy(BYTE* dst, BYTE* src) const {
			if (copier) {
				copier(dst, src);
				return true;
			}
			return false;
		}

		bool etype_unsafe_functions::ConstructInPlace(void* dst) const {
			if (construct_in_place_function) {
				construct_in_place_function(dst);
				return true;
			}
			return false;
		}

		bool etype_unsafe_functions::DestructInPlace(void* dst) const {
			if (destruct_in_place_function) {
				destruct_in_place_function(dst);
				return true;
			}
			return false;
		}

		bool etype_unsafe_functions::Delete(void* dst) const {
			if (delete_function) {
				delete_function(dst);
				return true;
			}
			return false;
		}

		void etype_unsafe_functions::SetArrayBuilderFunction(std::function<BYTE* (unsigned int n)>&& array_builder) {
			this->array_builder = array_builder;
		}

		void etype_unsafe_functions::SetVectorBuilderFunction(std::function<void* (unsigned int)>&& vector_builder) {
			this->vector_builder = vector_builder;
		}

		void etype_unsafe_functions::SetPointerVectorBuilderFunction(std::function<void* (unsigned int)>&& pointer_vector_builder) {
			this->pointer_vector_builder = pointer_vector_builder;
		}

		void etype_unsafe_functions::SetMoveFunction(std::function<void(void*, void*)>&& mover) {
			this->mover = mover;
		}

		void etype_unsafe_functions::SetCopyFunction(std::function<void(BYTE*, BYTE*)>&& copier) {
			this->copier = copier;
		}

		void etype_unsafe_functions::SetUniquePointerBuilder(std::function<void* (void*)>&& unique_ptr_builder) {
			this->unique_ptr_builder = unique_ptr_builder;
		}

		void etype_unsafe_functions::SetPointerArrayBuilder(std::function<void* (unsigned int)>&& ptr_array_builder) {
			this->ptr_array_builder = ptr_array_builder;
		}

		void etype_unsafe_functions::SetMovePointerIntoPointerArrayFunction(std::function<void(void*, void*, unsigned int n)>&& move_ptr_into_pointer_array_function) {
			this->move_ptr_into_pointer_array_function = move_ptr_into_pointer_array_function;
		}

		void etype_unsafe_functions::SetVectorCountFunction(std::function<unsigned int(void*, bool)>&& vector_count_function) {
			this->vector_count_function = vector_count_function;
		}

		void etype_unsafe_functions::SetVectorDataGetterFunction(std::function<void* (void*, bool)>&& vector_data_getter) {
			this->vector_data_getter = vector_data_getter;
		}

		void etype_unsafe_functions::SetDestructInPlaceFunction(std::function<void(void*)>&& destruct_in_place_function) {
			this->destruct_in_place_function = destruct_in_place_function;
		}

		void etype_unsafe_functions::SetDeleteFunction(std::function<void(void*)>&& delete_function) {
			this->delete_function = delete_function;
		}

		void etype_unsafe_functions::SetConstructInPlaceFunction(std::function<void(void*)>&& construct_in_place_function) {
			this->construct_in_place_function = construct_in_place_function;
		}

		etype_info::etype_info(const type_info& t_i, const std::string& name, const size_t size, const size_t align, const etype_properties& properties, const etype_unsafe_functions& unsafe_functions, const std::function<const Jigsaw::Util::etype_info* (void*)>&& instanceTypeFunction)
			: base_inf(t_i), name(name), size(size), align(align), properties(properties), unsafe_functions(unsafe_functions), instanceTypeFunction(instanceTypeFunction) {}

		std::mutex& etype_info::GetLock() {
			static std::mutex mut;
			return mut;
		}

		std::unordered_map<std::string, const etype_info*>& etype_info::GetQualifiedNameMap() {
			static std::unordered_map<std::string, const etype_info*> map;
			return map;
		}

		std::unordered_map<std::type_index, const etype_info*>& etype_info::GetMap() {
			static std::unordered_map<std::type_index, const etype_info*> map;
			return map;
		};

		const etype_unsafe_functions& etype_info::GetUnsafeFunctions() const {
			return unsafe_functions;
		}

		const etype_properties& etype_info::GetProperties() const {
			return properties;
		}

		const etype_info& etype_info::GetByQualifiedName(const std::string& name) {
			std::scoped_lock<std::mutex> scoped_lock(etype_info::GetLock());
			return *GetQualifiedNameMap().at(name);
		}

		const Jigsaw::Util::etype_info* etype_info::GetInstanceType(void* instance) const {
			return instanceTypeFunction(instance);
		}

		const std::string etype_info::GetQualifiedName() const {
			return name;
		}

		/// <summary>
		/// It was decided to leave the etype_index in an unsafe state if a proper constructor is not called. 
		/// It will throw an exception earlier, which will make it easier to detect the problem
		/// </summary>
		/// <returns></returns>
		etype_index::etype_index() noexcept : _Tptr(nullptr) {}

		std::string* etype_index_ToString(etype_index* index) {
			return new std::string(index->Get().GetQualifiedName());
		}

		void etype_index_FromString(etype_index* index, std::string* string) {
			*index = etype_index(etype_info::GetByQualifiedName(*string));
		}

		START_REGISTER_SERIALIZABLE_CLASS(etype_index)
			REGISTER_CUSTOM_FIELD_MAPPER(etype_index, std::string, type_name, etype_index_ToString, etype_index_FromString)
			END_REGISTER_SERIALIZABLE_CLASS(etype_index)
	}
}

