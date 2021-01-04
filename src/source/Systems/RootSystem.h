#ifndef _ROOT_SYS_H_
#define _ROOT_SYS_H_
#include <vector>
#include <string>
#include <typeinfo>
#include "Entities/JigsawEntity.h"
#include "System/SignatureMap.h"
#include "System/Signature.h"
#include "Systems/SystemRegistry.h"
#include "Math/LinAlg.h"

namespace Jigsaw {
	namespace Systems {

#define _THIS GetEntity()

#define h_next i
#define g_next h
#define f_next g
#define e_next f
#define d_next e
#define c_next d
#define b_next c
#define a_next b

#define six_next seven
#define five_next six
#define four_next five
#define three_next four
#define two_next three
#define one_next two
#define zero_next one

#define seven_ 7
#define six_ 6
#define five_ 5
#define four_ 4
#define three_ 3
#define two_ 2
#define one_ 1
#define zero_ 0

#define _EXPAND(...)__VA_ARGS__
#define _QUOTE r

#define _type_entity_mapping_cast(i, type_name) *static_cast<_EXPAND(type_name)*>((void*)(entity_loc + conversion_map[_EXPAND(i)_]))
#define _ref_type_arg(arg, type_name) type_name& arg
#define _sys_sig_member_type(arg, type_name) const Jigsaw::Util::etype_info* _EXPAND(arg)_ = &Jigsaw::Util::etype_info::Id<type_name>(); builder.AddType(*_EXPAND(arg)_);

#define _mac_arg_fe_1(mac, i, x, ...) _EXPAND(mac)(i, x)
#define _mac_arg_fe_2(mac, i, x, ...) _EXPAND(mac)(i, x), _EXPAND(_mac_arg_fe_1(mac, _EXPAND(i)_next, __VA_ARGS__))
#define _mac_arg_fe_3(mac, i, x, ...) _EXPAND(mac)(i, x), _EXPAND(_mac_arg_fe_2(mac, _EXPAND(i)_next, __VA_ARGS__))
#define _mac_arg_fe_4(mac, i, x, ...) _EXPAND(mac)(i, x), _EXPAND(_mac_arg_fe_3(mac, _EXPAND(i)_next, __VA_ARGS__))
#define _mac_arg_fe_5(mac, i, x, ...) _EXPAND(mac)(i, x), _EXPAND(_mac_arg_fe_4(mac, _EXPAND(i)_next, __VA_ARGS__))
#define _mac_arg_fe_6(mac, i, x, ...) _EXPAND(mac)(i, x), _EXPAND(_mac_arg_fe_5(mac, _EXPAND(i)_next, __VA_ARGS__))
#define _mac_arg_fe_7(mac, i, x, ...) _EXPAND(mac)(i, x), _EXPAND(_mac_arg_fe_6(mac, _EXPAND(i)_next, __VA_ARGS__))
#define _mac_arg_fe_8(mac, i, x, ...) _EXPAND(mac)(i, x), _EXPAND(_mac_arg_fe_7(mac, _EXPAND(i)_next, __VA_ARGS__))

#define _mac_fe_1(mac, i, x, ...) _EXPAND(mac)(i, x)
#define _mac_fe_2(mac, i, x, ...) _EXPAND(mac)(i, x) _EXPAND(_mac_fe_1(mac, _EXPAND(i)_next, __VA_ARGS__))
#define _mac_fe_3(mac, i, x, ...) _EXPAND(mac)(i, x) _EXPAND(_mac_fe_2(mac, _EXPAND(i)_next, __VA_ARGS__))
#define _mac_fe_4(mac, i, x, ...) _EXPAND(mac)(i, x) _EXPAND(_mac_fe_3(mac, _EXPAND(i)_next, __VA_ARGS__))
#define _mac_fe_5(mac, i, x, ...) _EXPAND(mac)(i, x) _EXPAND(_mac_fe_4(mac, _EXPAND(i)_next, __VA_ARGS__))
#define _mac_fe_6(mac, i, x, ...) _EXPAND(mac)(i, x) _EXPAND(_mac_fe_5(mac, _EXPAND(i)_next, __VA_ARGS__))
#define _mac_fe_7(mac, i, x, ...) _EXPAND(mac)(i, x) _EXPAND(_mac_fe_6(mac, _EXPAND(i)_next, __VA_ARGS__))
#define _mac_fe_8(mac, i, x, ...) _EXPAND(mac)(i, x) _EXPAND(_mac_fe_7(mac, _EXPAND(i)_next, __VA_ARGS__))

#define _COUNT_TYPE_ARGS(_1, _2, _3, _4, _5, _6, _7, _8, N, ...)N

#define JGSW_SYSTEM_METHOD(sys_name, ...) \
public:\
	void _EXPAND(sys_name)(_EXPAND(_COUNT_TYPE_ARGS(__VA_ARGS__, _mac_arg_fe_8, _mac_arg_fe_7, _mac_arg_fe_6, _mac_arg_fe_5, _mac_arg_fe_4, _mac_arg_fe_3, _mac_arg_fe_2, _mac_arg_fe_1)(_ref_type_arg, a, __VA_ARGS__))); \
	void Update(Jigsaw::Entities::JigsawEntity& entity, const Jigsaw::System::SignatureMap& conversion_map) override {\
		BYTE* entity_loc = entity.GetRawData(); \
		_EXPAND(sys_name)(_EXPAND(_COUNT_TYPE_ARGS(from __VA_ARGS__, _mac_arg_fe_8, _mac_arg_fe_7, _mac_arg_fe_6, _mac_arg_fe_5, _mac_arg_fe_4, _mac_arg_fe_3, _mac_arg_fe_2, _mac_arg_fe_1)(_type_entity_mapping_cast, zero, __VA_ARGS__)));\
	};\
 static Jigsaw::Systems::SYSTEM_REGISTRY_BEAN _Get_Registry_Bean() {\
	Jigsaw::Systems::SYSTEM_REGISTRY_BEAN bean; \
	Jigsaw::System::SignatureBuilder builder; \
	_EXPAND(_COUNT_TYPE_ARGS(__VA_ARGS__, _mac_fe_8, _mac_fe_7, _mac_fe_6, _mac_fe_5, _mac_fe_4, _mac_fe_3, _mac_fe_2, _mac_fe_1)(_sys_sig_member_type, a, __VA_ARGS__))\
	bean.signature = Jigsaw::MakeRef<Jigsaw::System::Signature>(builder.SortMode(Jigsaw::System::SignatureBuilder::SORT_MODE::SORT_MODE_OFF).Build());\
	bean.generate_system_function = [] () { return Jigsaw::Unique<RootSystem>(new ThisType); };\
	bean.type_info = &Jigsaw::Util::etype_info::Id<ThisType>(); \
	return bean; \
};

#define JGSW_SYSTEM(sys_name) class sys_name : public Jigsaw::Systems::RootSystem, private Jigsaw::Systems::DTO<sys_name>
#define REGISTER_SYSTEM_SIGNATURE(class_name, sys_name) static class_name::sys_name_Register sys_name_Register_call;

