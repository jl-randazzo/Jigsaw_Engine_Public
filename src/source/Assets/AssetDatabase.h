#ifndef _ASSET_DATABASE_H_
#define _ASSET_DATABASE_H_
#include <Windows.h>
#include "sqlite3.h"
#include "fileapi.h"
#include "BaseAsset.h"
#include <sstream>
#include "Ref.h"

namespace Jigsaw {
	namespace Assets {
		enum DB_CONNECTION_TYPE {
			DB_CONNECTION_READONLY, DB_CONNECTION_READWRITE, DB_CONNECTION_CREATE_READWRITE
		};

		/// <summary>
		/// The AssetDatabaseReader is used to examine the asset database to collect meta information related to unique
		/// identifiers (UID's) and their relevant file paths, resource types, and more. 
		/// </summary>
		class AssetDatabaseReader {
		public:
			AssetDatabaseReader(const char* db_connection_string);

			virtual ~AssetDatabaseReader();

			/// <summary>
			/// Writes the target 'descriptor' and its associated file_path and metadata to the AssetDatabase
			/// </summary>
			/// <param name="id"></param>
			/// <returns>The completed AssetDescriptor associated with the id</returns>
			AssetDescriptor FetchDescriptor(const Jigsaw::System::UID& id);

			/// <summary>
			/// Fetches an array of machine id's that are associated with a given scene object
			/// </summary>
			/// <param name="id"></param>
			/// <param name="out_count>a pointer to a location that receives the size of the output array</param>
			/// <returns></returns>
			Jigsaw::Unique<Jigsaw::System::UID[]> FetchSceneToMachineAssociations(const Jigsaw::System::UID& id, size_t* out_count);

		protected:
			/// <summary>
			/// A protected constructor is available so that inheriting classes can designate the connection type
			/// </summary>
			/// <param name="db_connection_string"></param>
			/// <param name="type"></param>
			AssetDatabaseReader(const char* db_connection_string, Jigsaw::Assets::DB_CONNECTION_TYPE type);

			sqlite3* db;
		private:
			/// <summary>
			/// Initializes the connection to the database. Called from constructors.
			/// </summary>
			void Init();

			const char* db_connection_string;
			const DB_CONNECTION_TYPE connection_type;
			sqlite3_stmt* desc_select_stmt;
			sqlite3_stmt* scene_machine_count_stmt;
			sqlite3_stmt* scene_machine_select_stmt;

		};

		/// <summary>
		/// The AssetDatabaseReadWriter inherits from the reader and also supports writing to the database.
		/// </summary>
		class AssetDatabaseReadWriter : public AssetDatabaseReader {
		public:
			/// <summary>
			/// Primary constructor for a Read/Writer just needs the connection string
			/// </summary>
			/// <param name="db_connection_string"></param>
			AssetDatabaseReadWriter(const char* db_connection_string);

			virtual ~AssetDatabaseReadWriter();

			/// <summary>
			/// Writes the target 'descriptor' and its associated file_path and metadata to the AssetDatabase
			/// </summary>
			/// <param name="descriptor"></param>
			void WriteDescriptor(const AssetDescriptor& descriptor);

			/// <summary>
			/// Function for writing scene-machine associations into the data table
			/// </summary>
			/// <param name="scene_id"></param>
			/// <param name="machine_ids"></param>
			void WriteSceneMachineAssociations(const Jigsaw::System::UID& scene_id, const Jigsaw::Unique<Jigsaw::System::UID[]>& machine_ids, const size_t count);

		private:

			sqlite3_stmt* desc_insert_stmt;
			sqlite3_stmt* scene_machine_insert_stmt;

		};
	}
}

#endif
