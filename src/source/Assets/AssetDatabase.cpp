#include "AssetDatabase.h"
#include "Debug/j_debug.h"
#include <windows.h>
#include "Util/BYTE.h"

using namespace Jigsaw::Assets;

Jigsaw::Assets::AssetDatabaseReader::AssetDatabaseReader(const char* db_connection_string): db_connection_string(db_connection_string), connection_type(DB_CONNECTION_READONLY) {
	Init();
}

AssetDatabaseReader::AssetDatabaseReader(const char* db_connection_string, DB_CONNECTION_TYPE type)
	: db_connection_string(db_connection_string), connection_type(type) {
	Init();
}

AssetDatabaseReader::~AssetDatabaseReader() {
	sqlite3_finalize(desc_select_stmt);
	sqlite3_finalize(scene_machine_select_stmt);
	sqlite3_finalize(scene_machine_count_stmt);
	sqlite3_close(db);
}

AssetDescriptor Jigsaw::Assets::AssetDatabaseReader::FetchDescriptor(const Jigsaw::System::UID& id) {

	J_LOG_TRACE(AssetDatabaseReader, "Fetching descriptor for UID {0}", Jigsaw::System::UIDToString(id));
	// converting the uid to hex
	std::string hex_str = Jigsaw::System::UIDToString(id);

	// binding the id blob to the prepared statement
	int bind_res = sqlite3_bind_text(desc_select_stmt, 1, hex_str.c_str(), hex_str.size(), SQLITE_STATIC);

	//issuing the initial request 
	int res = sqlite3_step(desc_select_stmt);

	J_D_ASSERT_LOG_ERROR((res == SQLITE_ROW), AssetDatabaseReader, "No database entry available for descriptor id {0}", Jigsaw::System::UIDToString(id));

	// fetching the returned data from the columns
	const unsigned char* id_blob = sqlite3_column_text(desc_select_stmt, 0);
	unsigned int type = sqlite3_column_int(desc_select_stmt, 1);
	int file_name_size = sqlite3_column_bytes(desc_select_stmt, 2);
	const unsigned char* file_name_read = sqlite3_column_text(desc_select_stmt, 2);
	int qual_type_arg_size = sqlite3_column_bytes(desc_select_stmt, 3);
	const unsigned char* qual_type_arg_read = sqlite3_column_text(desc_select_stmt, 3);

	// converting the id_blob into the long long data elements of the UID type
	Jigsaw::System::UID id_byte_blob = Jigsaw::System::UIDFromString(std::string((char*)id_blob));

	// reading the file name for the asset 
	char* file_name_str = new char[file_name_size + 1];
	file_name_str[file_name_size] = '\0';
	memcpy(file_name_str, file_name_read, file_name_size);

	// reading the file name for the asset 
	char* qual_type_arg_str = new char[qual_type_arg_size + 1];
	qual_type_arg_str[qual_type_arg_size] = '\0';
	memcpy(qual_type_arg_str, qual_type_arg_read, qual_type_arg_size);

	// resetting the statement
	res = sqlite3_step(desc_select_stmt);
	DEBUG_ASSERT(res == SQLITE_DONE, "More than one row available for unique data type");
	sqlite3_reset(desc_select_stmt);

	AssetDescriptor descriptor(id_byte_blob, (ASSET_TYPE)type, file_name_str);
	descriptor.fully_qualified_type_name = qual_type_arg_str;
	delete[] qual_type_arg_str;
	return std::move(descriptor);
}

Jigsaw::Unique<Jigsaw::System::UID[]> Jigsaw::Assets::AssetDatabaseReader::FetchSceneToMachineAssociations(const Jigsaw::System::UID& id, size_t* out_size) {
	// converting the uid to hex
	std::string hex_str = Jigsaw::System::UIDToString(id);

	// issuing an instruction to count the number of rows
	int count_bind_res = sqlite3_bind_text(scene_machine_count_stmt, 1, hex_str.c_str(), hex_str.size(), SQLITE_STATIC);
	int count_res = sqlite3_step(scene_machine_count_stmt);
	*out_size = sqlite3_column_int(scene_machine_count_stmt, 0);
	sqlite3_reset(scene_machine_count_stmt);

	int bind_res = sqlite3_bind_text(scene_machine_select_stmt, 1, hex_str.c_str(), hex_str.size(), SQLITE_STATIC);

	// stepping through the actual results
	Jigsaw::System::UID* uids = new Jigsaw::System::UID[count_res];
	int res = sqlite3_step(scene_machine_select_stmt);
	for (int i = 0; i < *out_size; i++) {
		const unsigned char* id_blob = sqlite3_column_text(scene_machine_select_stmt, 0);
		uids[i] = Jigsaw::System::UIDFromString(std::string((char*)id_blob));
		res = sqlite3_step(scene_machine_select_stmt);
	}
	DEBUG_ASSERT(res == SQLITE_DONE, "Failure in FetchSceneToMachineAssociations")
	sqlite3_reset(scene_machine_select_stmt);

	return Jigsaw::Unique<Jigsaw::System::UID[]>(uids);
}

