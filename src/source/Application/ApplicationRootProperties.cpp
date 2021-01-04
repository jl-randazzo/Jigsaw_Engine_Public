#include "Application/ApplicationRootProperties.h"
#include "Marshalling/JigsawMarshalling.h"

START_REGISTER_SERIALIZABLE_CLASS(ApplicationRootProperties)
REGISTER_SERIALIZABLE_FIELD(ApplicationRootProperties, Jigsaw::System::UID, root_scene_id)
REGISTER_SERIALIZABLE_FIELD(ApplicationRootProperties, std::string, db_connection)
END_REGISTER_SERIALIZABLE_CLASS(ApplicationRootProperties)

const ApplicationRootProperties& ApplicationRootProperties::Get() {
	static ApplicationRootProperties properties;
	return properties;
}
