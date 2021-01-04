#ifndef _APPLICATION_ROOT_PROPERTIES_H_
#define _APPLICATION_ROOT_PROPERTIES_H_

#include "System/UID.h"
#include "ApplicationRoot.h"

struct ApplicationRootProperties {
public:
	friend HRESULT Run();

	ApplicationRootProperties() {}

	Jigsaw::System::UID root_scene_id;

	std::string db_connection;

	std::string lib_path = "./Lib/metadata/";

	static const ApplicationRootProperties& Get();

private:

};
#endif