void Jigsaw::Assets::AssetDatabaseReader::Init() {
	int flags = 0;
	switch (connection_type) {
	case DB_CONNECTION_READONLY:
		flags |= SQLITE_OPEN_READONLY;
		break;
	case DB_CONNECTION_READWRITE:
		flags |= SQLITE_OPEN_READWRITE;
		break;
	case DB_CONNECTION_CREATE_READWRITE:
		flags |= SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
		break;
	}

	int res = sqlite3_open_v2(db_connection_string, &db, flags, nullptr);
	// if we are creating a new database, we must prepare the table and the indexing system
	if (connection_type == DB_CONNECTION_CREATE_READWRITE) {
		sqlite3_stmt* stmt;
		const char* create_cmd = "CREATE TABLE IF NOT EXISTS DescriptorTable (UID character(36) NOT NULL UNIQUE, type int, file_path varchar(1028), qualified_type_argument varchar(1028));";
		sqlite3_prepare_v2(db, create_cmd, strlen(create_cmd), &stmt, nullptr);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);

		const char* create_scene_machine_cmd = "CREATE TABLE IF NOT EXISTS SceneToMachineTable (SCENE_UID character(36) NOT NULL, MACHINE_UID character(36) NOT NULL);";
		sqlite3_prepare_v2(db, create_scene_machine_cmd, strlen(create_scene_machine_cmd), &stmt, nullptr);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);

		const char* index_cmd = "CREATE UNIQUE INDEX IF NOT EXISTS uid ON DescriptorTable(UID)";
		sqlite3_prepare_v2(db, index_cmd, strlen(index_cmd), &stmt, nullptr);
		int res = sqlite3_step(stmt);
		DEBUG_ASSERT(res == SQLITE_DONE, "Failed to create unique index on field");
		sqlite3_finalize(stmt);
	}

	const char* select_string = "SELECT * FROM DescriptorTable WHERE UID=?";
	int prep_res = sqlite3_prepare_v2(db, select_string, strlen(select_string), &desc_select_stmt, nullptr);

	const char* scene_machine_count_string = "SELECT COUNT(*) FROM SceneToMachineTable WHERE SCENE_UID=?";
	prep_res = sqlite3_prepare_v2(db, scene_machine_count_string, strlen(scene_machine_count_string), &scene_machine_count_stmt, nullptr);

	const char* scene_machine_select_string = "SELECT MACHINE_UID FROM SceneToMachineTable WHERE SCENE_UID=?";
	prep_res = sqlite3_prepare_v2(db, scene_machine_select_string, strlen(scene_machine_select_string), &scene_machine_select_stmt, nullptr);
}

Jigsaw::Assets::AssetDatabaseReadWriter::AssetDatabaseReadWriter(const char* db_connection_string): AssetDatabaseReader(db_connection_string, DB_CONNECTION_CREATE_READWRITE), desc_insert_stmt(nullptr) {
	const char* insert_str("INSERT INTO DescriptorTable (UID, type, file_path, qualified_type_argument) VALUES (?, ?, ?, ?)");
	int prep_res = sqlite3_prepare_v2(db, insert_str, strlen(insert_str), &desc_insert_stmt, nullptr);

	const char* insert_scene_machine_str("INSERT INTO SceneToMachineTable (SCENE_UID, MACHINE_UID) VALUES (?, ?)");
	prep_res = sqlite3_prepare_v2(db, insert_scene_machine_str, strlen(insert_scene_machine_str), &scene_machine_insert_stmt, nullptr);
}

Jigsaw::Assets::AssetDatabaseReadWriter::~AssetDatabaseReadWriter() {
	sqlite3_finalize(desc_insert_stmt);
	sqlite3_finalize(scene_machine_insert_stmt);
}

void Jigsaw::Assets::AssetDatabaseReadWriter::WriteDescriptor(const AssetDescriptor& descriptor) {
	// getting the UID in hex string form
	std::string id_str = Jigsaw::System::UIDToString(descriptor.id);

	int blob_res = sqlite3_bind_text(desc_insert_stmt, 1, id_str.c_str(), id_str.size(), SQLITE_STATIC);
	int int_res = sqlite3_bind_int(desc_insert_stmt, 2, descriptor.type);
	int text_res = sqlite3_bind_text(desc_insert_stmt, 3, descriptor.file_path, strlen(descriptor.file_path), SQLITE_STATIC);
	int type_res = sqlite3_bind_text(desc_insert_stmt, 4, descriptor.fully_qualified_type_name.c_str(), descriptor.fully_qualified_type_name.size(), SQLITE_STATIC);

	int res = sqlite3_step(desc_insert_stmt);
	if (res == SQLITE_CONSTRAINT) {
		J_LOG_ERROR(AssetDatabaseReadWriter, "SQL constraint encountered on element input")
	}
	else {
		J_D_ASSERT_LOG_ERROR(res == SQLITE_DONE, AssetDatabaseReadWriter, "Error while executing write step {0:d}", res);
	}
	sqlite3_reset(desc_insert_stmt);
}

void Jigsaw::Assets::AssetDatabaseReadWriter::WriteSceneMachineAssociations(const Jigsaw::System::UID& scene_id, const Jigsaw::Unique<Jigsaw::System::UID[]>& machine_ids, const size_t count) {
	// getting the UID in hex string form
	std::string scene_id_str = Jigsaw::System::UIDToString(scene_id);

	for (int i = 0; i < count; i++) {
		std::string machine_id_str = Jigsaw::System::UIDToString(machine_ids.get()[i]);
		int blob_res = sqlite3_bind_text(scene_machine_insert_stmt, 1, scene_id_str.c_str(), scene_id_str.size(), SQLITE_STATIC);
		blob_res = sqlite3_bind_text(scene_machine_insert_stmt, 2, machine_id_str.c_str(), machine_id_str.size(), SQLITE_STATIC);
		int res = sqlite3_step(scene_machine_insert_stmt);

		DEBUG_ASSERT(res == SQLITE_DONE, "Error while executing write step");
		sqlite3_reset(scene_machine_insert_stmt);
	}
}

