#include "EntityMapping.h"
#include <typeinfo>

using namespace Jigsaw::Entities;

EntityMap::~EntityMap() {
	delete[] data_map;
}

EntityMap::EntityMap(void* data, unsigned int* byteOffsets, unsigned int count) {
	unsigned int* raw_d = static_cast<unsigned int*>(data);
	data_map = new unsigned int* [count];
	for (int i = 0; i < count; i++) {
		data_map[i] = raw_d + byteOffsets[i];
	}
}

void* EntityMap::operator[](int i) {
	return data_map[i];
}

void* EntityMap::operator[](const type_info& i) {

	return data_map[0];
}
