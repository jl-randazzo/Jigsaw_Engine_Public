#ifndef _FIELD_DATA_H_
#define _FIELD_DATA_H_
#include <string>
#include <functional>
#include "Util/etype_info.h"

namespace Jigsaw {
	namespace Marshalling {

		/// <summary>
		/// field_data encapsulates type information about the field in question, and it provides an interface to 
		/// the getters and setters for the field in question. The first parameter of each is a pointer to the object.
		/// </summary>
		class field_data {
		public:
			/// <summary>
			/// By default, the field is not an array. If the field is an array, then the array_size default of -1 indicates an unbounded array.
			/// </summary>
			/// <param name="type"></param>
			/// <param name="is_array"></param>
			/// <param name="array_size"></param>
			field_data(std::string&& name, const Jigsaw::Util::etype_info& type, bool is_array = false, int array_size = -1, bool is_reference = false)
				: name(name), field_type(type), is_array(is_array), array_size(array_size), is_reference(is_reference), is_pointer(false) { }

			/// <summary>
			/// Copy constructor
			/// </summary>
			/// <param name="other"></param>
			field_data(const field_data& other) : name(other.name), field_type(other.field_type), getter(other.getter), setter(other.setter),
				is_array(other.is_array), array_size(other.array_size), is_reference(other.is_reference), is_pointer(other.is_pointer) { }

			/// <summary>
			/// Checks whether the target field is a bounded array organized contiguously in memory. If true, the 'bound_size'
			/// parameter will be populated with the number of elements in the array.
			/// </summary>
			/// <param name="bound_size"></param>
			/// <returns></returns>
			bool IsBoundedArray(size_t* bound_size) const {
				if (is_array && array_size > 0) {
					*bound_size = array_size;
					return true;
				}
				return false;
			}

			/// <summary>
			/// Indicates whether the target field is an array of elements
			/// </summary>
			/// <returns></returns>
			bool IsArray() const {
				return is_array;
			}

			/// <summary>
			/// Returns the etype_info object associated with the target field
			/// </summary>
			/// <returns></returns>
			const Jigsaw::Util::etype_info& GetType() const;

			const Jigsaw::Util::etype_info& field_type;

			const std::string name;

			std::function<void(void*, void*)> setter;

			std::function<void* (void*)> getter;

			int array_size;

			bool is_array;

			bool is_reference;

			bool is_pointer;

		};
	}
}
#endif // !_FIELD_DATA_H_
