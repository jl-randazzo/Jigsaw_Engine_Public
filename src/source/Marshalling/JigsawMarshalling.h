#ifndef _JIGSAW_MARSHALLING_H_
#define _JIGSAW_MARSHALLING_H_

#include "Ref.h"
#include "Util/etype_info.h"
#include "System/UID.h"
#include "Util/BYTE.h"
#include "MarshallingRegistry.h"
#include "MARSHALLING_RESULTS.h"
#include <functional>
#include <unordered_map>
#include <map>
#include <string>
#include <sstream>
#include <vector>

/**
* Semantics:
* In the header file:
* class Classname {
* public:
*     int value_name; // public, non-const, non-ref field declarations
*	  Unique<array_type[]> unbounded_array;
*	  array_type[3] bounded_array;
* }
*
* In a cpp file:
*
* START_REGISTER_SERIALIZABLE_CLASS(Classname)
* REGISTER_CLASS_ARRAY(Classname, array_type, bounded_array) // registers an unbounded array of array_type objects that are housed in a unique pointer in the object
* REGISTER_CLASS_BOUNDED_ARRAY(Classname, array_type, 3, valueName) // registers a bounded array of array_type objects that are housed contiguously in the object
* END_REGISTER_SERIALIZABLE_CLASS(Classname)
*/

