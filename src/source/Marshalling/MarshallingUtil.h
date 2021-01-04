#ifndef _MARSHALLING_UTIL_H_
#define _MARSHALLING_UTIL_H_
#include "JSONNode.h"
#include "File_IO/FileUtil.h"
#include "MarshallingRegistry.h"

namespace Jigsaw {
	namespace Marshalling {
		class MarshallingUtil {
		public:
			template<typename T>
			static Jigsaw::Ref<T> Unmarshal(Jigsaw::File_IO::FILE_DATA& file_data) {
				const Jigsaw::Util::etype_info& type_id = Jigsaw::Util::etype_info::Id<T>();
				std::istringstream json_stream(GetStringStreamFromFile(file_data));

				Jigsaw::Marshalling::JSONNodeReader root(json_stream, MarshallingRegistry::GetMarshallingMap(type_id));

				return Jigsaw::Ref<T>(static_cast<T*>(root.BuildNode().raw_data));
			};

			template<typename T>
			static Jigsaw::Ref<T> Unmarshal(const std::string& str) {
				const Jigsaw::Util::etype_info& type_id = Jigsaw::Util::etype_info::Id<T>();
				std::istringstream json_stream(str);

				Jigsaw::Marshalling::JSONNodeReader root(json_stream, MarshallingRegistry::GetMarshallingMap(type_id));

				return Jigsaw::Ref<T>(static_cast<T*>(root.BuildNode().raw_data));
			};

			template<typename T>
			static std::string Marshal(T* raw_object) {
				const Jigsaw::Util::etype_info& type_id = Jigsaw::Util::etype_info::Id<T>();

				Jigsaw::Marshalling::JSONNodeWriter root(static_cast<void*>(raw_object), MarshallingRegistry::GetMarshallingMap(type_id));
				MARSHALLING_RESULT res = root.ReadNode();

				return res.marshalled_object;
			}
		};
	}
}
#endif
