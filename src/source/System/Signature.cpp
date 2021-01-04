#include "Signature.h"

using namespace Jigsaw::System;
using namespace Jigsaw;
using namespace Jigsaw::Util;

size_t SumSizes(const std::vector<Jigsaw::Util::etype_index> aligned_types) {
	size_t size = 0;
	for (Jigsaw::Util::etype_index et_i : aligned_types) {
		size += et_i.Get().size;
	}

	return size;
}

// Signature Implementations

Signature::Signature(const std::vector<Jigsaw::Util::etype_index>& aligned_types) : aligned_types(aligned_types), sig_map(aligned_types) {}

Signature::Signature(const Signature& other) noexcept : aligned_types(other.aligned_types), sig_map(other.aligned_types) {}

const std::vector<Jigsaw::Util::etype_index>& Signature::GetAlignedTypes() const {
	return aligned_types;
}

const bool Signature::ContainsType(Jigsaw::Util::etype_index type) const {
	auto iterator = std::find(std::begin(aligned_types), std::end(aligned_types), type);
	return std::end(aligned_types) != iterator;
}

const SignatureMap& Signature::GetMap() const {
	return sig_map;
}


bool Signature::HasConversionMap(const Signature& convert_to, Unique<SignatureMap>* out_conversion_map) const {
	std::vector<etype_index> conv_aligned_types = convert_to.GetAlignedTypes();
	unsigned int* aligned_offsets = new unsigned int[conv_aligned_types.size()];
	unsigned int i = 0;
	bool is_valid = true;

	for (etype_index type : conv_aligned_types) {
		is_valid &= this->ContainsType(type);
		if (!is_valid) {
			break;
		}

		aligned_offsets[i++] = sig_map[type];
	}

	// only create the conversion_map if there is a valid conversion map that can be created
	if (is_valid) {
		*out_conversion_map = MakeUnique<SignatureMap>(aligned_offsets, aligned_types);
	}

	return is_valid;
}

bool Jigsaw::System::Signature::operator==(const Signature& other) const {
	const std::vector<etype_index>& o_aligned_types = other.GetAlignedTypes();

	bool match = aligned_types.size() == o_aligned_types.size();

	auto local_iter = aligned_types.begin();
	auto other_iter = o_aligned_types.begin();
	while (match && local_iter != aligned_types.end()) {
		match &= *local_iter++ == *other_iter++;
	}
	return match;
}

bool Jigsaw::System::Signature::operator<(const Signature& other) const {
	const std::vector<etype_index>& o_aligned_types = other.GetAlignedTypes();

	bool size_match = aligned_types.size() == o_aligned_types.size();

	if (size_match) {
		bool less_than = false;

		auto local_iter = aligned_types.begin();
		auto other_iter = o_aligned_types.begin();
		while (!less_than && local_iter != aligned_types.end()) {
			less_than |= (*local_iter) < (*other_iter);
			local_iter++;
			other_iter++;
		}
		return less_than;
	}
	else {
		return aligned_types.size() < o_aligned_types.size();
	}
}

SignatureBuilder& Jigsaw::System::SignatureBuilder::operator=(SignatureBuilder& other) {
	types = other.types;
	return *this;
}

// SignatureBuilder implementations
SignatureBuilder& SignatureBuilder::AddType(const Jigsaw::Util::etype_info& type) {
	if (!HasType(type)) {
		types.push_back(type);
	}
	return *this;
}

SignatureBuilder& SignatureBuilder::AddTypes(const Signature& signature) {
	const std::vector<etype_index> aligned_types = signature.GetAlignedTypes();
	std::for_each(std::begin(aligned_types), std::end(aligned_types), [&](etype_index index) -> void {
		AddType(index.Get());
		});
	return *this;
}

SignatureBuilder& Jigsaw::System::SignatureBuilder::AddTypes(const std::vector<Jigsaw::Util::etype_index>& indices) {
	for (const Jigsaw::Util::etype_index& index : indices) {
		AddType(index.Get());
	}
	return *this;
}

SignatureBuilder& Jigsaw::System::SignatureBuilder::SortMode(SORT_MODE sort_mode) {
	this->sort_mode = sort_mode;
	return *this;
}

bool SignatureBuilder::HasType(Jigsaw::Util::etype_index type) const {
	return std::find(std::begin(types), std::end(types), type) != std::end(types);
}

Unique<Signature> SignatureBuilder::Build() {
	if (sort_mode == SORT_MODE::SORT_MODE_ON) {
		std::sort(types.begin(), types.end());
	}
	Signature* sig = new Signature(types);
	return Unique<Signature>(sig);
}
