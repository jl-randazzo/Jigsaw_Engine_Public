#ifndef _ENTITY_MAPPING_H_ 
#define _ENTITY_MAPPING_H_

namespace Jigsaw {
	namespace Entities {

		/// <summary>
		/// Indexing into an entity map will return the address of the corresponding aligned arguments for a given Signature
		/// </summary>
		class EntityMap {
		public:
			EntityMap(void* data, unsigned int* byteOffsets, unsigned int count);
			EntityMap(const EntityMap& other);

			~EntityMap();

			void* operator[](int i); 
			void* operator[](const type_info& i);

		private:
			unsigned int** data_map;
		};

		/// <summary>
		/// EntitySignatureMapping encapsulates the data needed to map any entity containing a given EntitySignature.
		/// </summary>
		class EntitySignatureMapping {
		public:
			EntitySignatureMapping();

			EntityMap Map(void* raw_data) const;
		private:

		};
	}
}
#endif
