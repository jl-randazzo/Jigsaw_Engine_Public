#include "SignatureMap.h"
#include "Debug/j_debug.h"

using namespace Jigsaw::System;
using namespace Jigsaw::Util;

// utility method for initializing the size map
std::map<Jigsaw::Util::etype_index, unsigned int> BuildSizeMapAndOffsets(const std::vector<etype_index> etype_indices, unsigned int * index_offset, size_t& size) {
	std::map<Jigsaw::Util::etype_index, unsigned int> etype_info_offset_map;
	unsigned int offset = 0;
	unsigned int index = 0;
	size_t max_align = 1;
	for (etype_index et_i : etype_indices) {
		size_t align = et_i.Get().align;
		max_align = align > max_align ? align : max_align;

		J_D_ASSERT_LOG_ERROR((align <= sizeof(max_align_t)), SignatureMap, "SignatureMap attempted to be built with an object whose maximum alignment exceeds 'max_align_t' value");

		unsigned int misalignment = offset % align;
		if (misalignment != 0) {
			// padding needed to realign for the target type
			offset += (et_i.Get().align - misalignment);
		}
		etype_info_offset_map.insert(std::make_pair(et_i, offset));
		index_offset[index++] = offset;
		offset += et_i.Get().size;
	}
	size_t pad = max_align - (offset % max_align);
	// include some padding to align to the highest alignment requirement for the given types
	size = offset + (pad == max_align ? 0 : pad);
	return etype_info_offset_map;
}

// utility method for initializing the size map without the index offsets
std::map<Jigsaw::Util::etype_index, unsigned int> BuildSizeMap(const std::vector<etype_index> etype_indices) {
	std::map<Jigsaw::Util::etype_index, unsigned int> etype_info_offset_map;
	unsigned int offset = 0;
	for (etype_index et_i : etype_indices) {
		etype_info_offset_map.insert(std::make_pair(et_i, offset));
		offset += et_i.Get().size;
	}
	return etype_info_offset_map;
}

SignatureMap::SignatureMap(const std::vector<etype_index> etype_indices) : index_offset(new unsigned int[etype_indices.size()]), 
	etype_info_offset_map(BuildSizeMapAndOffsets(etype_indices, this->index_offset.get(), size)) { }

// this constructor is used to override the index offsets
Jigsaw::System::SignatureMap::SignatureMap(unsigned int* index_offset, const std::vector<Jigsaw::Util::etype_index> aligned_types) : index_offset(index_offset),
	etype_info_offset_map(BuildSizeMap(aligned_types)) { }

Jigsaw::System::SignatureMap::SignatureMap(SignatureMap&& other) noexcept
	: index_offset(std::move(other.index_offset)), etype_info_offset_map(std::move(other.etype_info_offset_map)), size(other.size) { }

int Jigsaw::System::SignatureMap::GetTypeOffset(const etype_index& type_info) const {
	return etype_info_offset_map.at(type_info);
}

int Jigsaw::System::SignatureMap::operator[](const Jigsaw::Util::etype_index& type_info) const {
	return etype_info_offset_map.at(type_info);
}

int Jigsaw::System::SignatureMap::GetOffsetOfTypeIndex(const unsigned int i) const {
	return index_offset[i];
}
