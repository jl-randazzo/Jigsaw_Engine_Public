#include "UID.h"

#include <combaseapi.h>
#include <algorithm>
#include "Util/BYTE.h"
#include "Marshalling/JigsawMarshalling.h"

using namespace Jigsaw::System;

START_REGISTER_SERIALIZABLE_CLASS(UID)
REGISTER_CUSTOM_FIELD_MAPPER(UID, std::string, value, UIDToString, UIDFromString)
END_REGISTER_SERIALIZABLE_CLASS(UID)

void Jigsaw::System::UIDFromString(Jigsaw::System::UID* node, std::string* string) {
	int index = 0;
	while ((index = string->find_first_of('-')) >= 0) {
		string = &string->erase(index, 1);
	}
	Jigsaw::Unique<BYTE[]> bytes = Jigsaw::Util::HexStrToBytes(string->c_str(), string->size());
	long long* long_bytes = (long long*)(bytes.get());

	node->data_a = long_bytes[0];
	node->data_b = long_bytes[1];
}

UID Jigsaw::System::UIDFromString(std::string&& str) {
	UID ret_val;
	UIDFromString(&ret_val, &str);
	return ret_val;
}

std::string* Jigsaw::System::UIDToString(const Jigsaw::System::UID* uid) {
	// converting the uid to hex
	long long data_[2];
	data_[0] = uid->GetData_a();
	data_[1] = uid->GetData_b();
	Jigsaw::Unique<char[]> hex_str = Jigsaw::Util::BytesToHexStr((BYTE*)& data_, 16);
	std::string i_str = std::string(hex_str.get());
	return new std::string(i_str.substr(0, 8) + '-' + i_str.substr(8, 4) + '-' + i_str.substr(12, 4) + '-' + i_str.substr(16, 4) + '-' + i_str.substr(20));
}

std::string Jigsaw::System::UIDToString(const Jigsaw::System::UID& uid) {
	std::string* str = UIDToString(&uid);
	std::string ret_val = *str;
	delete str;
	return ret_val;
}

UID& Jigsaw::System::UID::operator=(const UID& other) {
	data_a = other.GetData_a();
	data_b = other.GetData_b();
	return *this;
}

UID UID::Create()
{
	GUID id;
	CoCreateGuid(&id);
	long long data_a = static_cast<long long>(id.Data1) << 32;
	data_a |= static_cast<long long>(id.Data2) << 16;
	data_a |= static_cast<long long>(id.Data3);
	long long data_b = *(long long*)id.Data4;
	return UID(data_a, data_b);
}

UID Jigsaw::System::UID::Empty() {
	return UID(0, 0);
}

long long Jigsaw::System::UID::GetData_a() const {
	return data_a;
}

long long Jigsaw::System::UID::GetData_b() const {
	return data_b;
}

bool Jigsaw::System::UID::operator==(const UID& other) const {
	return data_a == other.data_a && data_b == other.data_b;
}
bool Jigsaw::System::UID::operator<(const UID& other) const {
	return data_a < other.data_a || (data_a == other.data_a && data_b < other.data_b);
}

