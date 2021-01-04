#ifdef _RUN_UNIT_TESTS_
#include "System/THREAD_SAFE_SYSTEM_RESOURCES.h"

class TestResource : public Jigsaw::Assets::DataAsset {
public:
	TestResource(const Jigsaw::Assets::AssetDescriptor& descriptor): DataAsset(descriptor) { }

	
protected:
	Jigsaw::Assets::ASSET_LOAD_RESULT Load(const Jigsaw::File_IO::FILE_DATA& file_data, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources) {
		Sleep(10);
		Jigsaw::Assets::ASSET_LOAD_RESULT result;
		result.result = Jigsaw::Assets::COMPLETE;
		return result;
	}
};

#endif