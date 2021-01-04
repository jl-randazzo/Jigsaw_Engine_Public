#ifndef _BASE_ASSET_H_
#define _BASE_ASSET_H_
#include "ASSET_LOAD_RESULT.h"
#include <Windows.h>
#include <guiddef.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include "System/UID.h"

namespace Jigsaw {
	namespace Assets {
		enum ASSET_TYPE {
			ASSET_TEST = -1,
			ASSET_MODEL = 0,
			ASSET_TEXTURE = 1,
			ASSET_VERTEX_SHADER = 2,
			ASSET_PIXEL_SHADER = 3,
			ASSET_GEOMETRY_SHADER = 4,
			ASSET_JSON = 28,
			ASSET_TEXT = 29,
			ASSET_GPU_BUFFER = 30,
			ASSET_SO_BUFFER = 34,
			ASSET_COPY_BUFFER = 37,
			ASSET_INPUT_LAYOUT = 44,
			ASSET_RASTERIZER_STATE = 50,
			ASSET_ROOT_SIGNATURE = 51,
			ASSET_PIPELINE_STATE = 52,
			ASSET_CUBE = 500000
		};

		// AssetDescriptors are the primary key used to load, examine, and store references to 
		class AssetDescriptor {

		public:
			Jigsaw::System::UID id;
			std::string fully_qualified_type_name; // these data are applicable in certain situations
			ASSET_TYPE type;
			const char* file_path;

		public:
			AssetDescriptor() : id(), type(), file_path(nullptr) {}
			AssetDescriptor(Jigsaw::System::UID id, ASSET_TYPE type, const char* file_path) : id(id), type(type), file_path(file_path) {}
			bool operator< (const AssetDescriptor& other) {
				return id < other.id;
			}
		};

		class BaseAsset {
		public:
			const AssetDescriptor& GetDescriptor() const {
				return descriptor;
			}

		protected:
			BaseAsset(const AssetDescriptor& descriptor) : descriptor(descriptor) { }
			virtual ~BaseAsset() = 0;

			const AssetDescriptor descriptor;
		};

		// equality operator override for AssetDescriptor
		inline bool operator==(const AssetDescriptor& lhs, const AssetDescriptor& rhs) {
			return lhs.id == rhs.id;
		}
	}
}

namespace std {
	template<>
	struct hash<Jigsaw::Assets::AssetDescriptor> {
		size_t operator()(const Jigsaw::Assets::AssetDescriptor& descriptor) const {
			return hash<Jigsaw::System::UID>()(descriptor.id);
		}
	};
}
#endif
