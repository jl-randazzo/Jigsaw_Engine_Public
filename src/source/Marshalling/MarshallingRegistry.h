#ifndef _MARSHALLING_REGISTRY_H_
#define _MARSHALLING_REGISTRY_H_

#include <unordered_map>
#include "Util/etype_info.h"
#include "MarshallingMap.h"

namespace Jigsaw {
	namespace Marshalling {
		/// <summary>
		/// The core registry where all MarshallingMaps for all serializable classes are stored. This registry is populated at run time
		/// while the data segment is being initialized. 
		/// </summary>
		class MarshallingRegistry {
		public:
			/// <summary>
			/// Do not call this method unless you're absolutely sure what you're doing. MarshallingMap's should be inserted using the 'REGISTER_SERIALIZABLE_..." methods,
			/// not through direct calls to 'RegisterMap' in consuming code. 
			/// </summary>
			/// <param name="type"></param>
			/// <param name="map"></param>
			static void RegisterMap(const Jigsaw::Util::etype_info& type, const MarshallingMap& map);

			/// <summary>
			/// Returns a MarshallingMap associated with the given 'type'
			/// </summary>
			/// <param name="type"></param>
			/// <returns></returns>
			static const MarshallingMap& GetMarshallingMap(const Jigsaw::Util::etype_info& type);

		private:
			/// <summary>
			/// The MarshallingRegistry needs to be specifically instantiated since the order of static instantiation of
			/// translation units is undefined. Hence, a singleton-retrieving method needs to be represented internally so that 
			/// the other static functions behave consistently. 
			/// </summary>
			/// <returns></returns>
			static MarshallingRegistry& GetInstance();

			std::unordered_map<Jigsaw::Util::etype_index, MarshallingMap> type_marshalling_map;

		};
	}
}

#endif // !_MARSHALLING_REGISTRY_H_
