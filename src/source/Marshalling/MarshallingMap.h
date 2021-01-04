#ifndef _MARSHALLING_MAP_H_
#define _MARSHALLING_MAP_H_

#include <map>
#include <string>
#include "field_data.h"

namespace Jigsaw {
	namespace Marshalling {
		/// <summary>
		/// The MarshallingMap provides a view into the fields of a given type, and a method for generating new instances of a given type
		/// without the implementing class needing to know the exact details of the class in question. 
		/// </summary>
		class MarshallingMap {
		public:
			/// <summary>
			/// Primary constructor
			/// </summary>
			/// <param name="type"></param>
			/// <param name="manufacturer"></param>
			MarshallingMap(const Jigsaw::Util::etype_info& type, std::function<void* ()> manufacturer) : type(type), manufacturer(manufacturer), field_map() {}

			/// <summary>
			/// Constructor for objects that have context-rooted intialization
			/// </summary>
			/// <param name="type"></param>
			/// <param name="manufacturer"></param>
			MarshallingMap(const Jigsaw::Util::etype_info& type, std::function<void* (void*)> context_root_manufacturer)
				: type(type), manufacturer(), context_root_manufacturer(context_root_manufacturer), field_map() {}

			/// <summary>
			/// Copy constructor
			/// </summary>
			/// <param name="other"></param>
			MarshallingMap(const MarshallingMap& other) : type(other.type), field_map(other.field_map), manufacturer(other.manufacturer),
				context_root_manufacturer(other.context_root_manufacturer), parent_classes(other.parent_classes) {}

			/// <summary>
			/// For a MarshallingMap that has not been stored in the MarshallingRegistry, new field data can be registered.
			/// </summary>
			/// <param name="name"></param>
			/// <param name="f_data"></param>
			void RegisterFieldData(const std::string&& name, const field_data* f_data);

			/// <summary>
			/// For a MarshallingMap that has not been stored in the MarshallingRegistry, parent types can be registered. Do not call this method directly.
			/// </summary>
			/// <param name="parent"></param>
			void RegisterParentClass(const Jigsaw::Util::etype_info& parent);

			/// <summary>
			/// If a field name with the passed in 'name' exists, then a pointer to it is returned. Else, nullptr is returned.
			/// </summary>
			/// <param name="name"></param>
			/// <returns></returns>
			const field_data* GetFieldDataByName(const std::string&& name) const;

			/// <summary>
			/// Returns all field_data objects associated with this type and its parents
			/// </summary>
			/// <returns></returns>
			const std::vector<const field_data*> GetFields() const;

			/// <summary>
			/// Generates the default representation of the given object. If the object is context-rooted, then the 'void*'
			/// argument is used to initialize the object. 
			/// </summary>
			/// <returns></returns>
			void* Manufacture(void* context) const;

			/// <summary>
			/// Returns the type of the object described by this MarshallingMap
			/// </summary>
			/// <returns></returns>
			const Jigsaw::Util::etype_info& GetType() const;

			/// <summary>
			/// Returns true if the passed in 'parent' type is nested anywhere within the 'parent_classes' 
			/// </summary>
			/// <param name="parent"></param>
			/// <returns></returns>
			const bool InheritsFrom(const Jigsaw::Util::etype_info& parent) const;

			/// <summary>
			/// Returns true if the target object is initialized with a contextual link to the enclosing object.
			/// </summary>
			/// <returns></returns>
			const bool IsContextInitialized() const;

		private:
			/// <summary>
			/// Internal method that populates the return_vector with all fields contained in this type and its parents
			/// </summary>
			/// <param name="return_vector"></param>
			void GetFields(std::vector<const field_data*>& return_vector) const;

			/// <summary>
			/// Internal method used to examine the parent_classes to find the given field by name. Returns the first occurrence of a field of that name.
			/// </summary>
			/// <param name="name"></param>
			/// <param name="f_data"></param>
			/// <returns></returns>
			const void GetFieldDataByName(const std::string& name, const field_data** f_data) const;

			const Jigsaw::Util::etype_info& type;
			std::vector<Jigsaw::Util::etype_index> parent_classes;
			std::map<std::string, const field_data*> field_map;
			std::function<void* ()> manufacturer;
			std::function<void* (void*)> context_root_manufacturer;
		};

	}
}
#endif