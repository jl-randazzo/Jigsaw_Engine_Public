#ifndef _SIGNATURE_MAP_H_
#define _SIGNATURE_MAP_H_
#include <map>
#include <vector>
#include "Util/etype_info.h"

namespace Jigsaw {
	namespace System {
		/// <summary>
		/// All completed Signatures have a SignatureMapping that is used to index either by integer
		/// for aligned data types or by type_info to return the byte offset to the corresponding data type.
		/// </summary>
		class SignatureMap {
		public:
			/// <summary>
			/// Primary constructor takes an immutable vector of type_indices
			/// </summary>
			SignatureMap(const std::vector<Jigsaw::Util::etype_index> aligned_types);

			/// <summary>
			/// A constructor that provides an override for directly passing the index offsets. 
			/// </summary>
			/// <param name="index_offset"></param>
			/// <param name="aligned_types"></param>
			SignatureMap(unsigned int* index_offset, const std::vector<Jigsaw::Util::etype_index> aligned_types);

			/// <summary>
			/// Move constructor
			/// </summary>
			/// <param name="other"></param>
			/// <returns></returns>
			SignatureMap(SignatureMap&& other) noexcept;

			/// <summary>
			/// Empty constructor. Without initializing or assigning to, the SignatureMap will be useless
			/// </summary>
			SignatureMap() {}

			/// <summary>
			/// </summary>
			/// <param name="type_info"></param>
			/// <returns>The offset in bytes to the type_info</returns>
			int GetTypeOffset(const Jigsaw::Util::etype_index& type_info) const;

			int operator[](const Jigsaw::Util::etype_index& type_info) const;

			/// <summary>
			/// </summary>
			/// <returns>The offset in bytes to the aligned_type index i</returns>
			int GetOffsetOfTypeIndex(const unsigned int i) const;

			/// <summary>
			/// </summary>
			/// <returns>The offset in bytes to the aligned_type index i</returns>
			inline int operator[](const unsigned int i) const {
				return (index_offset.get())[i];
			}

			/// <summary>
			/// Returns the total, aligned size of the given object
			/// </summary>
			/// <returns></returns>
			size_t GetSize() const {
				return size;
			}

		private:

			size_t size;
			Jigsaw::Unique<unsigned int[]> index_offset;
			std::map<Jigsaw::Util::etype_index, unsigned int> etype_info_offset_map;
		};
	}
}

#endif // !_SIGNATURE_MAP_H_