namespace Jigsaw {
	namespace Marshalling {

#define EXPANDER(fieldname) fieldname

#define D_EXPANDER(classname, field_name) EXPANDER(classname)EXPANDER(_)EXPANDER(field_name)

#define CALL_BASE_MOVE_ASSIGNMENT(base_class, param)\
base_class::operator=(std::move(*static_cast<base_class*>(&param)));

#define CALL_BASE_COPY_ASSIGNMENT(base_class, param)\
base_class::operator=(*static_cast<base_class*>(&param));

#define CONST_FIELD_ASSIGN(field_name)\
const_cast<std::remove_const<decltype(field_name)>::type&>(field_name)

#define START_REGISTER_SERIALIZABLE_CLASS(classname) \
namespace _____ {\
Jigsaw::Marshalling::MarshallingMap* EXPANDER(classname)_marshalling_map = new Jigsaw::Marshalling::MarshallingMap(Jigsaw::Util::etype_info::Id<classname>(), []() -> void* { return new EXPANDER(classname)(); });\
constexpr classname* EXPANDER(classname)_Cast(void* a) { return static_cast<EXPANDER(classname)*>(a); }; 

#define START_REGISTER_CONTEXT_ROOTED_SERIALIZABLE_CLASS(classname, context_classname) \
namespace _____ {\
Jigsaw::Marshalling::MarshallingMap* EXPANDER(classname)_marshalling_map = new Jigsaw::Marshalling::MarshallingMap(Jigsaw::Util::etype_info::Id<classname>(), [](void* context) -> void* { return new EXPANDER(classname)(static_cast<context_classname*>(context)); });\
constexpr classname* EXPANDER(classname)_Cast(void* a) { return static_cast<EXPANDER(classname)*>(a); }; 

#define REGISTER_SERIALIZABLE_CHILD_OF(child_class, parent_class) \
struct D_EXPANDER(child_class, parent_class)_child_parent_def {\
	D_EXPANDER(child_class, parent_class)_child_parent_def() {\
		if constexpr (std::is_base_of<parent_class, child_class>::value){\
			EXPANDER(child_class)_marshalling_map->RegisterParentClass(Jigsaw::Util::etype_info::Id<parent_class>());\
		} else {\
			throw "Incompatible parent-child relationship attempted to be filed into the marshalling_map";\
		} \
	}; \
}; \
D_EXPANDER(child_class, parent_class)_child_parent_def D_EXPANDER(child_class, parent_class)_child_parent_def_register;

#define REGISTER_SERIALIZABLE_FIELD(classname, _type, field_name) \
\
struct D_EXPANDER(classname, field_name)_def { \
	D_EXPANDER(classname, field_name)_def() { \
		Jigsaw::Marshalling::field_data* f_data = new Jigsaw::Marshalling::field_data(#field_name, Jigsaw::Util::etype_info::Id<_type>(), false);\
		f_data->getter = [](void* a) -> void* { \
			classname* c = EXPANDER(classname)_Cast(a);\
			return static_cast<void*>(const_cast<_type*>(&c->EXPANDER(field_name))); }; \
		f_data->setter = [](void* a, void* b) -> void { const_cast<_type&>(EXPANDER(classname)_Cast(a)->EXPANDER(field_name)) = *static_cast<std::remove_const<decltype(classname::field_name)>::type*>(b); };\
		EXPANDER(classname)_marshalling_map->RegisterFieldData(std::string(#field_name), f_data);\
	}; \
}; \
\
D_EXPANDER(classname, field_name)_def D_EXPANDER(classname, field_name)_def_build;

#define REGISTER_SERIALIZABLE_POINTER(classname, type, field_name) \
\
struct D_EXPANDER(classname, field_name)_def { \
	D_EXPANDER(classname, field_name)_def() { \
		Jigsaw::Marshalling::field_data* f_data = new Jigsaw::Marshalling::field_data(#field_name, Jigsaw::Util::etype_info::Id<type>());\
		f_data->is_pointer = true; \
		f_data->getter = [](void* a) -> void* { return static_cast<void*>((EXPANDER(classname)_Cast(a)->EXPANDER(field_name)).get()); }; \
		f_data->setter = [](void* a, void* b) -> void { EXPANDER(classname)_Cast(a)->EXPANDER(field_name) = Jigsaw::Unique<type>(static_cast<type*>(b)); };\
		EXPANDER(classname)_marshalling_map->RegisterFieldData(std::string(#field_name), f_data);\
	}; \
}; \
\
D_EXPANDER(classname, field_name)_def D_EXPANDER(classname, field_name)_def_build;

#define REGISTER_SERIALIZABLE_VECTOR(classname, type, field_name) \
\
struct D_EXPANDER(classname, field_name)_def { \
	D_EXPANDER(classname, field_name)_def() { \
		Jigsaw::Marshalling::field_data* f_data = new Jigsaw::Marshalling::field_data(#field_name, Jigsaw::Util::etype_info::Id<type>(), true);\
		f_data->getter = [](void* a) -> void* { return static_cast<void*>(&(EXPANDER(classname)_Cast(a)->EXPANDER(field_name))); }; \
		f_data->setter = [](void* a, void* b) -> void { EXPANDER(classname)_Cast(a)->EXPANDER(field_name) = std::move(*static_cast<std::vector<type>*>(b)); };\
		EXPANDER(classname)_marshalling_map->RegisterFieldData(std::string(#field_name), f_data);\
	}; \
}; \
\
D_EXPANDER(classname, field_name)_def D_EXPANDER(classname, field_name)_def_build;

#define REGISTER_SERIALIZABLE_POINTER_VECTOR(classname, type, field_name) \
\
struct D_EXPANDER(classname, field_name)_def { \
	D_EXPANDER(classname, field_name)_def() { \
		Jigsaw::Marshalling::field_data* f_data = new Jigsaw::Marshalling::field_data(#field_name, Jigsaw::Util::etype_info::Id<type>(), true);\
		f_data->is_pointer = true; \
		f_data->getter = [](void* a) -> void* { return static_cast<void*>(&(EXPANDER(classname)_Cast(a)->EXPANDER(field_name))); }; \
		f_data->setter = [](void* a, void* b) -> void { EXPANDER(classname)_Cast(a)->EXPANDER(field_name) = std::move(*static_cast<std::vector<type*>*>(b)); };\
		EXPANDER(classname)_marshalling_map->RegisterFieldData(std::string(#field_name), f_data);\
	}; \
}; \
\
D_EXPANDER(classname, field_name)_def D_EXPANDER(classname, field_name)_def_build;

#define REGISTER_SERIALIZABLE_BOUNDED_ARRAY(classname, type, bound, field_name) \
\
struct D_EXPANDER(classname, field_name)_def { \
	D_EXPANDER(classname, field_name)_def() { \
		const Jigsaw::Util::etype_info& t_info = Jigsaw::Util::etype_info::Id<type>();\
		Jigsaw::Marshalling::field_data* f_data = new Jigsaw::Marshalling::field_data(#field_name, t_info, true, bound);\
		f_data->getter = [](void* a) -> void* { return static_cast<void*>((EXPANDER(classname)_Cast(a)->EXPANDER(field_name))); }; \
		f_data->setter = [=, &t_info](void* a, void* b) -> void { \
			BYTE* dst = static_cast<BYTE*>(static_cast<void*>(EXPANDER(classname)_Cast(a)->EXPANDER(field_name)));\
			BYTE* src = static_cast<BYTE*>(b);\
			size_t size = t_info.size;\
			for(int i = 0; i < bound; i++){ \
				BYTE* dst_ = dst + (i * size);\
				BYTE* src_ = src + (i * size);\
				t_info.GetUnsafeFunctions().Move(dst_, src_);\
			}\
		};\
		EXPANDER(classname)_marshalling_map->RegisterFieldData(std::string(#field_name), f_data);\
	};\
}; \
\
D_EXPANDER(classname, field_name)_def D_EXPANDER(classname, field_name)_def_build;

#define END_REGISTER_SERIALIZABLE_CLASS(classname) \
	struct EXPANDER(classname)_register {\
		EXPANDER(classname)_register() {\
			Jigsaw::Marshalling::MarshallingRegistry::RegisterMap(Jigsaw::Util::etype_info::Id<classname>(), *EXPANDER(classname)_marshalling_map);\
			delete EXPANDER(classname)_marshalling_map;\
		};\
	};\
	EXPANDER(classname)_register EXPANDER(classname)_register_build;\
};

#define REGISTER_CUSTOM_FIELD_MAPPER(classname, alias_type, alias_name, g_getter, g_setter)\
\
struct D_EXPANDER(classname, alias_name)_def { \
	D_EXPANDER(classname, alias_name)_def() { \
		Jigsaw::Marshalling::field_data* f_data = new Jigsaw::Marshalling::field_data(#alias_name, Jigsaw::Util::etype_info::Id<alias_type>());\
		f_data->getter = [](void* a) -> void* { return static_cast<void*>(g_getter(EXPANDER(classname)_Cast(a))); }; \
		f_data->setter = [](void* a, void* b) -> void { g_setter(EXPANDER(classname)_Cast(a), static_cast<alias_type*>(b)); };\
		EXPANDER(classname)_marshalling_map->RegisterFieldData(std::string(#alias_name), f_data);\
	}; \
}; \
\
D_EXPANDER(classname, alias_name)_def D_EXPANDER(classname, alias_name)_def_build;

#define SERIALIZABLE_ASSET_REFERENCE(asset_type, asset_ref_name) \
Jigsaw::Ref<EXPANDER(asset_type)> asset_ref_name; \
Jigsaw::System::UID EXPANDER(asset_ref_name)_id;

		///
		/// Macro for registering fields of type Ref<? extends DataAsset>
#define REGISTER_SERIALIZABLE_ASSET_REFERENCE(Classname, asset_type, field_name) \
\
struct D_EXPANDER(Classname, field_name)_def { \
	D_EXPANDER(Classname, field_name)_def() { \
		Jigsaw::Marshalling::field_data* f_data = new Jigsaw::Marshalling::field_data(#field_name, Jigsaw::Util::etype_info::Id<Jigsaw::Ref<asset_type>>(), false, -1, true); \
		f_data->getter = [](void* a) -> void* { return static_cast<void*>(EXPANDER(Classname)_Cast(a)->field_name.get()); }; \
		f_data->setter = [](void* a, void* b) -> void { const_cast<Jigsaw::Ref<asset_type>&>(EXPANDER(Classname)_Cast(a)->field_name) = std::dynamic_pointer_cast<asset_type>(*static_cast<Jigsaw::Ref<Jigsaw::Assets::DataAsset>*>(b)); }; \
		EXPANDER(Classname)_marshalling_map->RegisterFieldData(std::string(#field_name), f_data); \
	}; \
}; \
 \
D_EXPANDER(Classname, field_name)_def D_EXPANDER(Classname, field_name)_def_build;

///
/// Macro for registering fields of type Ref<? extends DataAsset>
#define REGISTER_SERIALIZABLE_REFERENCE(Classname, asset_type, field_name) \
\
struct D_EXPANDER(Classname, field_name)_def { \
	D_EXPANDER(Classname, field_name)_def() { \
		Jigsaw::Marshalling::field_data* f_data = new Jigsaw::Marshalling::field_data(#field_name, Jigsaw::Util::etype_info::Id<Jigsaw::Ref<asset_type>>(), false, -1, true); \
		f_data->getter = [](void* a) -> void* { return static_cast<void*>((EXPANDER(Classname)_Cast(a)->field_name.GetJsonAsset()).get()); }; \
		f_data->setter = [](void* a, void* b) -> void { EXPANDER(Classname)_Cast(a)->field_name = Jigsaw::Assets::SerializedRef<asset_type>(*static_cast<Jigsaw::Ref<Jigsaw::Assets::JsonAsset>*>(b)); }; \
		EXPANDER(Classname)_marshalling_map->RegisterFieldData(std::string(#field_name), f_data); \
	}; \
}; \
 \
D_EXPANDER(Classname, field_name)_def D_EXPANDER(classname, field_name)_def_build;

	}
}
#endif