		template <typename T>
		class DTO {
		protected:
			typedef T ThisType;
		};

		/// <summary>
		/// Every system, multi-threaded or not, must inherit from the RootSystem class
		/// 
		/// New systems are registered using the macros defined above.
		/// </summary>
		class RootSystem {
		public:
			virtual void Update(Jigsaw::Entities::JigsawEntity& entity, const Jigsaw::System::SignatureMap& conversion_map) = 0;
			/// <summary>
			/// Returns an array of SystemSignature objects that are used by the EntitySystemService to bind a set of EntityPatterns to 
			/// their relevant Systems. These systems will subsequently be executed on all entities that have the relevant member types. 
			/// </summary>
			/// <returns>An immutable set of SystemSignatures that defines the calling signature of each of the Class Systems</returns>
			const std::vector<Jigsaw::System::Signature> GetSystemSignatures() {
			};

		protected:
			const Jigsaw::Entities::JigsawEntity& GetEntity();

			static void PushSystemSignature(Jigsaw::System::Signature& sys_sig);

		private:
			static std::vector<Jigsaw::System::Signature> system_signatures;

		};

		JGSW_SYSTEM(ImplSystem) {
		public:
			JGSW_SYSTEM_METHOD(RenderSystemMethod, const Quaternion, Vector2);

		private:
		};
	}
}

#define REGISTER_SYSTEM(sys_name) \
struct _EXPAND(sys_name)_Bean_Register { \
	_EXPAND(sys_name)_Bean_Register() {\
		Jigsaw::Systems::SYSTEM_REGISTRY_BEAN bean = _EXPAND(sys_name)::_Get_Registry_Bean(); \
		Jigsaw::Systems::SystemRegistry::GetBeans().push_back(bean); \
	}; \
}; \
_EXPAND(sys_name)_Bean_Register _EXPAND(sys_name)_Bean_Register_def;
#endif 
