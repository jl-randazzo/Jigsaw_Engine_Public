#include "JigsawMarshalling.h"

#include "Assets/ASSET_LOAD_RESULT.h"
#include <algorithm>

#define VALUE_NOT_FOUND -1

using namespace Jigsaw::Util;
using namespace Jigsaw::Marshalling;

void MarshallingMap::RegisterFieldData(const std::string&& name, const field_data* f_data) {
	field_map.insert(std::make_pair(name, f_data));
}

void MarshallingMap::RegisterParentClass(const etype_info& parent) {
	etype_index index(parent);
	auto found = std::find(parent_classes.begin(), parent_classes.end(), index);
	if (found == parent_classes.end()) {
		parent_classes.push_back(index);
	}
}

const field_data* MarshallingMap::GetFieldDataByName(const std::string&& name) const {
	const field_data* f_data = nullptr;
	GetFieldDataByName(name, &f_data);
	return f_data;
}

const std::vector<const field_data*> Jigsaw::Marshalling::MarshallingMap::GetFields() const {
	std::vector<const field_data*> ret_val;
	GetFields(ret_val);
	std::unique(ret_val.begin(), ret_val.end(), [](const field_data* a, const field_data* b) { return a->name == b->name; });
	return ret_val;
}

void* MarshallingMap::Manufacture(void* arg) const {
	if (context_root_manufacturer && arg) {
		return context_root_manufacturer(arg);
	}
	else {
		return manufacturer();
	}
}

const Jigsaw::Util::etype_info& MarshallingMap::GetType() const {
	return type;
}

const bool MarshallingMap::InheritsFrom(const Jigsaw::Util::etype_info& parent) const {
	bool found = parent == GetType();
	auto iterator = parent_classes.begin();

	while (iterator != parent_classes.end() && !found) {
		found |= *iterator == parent || MarshallingRegistry::GetMarshallingMap(*iterator).InheritsFrom(parent);
		iterator++;
	}

	return found;
}

const bool Jigsaw::Marshalling::MarshallingMap::IsContextInitialized() const {
	return (bool)context_root_manufacturer;
}

void Jigsaw::Marshalling::MarshallingMap::GetFields(std::vector<const field_data*>& return_vector) const {
	for (auto field_data_pair : field_map) {
		const field_data* f_data = field_data_pair.second;
		return_vector.push_back(f_data);
	}

	for (auto parent_type : parent_classes) {
		MarshallingRegistry::GetMarshallingMap(parent_type).GetFields(return_vector);
	}
}

const void MarshallingMap::GetFieldDataByName(const std::string& name, const field_data** f_data) const {
	auto iter = field_map.find(name);
	if (iter == field_map.end()) {
		for (const etype_info& parent : parent_classes) {
			const MarshallingMap& map = MarshallingRegistry::GetMarshallingMap(parent);
			map.GetFieldDataByName(name, f_data);
			if (*f_data != nullptr) {
				return;
			}
		}
	}
	else {
		*f_data = (*iter).second;
	}
}

void MarshallingRegistry::RegisterMap(const etype_info& type, const MarshallingMap& map) {
	if (GetInstance().type_marshalling_map.find(type) == GetInstance().type_marshalling_map.end()) {
		etype_index ind(type);
		GetInstance().type_marshalling_map.insert(std::make_pair(ind, map));
	} // else SOL
}

const MarshallingMap& MarshallingRegistry::GetMarshallingMap(const etype_info& type) {
	return GetInstance().type_marshalling_map.at(type);
}

MarshallingRegistry& MarshallingRegistry::GetInstance() {
	static MarshallingRegistry registry;
	return registry;
}

const etype_info& field_data::GetType() const {
	return field_type;
}

