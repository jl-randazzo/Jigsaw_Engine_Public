#include "JSONNode.h"
#include "Util/etype_info.h"

#include "JigsawMarshalling.h"
#include "Assets/DataAssets.h"

using namespace Jigsaw::Assets;
using namespace Jigsaw::Marshalling;
using namespace Jigsaw::Util;

JSONNodeReader::JSONNodeReader(std::istringstream& json_stream, const MarshallingMap& marshalling_map)
	: json_stream(json_stream), marshalling_map(&marshalling_map) { }

UNMARSHALLING_RESULT JSONNodeReader::BuildNode() {
	node_state_stack.push(NODE_STATE_BEGIN);

	working_result.result = COMPLETE;
	while (!node_state_stack.empty()) {
		NODE_STATE state = node_state_stack.top();
		node_state_stack.pop();

		switch (state) {
		case NODE_STATE_BEGIN:
			NodeStateBeginRoutine();
			break;
		case NODE_STATE_TYPE_CHECK:
			NodeStateTypeCheck();
			break;
		case NODE_STATE_TYPE_SET:
			NodeStateTypeSet();
			break;
		case NODE_STATE_PROPERTY_VALUE_DELIMITOR:
			NodeStatePropertyValueDelimitor();
			break;
		case NODE_STATE_PROPERTY_OPEN:
			NodeStatePropertyOpen();
			break;
		case NODE_STATE_PROPERTY_FIRST_CHAR:
			NodeStatePropertyFirstChar();
			break;
		case NODE_STATE_PROPERTY_SUBSEQUENT_CHARS:
			NodeStatePropertySubsequentChars();
			break;
		case NODE_STATE_PROPERTY_CLOSE:
			NodeStatePropertyClose();
			break;
		case NODE_STATE_VALUE_SEARCH:
			NodeStateValueSearch();
			break;
		case NODE_STATE_VALUE_FOUND:
			NodeStateValueFound();
			break;
		case NODE_STATE_PROPERTY_DELIMITOR:
			NodeStatePropertyDelimitor();
			break;
		case NODE_STATE_VALUE_SIGNED_NUMBER:
			NodeStateValueSignedNumber();
			break;
		case NODE_STATE_VALUE_INT:
			NodeStateValueInt();
			break;
		case NODE_STATE_VALUE_FLOAT:
			NodeStateValueFloat();
			break;
		case NODE_STATE_VALUE_STRING_OPEN:
			NodeStateValueStringOpen();
			break;
		case NODE_STATE_VALUE_STRING_CHAR:
			NodeStateValueStringChar();
			break;
		case NODE_STATE_VALUE_STRING_ESCAPE:
			NodeStateValueStringEscape();
			break;
		case NODE_STATE_VALUE_BOOL:
			NodeStateValueBool();
			break;
		case NODE_STATE_VALUE_OBJECT:
			NodeStateValueObject();
			break;
		case NODE_STATE_VALUE_ARRAY_OPEN:
			NodeStateValueArrayOpen();
			break;
		case NODE_STATE_ARRAY_COUNT:
			NodeStateArrayCount();
			break;
		case NODE_STATE_ARRAY_COUNT_RESET:
			NodeStateArrayCountReset();
			break;
		case NODE_STATE_VALUE_ARRAY_DELIMITOR:
			NodeStateValueArrayDelimitor();
			break;
		case NODE_STATE_VALUE_ARRAY_CLOSE:
			NodeStateValueArrayClose();
			break;
		case NODE_STATE_VALUE_SET:
			NodeStateValueSet();
			break;
		case NODE_STATE_FAST_FORWARD:
			NodeStateFastForward();
			break;
		}
	}

	return working_result;
}

Jigsaw::Marshalling::JSONNodeReader::JSONNodeReader(std::istringstream& json_stream, const MarshallingMap& marshalling_map, void* obj_target, void* const context)
	: json_stream(json_stream), marshalling_map(&marshalling_map), context(context) {

	if (marshalling_map.IsContextInitialized() && obj_target) {
		void* new_item = marshalling_map.Manufacture(context);
		marshalling_map.GetType().GetUnsafeFunctions().Move(static_cast<BYTE*>(obj_target), static_cast<BYTE*>(new_item));
		delete new_item;
	}

	working_result.raw_data = obj_target;
}

void JSONNodeReader::NodeStateBeginRoutine() {
	while (json_stream.peek() != EOF) {
		char c;
		json_stream >> c;
		switch (c) {
		case FORMATTING_JUNK:
			continue;
		case '{':
			node_state_stack.push(NODE_STATE_TYPE_CHECK);
			return;
		default:
			throw c;
		}
	}
}

void JSONNodeReader::NodeStateTypeCheck() {
	while (json_stream.peek() != EOF) {
		char c;
		json_stream >> c;
		switch (c) {
		case FORMATTING_JUNK:
			continue;
		case '}':
			if (working_result.raw_data == nullptr) {
				working_result.raw_data = marshalling_map->Manufacture(context);
			}
			return;
		case '"':
			goto TYPE_CHAR;
		}
	}

TYPE_CHAR:
	char c = json_stream.peek();
	if (c != '@') { // at this point, we know there is no inheritance/type information, so we construct the node.
		if (working_result.raw_data == nullptr) {
			working_result.raw_data = marshalling_map->Manufacture(context);
		}
		node_state_stack.push(NODE_STATE_PROPERTY_FIRST_CHAR);
		return;
	}

	char type[7];
	type[6] = '\0';
	json_stream.read(type, 6);
	if (!json_stream.fail() && std::string(type) == std::string("@type\"")) {
		// the following steps need to be taken to set the type of the object
		node_state_stack.push(NODE_STATE_TYPE_SET);
		node_state_stack.push(NODE_STATE_VALUE_STRING_CHAR);
		node_state_stack.push(NODE_STATE_VALUE_STRING_OPEN);
		node_state_stack.push(NODE_STATE_VALUE_SEARCH);
		node_state_stack.push(NODE_STATE_PROPERTY_VALUE_DELIMITOR);
	}
	else {
		throw c;
	}
}

void JSONNodeReader::NodeStateTypeSet() {
	std::string qualified_type_name(accumulator.str());
	accumulator.str("");

	const etype_info& child_type = etype_info::GetByQualifiedName(qualified_type_name);
	const MarshallingMap& child_map = MarshallingRegistry::GetMarshallingMap(child_type);

	if (child_map.InheritsFrom(marshalling_map->GetType()) && working_result.raw_data == nullptr) {
		marshalling_map = &child_map;
		working_result.raw_data = marshalling_map->Manufacture(context);
	}

	node_state_stack.push(NODE_STATE_PROPERTY_DELIMITOR);
}

void JSONNodeReader::NodeStatePropertyValueDelimitor() {
	while (json_stream.peek() != EOF) {
		char c;
		json_stream >> c;
		switch (c) {
		case FORMATTING_JUNK:
			continue;
		case ':':
			return;
		default:
			throw c;
		}
	}
}

void JSONNodeReader::NodeStatePropertyOpen() {
	while (json_stream.peek() != EOF) {
		char c;
		json_stream >> c;
		switch (c) {
		case FORMATTING_JUNK:
			continue;
		case '"':
			node_state_stack.push(NODE_STATE_PROPERTY_FIRST_CHAR);
			return;
		case '}':
			return;
		default:
			throw c;
		}
	}
}

void JSONNodeReader::NodeStatePropertyFirstChar() {
	while (json_stream.peek() != EOF) {
		char c;
		json_stream >> c;
		switch (c) {
		case FORMATTING_JUNK:
			continue;
		case PROPERTY_OPEN_CHAR:
			accumulator << c;
			node_state_stack.push(NODE_STATE_PROPERTY_SUBSEQUENT_CHARS);
			return;
		default:
			throw c;
		}
	}
}

void JSONNodeReader::NodeStatePropertySubsequentChars() {
	while (json_stream.peek() != EOF) {
		char c;
		json_stream >> c;
		switch (c) {
		case FORMATTING_JUNK:
			continue;
		case PROPERTY_OPEN_CHAR:
		case PROPERTY_OTHER_CHARS:
			accumulator << c;
			continue;
		case '\"':
			node_state_stack.push(NODE_STATE_PROPERTY_CLOSE);
			return;
		default:
			throw c;
		}
	}
}

void JSONNodeReader::NodeStatePropertyClose() {
	target_field = marshalling_map->GetFieldDataByName(accumulator.str());
	accumulator.str("");

	// if the field is not present, then we build a mock node to fast forward past the data represented by the given node
	if (target_field == nullptr) {
		node_state_stack.push(NODE_STATE_PROPERTY_DELIMITOR);
		node_state_stack.push(NODE_STATE_FAST_FORWARD);
		node_state_stack.push(NODE_STATE_VALUE_SEARCH);
	}
	else if (target_field->IsArray()) {
		node_state_stack.push(NODE_STATE_VALUE_ARRAY_CLOSE);
		size_t array_size;
		if (target_field->IsBoundedArray(&array_size)) {
			array_building_data = static_cast<BYTE*>(target_field->getter(working_result.raw_data));
			node_state_stack.push(NODE_STATE_VALUE_SET);
		}
		else {
			node_state_stack.push(NODE_STATE_ARRAY_COUNT_RESET);
			node_state_stack.push(NODE_STATE_ARRAY_COUNT);
			array_capacity = 0;
		}
		node_state_stack.push(NODE_STATE_VALUE_SEARCH);
		node_state_stack.push(NODE_STATE_VALUE_ARRAY_OPEN);
	}
	else {
		node_state_stack.push(NODE_STATE_VALUE_SET);
		node_state_stack.push(NODE_STATE_VALUE_SEARCH);
	}

	node_state_stack.push(NODE_STATE_PROPERTY_VALUE_DELIMITOR);
}

void JSONNodeReader::NodeStateValueSearch() {

	while (json_stream.peek() != EOF) {
		char c = json_stream.peek();
		switch (c) {
		case FORMATTING_JUNK:
			json_stream.read(&c, 1);
			continue;
		case '\"':
		case PROPERTY_NUMERIC_CHARS:
		case '-':
		case '{':
		case 't':
		case 'f':
		case '[':
			node_state_stack.push(NODE_STATE_VALUE_FOUND);
			return;
		case ']':
			if (target_field->is_array) {
				return;
			}
		default:
			throw c;
		}
	}
}

void JSONNodeReader::NodeStateValueFound() {
	if (target_field == nullptr) {
		// this case is reachable if we are still trying to determine the type of the object or if we need to fast-forward.
		// Subsequent steps should already be determined in this case
		return;
	}
	else if (target_field->GetType() == etype_info::Id<unsigned int>()
		|| target_field->GetType() == etype_info::Id<unsigned long long>()
		|| target_field->GetType() == etype_info::Id<unsigned long>()
		|| target_field->GetType() == etype_info::Id<unsigned char>()) {

		node_state_stack.push(NODE_STATE_VALUE_INT);

	}
	else if (target_field->GetType() == etype_info::Id<int>()
		|| target_field->GetType() == etype_info::Id<long long>()
		|| target_field->GetType() == etype_info::Id<long>()
		|| target_field->GetType() == etype_info::Id<char>()
		|| target_field->GetType().GetProperties().is_enum) {

		node_state_stack.push(NODE_STATE_VALUE_INT);
		node_state_stack.push(NODE_STATE_VALUE_SIGNED_NUMBER);

	}
	else if (target_field->GetType() == etype_info::Id<float>()
		|| target_field->GetType() == etype_info::Id<double>()) {

		node_state_stack.push(NODE_STATE_VALUE_FLOAT);
		node_state_stack.push(NODE_STATE_VALUE_SIGNED_NUMBER);

	}
	else if (target_field->GetType() == etype_info::Id<std::string>()
		|| target_field->is_reference) {

		node_state_stack.push(NODE_STATE_VALUE_STRING_CHAR);
		node_state_stack.push(NODE_STATE_VALUE_STRING_OPEN);

	}
	else if (target_field->GetType() == etype_info::Id<bool>()) {

		node_state_stack.push(NODE_STATE_VALUE_BOOL);
	}
	else {
		node_state_stack.push(NODE_STATE_VALUE_OBJECT);
	}
}

void JSONNodeReader::NodeStateValueSignedNumber() {
	while (json_stream.peek() != EOF) {
		char c = json_stream.peek();
		switch (c) {
		case '-':
			json_stream >> c;
			accumulator << c;
		case PROPERTY_NUMERIC_CHARS:
			return;
		default:
			throw c;
		}
	}
}

void JSONNodeReader::NodeStatePropertyDelimitor() {
	while (json_stream.peek() != EOF) {
		char c;
		json_stream >> c;
		switch (c) {
		case ',':
			node_state_stack.push(NODE_STATE_PROPERTY_OPEN);
			return;
		case '}': // The object is complete
			return;
		case FORMATTING_JUNK:
			continue;
		default:
			throw c;
		}
	}
}

void JSONNodeReader::NodeStateValueInt() {
	while (json_stream.peek() != EOF) {
		char c = json_stream.peek();
		switch (c) {
		case PROPERTY_NUMERIC_CHARS:
			json_stream >> c;
			accumulator << c;
			continue;
		case FORMATTING_JUNK:
		case ',':
			return;
		case '}':
			if (!target_field->is_array) {
				return;
			}
		default:
			throw c;
		}
	}
}

void JSONNodeReader::NodeStateValueFloat() {
	while (json_stream.peek() != EOF) {
		char c = json_stream.peek();
		switch (c) {
		case PROPERTY_NUMERIC_CHARS:
			json_stream >> c;
			accumulator << c;
			continue;
		case '.':
			node_state_stack.push(NODE_STATE_VALUE_INT);
			json_stream >> c;
			accumulator << c;
			return;
		case FORMATTING_JUNK:
		case ',':
			return;
		case '}':
			if (!target_field->is_array) {
				return;
			}
		default:
			throw c;
		}
	}
}

void JSONNodeReader::NodeStateValueStringOpen() {
	while (json_stream.peek() != EOF) {
		char c;
		json_stream >> c;
		switch (c) {
		case FORMATTING_JUNK:
			continue;
		case '\"':
			return;
		default:
			throw c;
		}
	}
}

void JSONNodeReader::NodeStateValueStringChar() {
	while (json_stream.peek() != EOF) {

		char c;
		json_stream.read(&c, 1); // can't miss some of the characters bypassed by >> operator
		switch (c) {
		case '\\':
			node_state_stack.push(NODE_STATE_VALUE_STRING_CHAR);
			node_state_stack.push(NODE_STATE_VALUE_STRING_ESCAPE);
			return;
		case '"':
			return;
		default:
			accumulator << c;
			continue;
		}
	}
}

void JSONNodeReader::NodeStateValueStringEscape() {
	char c;
	json_stream >> c;
	switch (c) {
	case '\\':
	case '\"':
		accumulator << c;
		return;
	case 'n':
		accumulator << '\n';
		return;
	case 't':
		accumulator << '\t';
		return;
	case 'r':
		accumulator << '\r';
		return;
	case 'b':
		accumulator << '\b';
		return;
	case 'f':
		accumulator << '\f';
		return;
	case '/':
		accumulator << '\/';
		return;
	default:
		throw c;
	}
}

void JSONNodeReader::NodeStateValueBool() {
	char c;
	int read_ct = 0;
	json_stream >> c;
	if (c == 't') {
		read_ct = 3;
	}
	else if (c == 'f') {
		read_ct = 4;
	}
	else {
		throw c;
	}
	accumulator << c;

	for (int i = 0; i < read_ct; i++) {
		json_stream >> c;
		if (c == EOF) {
			throw c;
		}
		accumulator << c;
	}
}

void Jigsaw::Marshalling::JSONNodeReader::NodeStateValueObject() {
	unsigned int opening_count = 0;
	while (json_stream.peek() != EOF) {
		char c;
		json_stream.read(&c, 1);
		accumulator << c;
		switch (c) {
		case '{':
			opening_count++;
			break;
		case '}':
			opening_count--;
			if (opening_count == 0) {
				return;
			}
			else if (opening_count < 0) {
				throw c;
			}
			break;
		default:
			break;
		}
	}
}

void JSONNodeReader::NodeStateValueSet() {
	const etype_info& t_inf = target_field->GetType();

	std::string str(accumulator.str());
	accumulator.str("");
	void* primitive_val = nullptr;
	if (t_inf == etype_info::Id<int>()
		|| t_inf.GetProperties().is_enum) {
		primitive_val = new int(std::stoi(str));
	}
	else if (t_inf == etype_info::Id<long long>()) {
		primitive_val = new long long(std::stoll(str));
	}
	else if (t_inf == etype_info::Id<unsigned int>()
		|| t_inf == etype_info::Id<unsigned long>()) {
		primitive_val = new unsigned int(std::stoul(str));
	}
	else if (t_inf == etype_info::Id<unsigned long long>()) {
		primitive_val = new unsigned long long(std::stoull(str));
	}
	else if (t_inf == etype_info::Id<std::string>()
		|| target_field->is_reference) {
		primitive_val = new std::string(str);
	}
	else if (t_inf == etype_info::Id<float>()) {
		primitive_val = new float(std::stof(str));
	}
	else if (t_inf == etype_info::Id<double>()) {
		primitive_val = new double(std::stod(str));
	}
	else if (t_inf == etype_info::Id<bool>()) {
		primitive_val = new bool(std::string("true") == str);
	}
	else { // this block handles all other objects
		std::istringstream c_json_stream(str);
		void* obj;
		if (target_field->is_pointer) {
			obj = nullptr;
		}
		else if (target_field->IsArray()) {
			obj = array_building_data + (no_of_array_elements * target_field->GetType().size);
		}
		else {
			obj = target_field->getter(working_result.raw_data);
		}

		// if obj is non-null, then the raw_data of the child result will be a pointer to the same obj
		JSONNodeReader child(c_json_stream, MarshallingRegistry::GetMarshallingMap(t_inf), obj, working_result.raw_data);

		UNMARSHALLING_RESULT child_result = child.BuildNode();
		if (target_field->is_pointer) {
			if (target_field->IsArray()) {
				target_field->GetType().GetUnsafeFunctions().MovePointerIntoPointerArray(array_building_data,
					child_result.raw_data, no_of_array_elements);
			}
			else {
				target_field->setter(working_result.raw_data, child_result.raw_data);
			}
		}

		for (UNRESOLVED_REFERENCE ref : child_result.unresolved_references) {
			working_result.result = UNRESOLVED_REFERENCES;
			working_result.unresolved_references.push_back(ref);
		}
	}

	if (target_field->IsArray()) {
		// for regular primitive_value types, the resolved primitive_value needs to be moved into the destination in the array
		if (primitive_val) {
			if (!target_field->is_pointer) {
				BYTE* dest = array_building_data + (no_of_array_elements * target_field->GetType().size);
				target_field->GetType().GetUnsafeFunctions().Move(dest, static_cast<BYTE*>(primitive_val));
				delete primitive_val;
			}
			else {
				// for pointers to regular primitive_values, the unique pionter needs to be pass
				target_field->GetType().GetUnsafeFunctions().MovePointerIntoPointerArray(array_building_data,
					primitive_val, no_of_array_elements);
			}
		}

		no_of_array_elements++;
		node_state_stack.push(NODE_STATE_VALUE_SET);
		node_state_stack.push(NODE_STATE_VALUE_ARRAY_DELIMITOR);
	}
	else if (target_field->is_reference) {
		// for reference types, we need to add an unresolved reference to the working result. 
		node_state_stack.push(NODE_STATE_PROPERTY_DELIMITOR);
		working_result.result = UNRESOLVED_REFERENCES;

		UNRESOLVED_REFERENCE unresolved_ref;
		Jigsaw::System::UIDFromString(&unresolved_ref.ref_id, static_cast<std::string*>(primitive_val));
		delete primitive_val;

		// compiler optimizations make it simpler to specify these specifically
		void* const data_object = working_result.raw_data;
		const field_data* const field = target_field;

		// The injector will allow the external reference to be injected directly into this object
		unresolved_ref.injector = [data_object, field](void* ref) -> void { field->setter(data_object, ref); };
		working_result.unresolved_references.push_back(unresolved_ref);
	}
	else {
		if (primitive_val) {
			target_field->setter(working_result.raw_data, primitive_val);
			delete primitive_val;
		}
		node_state_stack.push(NODE_STATE_PROPERTY_DELIMITOR);
	}

	primitive_val = nullptr;
}

void JSONNodeReader::NodeStateValueArrayOpen() {
	while (json_stream.peek() != EOF) {
		char c;
		json_stream >> c;
		switch (c) {
		case FORMATTING_JUNK:
			continue;
		case '[':
			array_start_location = json_stream.tellg();
			return;
		default:
			throw c;
		}
	}
}

void Jigsaw::Marshalling::JSONNodeReader::NodeStateArrayCount() {
	if (!accumulator.str().empty()) {
		array_capacity++;
		node_state_stack.push(NODE_STATE_ARRAY_COUNT);
		node_state_stack.push(NODE_STATE_VALUE_ARRAY_DELIMITOR);
	}
	accumulator.str("");
}

void Jigsaw::Marshalling::JSONNodeReader::NodeStateArrayCountReset() {
	json_stream.seekg(array_start_location);
	node_state_stack.push(NODE_STATE_VALUE_SET);
	node_state_stack.push(NODE_STATE_VALUE_SEARCH);

	void* vector = nullptr;
	if (target_field->is_pointer) {
		target_field->GetType().GetUnsafeFunctions().BuildPointerVector(array_capacity, &vector);
	}
	else {
		target_field->GetType().GetUnsafeFunctions().BuildVector(array_capacity, &vector);
	}

	target_field->setter(working_result.raw_data, vector);
	void* temp_data;
	temp_data = target_field->getter(working_result.raw_data);
	target_field->GetType().GetUnsafeFunctions().GetVectorRawData(temp_data, target_field->is_pointer, &temp_data);
	array_building_data = static_cast<BYTE*>(temp_data);
}

void JSONNodeReader::NodeStateValueArrayDelimitor() {
	auto next = node_state_stack.top();
	while (json_stream.peek() != EOF) {
		char c;
		json_stream >> c;
		switch (c) {
		case FORMATTING_JUNK:
			continue;
		case ']':
			if (next == NODE_STATE_VALUE_SET || next == NODE_STATE_ARRAY_COUNT) {
				node_state_stack.pop(); // we must remove the instruction to set or ignore a prospective value
			}
			return;
		case ',':
			size_t array_size;
			if (target_field->IsBoundedArray(&array_size) && no_of_array_elements == array_size) {
				throw c;
			}
			node_state_stack.push(NODE_STATE_VALUE_SEARCH);
			return;
		default:
			throw c;
		}
	}
}

void JSONNodeReader::NodeStateValueArrayClose() {
	no_of_array_elements = 0;
	array_capacity = 0;
	array_building_data = nullptr;
	node_state_stack.push(NODE_STATE_PROPERTY_DELIMITOR);
}

void JSONNodeReader::NodeStateFastForward() {
	char opening_char = '\0';
	char closing_char = '\0';
	int opening_count = 1;
	bool read_final = false;

	char c;
	json_stream >> c;
	switch (c) {
	case '{':
		opening_char = '{';
		closing_char = '}';
		read_final = true;
		break;
	case '[':
		opening_char = '[';
		closing_char = ']';
		read_final = true;
		break;
	case '"':
	case PROPERTY_NUMERIC_CHARS:
	case 't':
	case 'f':
	case '-':
		closing_char = ',';
		break;
	}

	while (json_stream.peek() != EOF) {
		c = json_stream.peek();
		if (c == opening_char) {
			json_stream.read(&c, 1);
			opening_count++;
			continue;
		}
		else if (c == closing_char && --opening_count == 0) {
			if (read_final) {
				json_stream.read(&c, 1);
			}
			return;
		}
		else if ((closing_char != ']' && closing_char != '}' && c == ']') || (closing_char != ']' && closing_char != '}' && c == '}')) {
			return;
		}
		else {
			json_stream.read(&c, 1);
		}
	}

	throw c;
}

// JSONNodeWriter implementations follow

/// <summary>
/// Initial constructor
/// </summary>
/// <param name="target"></param>
/// <param name="marshalling_map"></param>
Jigsaw::Marshalling::JSONNodeWriter::JSONNodeWriter(void* target, const Jigsaw::Marshalling::MarshallingMap& marshalling_map) : marshalling_map(&marshalling_map), target(target) {
}

MARSHALLING_RESULT Jigsaw::Marshalling::JSONNodeWriter::ReadNode() {
	node_state_stack.push(NODE_STATE_END);
	node_state_stack.push(NODE_STATE_INDENT);
	node_state_stack.push(NODE_STATE_BEGIN);

	working_result.result = COMPLETE;

	while (!node_state_stack.empty()) {
		NODE_STATE state = node_state_stack.top();
		node_state_stack.pop();

		switch (state) {
		case NODE_STATE_BEGIN:
			NodeStateBegin();
			break;
		case NODE_STATE_TYPE_SET:
			NodeStateTypeSet();
			break;
		case NODE_STATE_INDENT:
			NodeStateIndent();
			break;
		case NODE_STATE_END:
			NodeStateEnd();
			break;
		case NODE_STATE_VALUE_SEARCH:
			NodeStateValueSearch();
			break;
		case NODE_STATE_VALUE_FOUND:
			NodeStateValueFound();
			break;
		case NODE_STATE_PROPERTY_DELIMITOR:
			NodeStatePropertyDelimitor();
			break;
		case NODE_STATE_PROPERTY_OPEN:
			NodeStatePropertyOpen();
			break;
		case NODE_STATE_PROPERTY_VALUE_DELIMITOR:
			NodeStatePropertyValueDelimitor();
			break;
		case NODE_STATE_VALUE_SET:
			NodeStateValueSet();
			break;
		case NODE_STATE_VALUE_INT:
			NodeStateValueInt();
			break;
		case NODE_STATE_VALUE_BOOL:
			NodeStateValueBool();
			break;
		case NODE_STATE_VALUE_FLOAT:
			NodeStateValueFloat();
			break;
		case NODE_STATE_VALUE_OBJECT:
			NodeStateValueObject();
			break;
		case NODE_STATE_VALUE_STRING_OPEN:
			NodeStateValueStringOpen();
			break;
		case NODE_STATE_ARRAY_COUNT:
			NodeStateArrayCount();
			break;
		case NODE_STATE_ARRAY_COUNT_RESET:
			NodeStateArrayCountReset();
			break;
		case NODE_STATE_VALUE_ARRAY_OPEN:
			NodeStateArrayOpen();
			break;
		case NODE_STATE_VALUE_ARRAY_CLOSE:
			NodeStateArrayClose();
			break;
		case NODE_STATE_VALUE_ARRAY_DELIMITOR:
			NodeStateValueArrayDelimitor();
			break;
		}
	}
	working_result.marshalled_object = json_stream.str();
	return working_result;
}

Jigsaw::Marshalling::JSONNodeWriter::JSONNodeWriter(void* target, const Jigsaw::Marshalling::MarshallingMap& marshalling_map, unsigned int indent) : marshalling_map(&marshalling_map), target(target), indent(indent) { }

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateBegin() {
	json_stream << '{' << END_L;
	node_state_stack.push(NODE_STATE_TYPE_SET);
	node_state_stack.push(NODE_STATE_INDENT);
	indent++;
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateEnd()
{
	json_stream << '}';
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateIndent() {
	for (int i = 0; i < indent; i++) {
		json_stream << '\t';
	}
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateTypeSet() {
	const etype_info* other_type_info = marshalling_map->GetType().GetInstanceType(target);
	if (other_type_info) {
		marshalling_map = &MarshallingRegistry::GetMarshallingMap(*other_type_info);
	}

	object_fields = std::move(marshalling_map->GetFields());
	field_iterator = object_fields.begin();
	json_stream << "\"@type\":\"" << marshalling_map->GetType().GetQualifiedName() << "\"";
	node_state_stack.push(NODE_STATE_VALUE_SEARCH);
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStatePropertyDelimitor() {
	json_stream << "," << END_L;
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateValueSearch() {
	if (field_iterator != object_fields.end()) {
		target_field = *field_iterator;
		node_state_stack.push(NODE_STATE_VALUE_SEARCH);
		node_state_stack.push(NODE_STATE_VALUE_FOUND);
		field_iterator++;
		return;
	}
	json_stream << END_L;
	indent--;
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateValueFound() {
	if (target_field->IsArray()) {
		// the behaviors for arrays are distinct enough that they deserve their own node state to be 
		node_state_stack.push(NODE_STATE_ARRAY_COUNT);
		return;
	}
	else if (target_field->is_reference) {
		field_raw_data = target_field->getter(target);

		// if there is raw data available, then the getter has returned a pointer to a DataAsset of some kind
		// we need to write the std::string representation of that UID to the json output stream
		if (field_raw_data) {
			Jigsaw::Assets::DataAsset* d_asset = static_cast<Jigsaw::Assets::DataAsset*>(field_raw_data);
			Jigsaw::System::UID id = d_asset->GetDescriptor().id;
			std::string* str = Jigsaw::System::UIDToString(&id);
			field_raw_data = (void*)static_cast<const void*>(str);
			node_state_stack.push(NODE_STATE_VALUE_STRING_OPEN);
		}
		else {
			// if no raw data, then there is no active reference, and we can move on
			return;
		}
	}
	else {
		// this condition covers pointer objects and regular primitives
		// either way, if information is present, then it needs to be read/written
		field_raw_data = target_field->getter(target);
		if (field_raw_data) {
			node_state_stack.push(NODE_STATE_VALUE_SET);
		}
		else {
			return;
		}
	}
	node_state_stack.push(NODE_STATE_PROPERTY_VALUE_DELIMITOR);
	node_state_stack.push(NODE_STATE_PROPERTY_OPEN);
	node_state_stack.push(NODE_STATE_INDENT);
	node_state_stack.push(NODE_STATE_PROPERTY_DELIMITOR);
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStatePropertyOpen() {
	json_stream << "\"" << target_field->name << "\"";
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStatePropertyValueDelimitor() {
	json_stream << ":";
}

NODE_STATE GetNodeStateFromFieldType(const field_data* target_field) {
	if (target_field->GetType() == etype_info::Id<bool>()) {
		return NODE_STATE_VALUE_BOOL;
	}
	else if (target_field->GetType() == etype_info::Id<float>()
		|| target_field->GetType() == etype_info::Id<double>()) {
		return NODE_STATE_VALUE_FLOAT;
	}
	else if (target_field->GetType() == etype_info::Id<std::string>()) {
		return NODE_STATE_VALUE_STRING_OPEN;
	}
	if (target_field->GetType() == etype_info::Id<long long>()
		|| target_field->GetType() == etype_info::Id<long>()
		|| target_field->GetType() == etype_info::Id<char>()
		|| target_field->GetType() == etype_info::Id<int>()
		|| target_field->GetType().GetProperties().is_enum
		|| target_field->GetType() == etype_info::Id<short>()
		|| target_field->GetType() == etype_info::Id<unsigned long long>()
		|| target_field->GetType() == etype_info::Id<unsigned long>()
		|| target_field->GetType() == etype_info::Id<unsigned int>()
		|| target_field->GetType() == etype_info::Id<unsigned char>()
		|| target_field->GetType() == etype_info::Id<unsigned short>()) {
		return NODE_STATE_VALUE_INT;
	}
	else {
		return NODE_STATE_VALUE_OBJECT;
	}
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateValueSet() {
	node_state_stack.push(GetNodeStateFromFieldType(target_field));
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateValueInt() {
	if (target_field->GetType() == etype_info::Id<long long>()) {
		json_stream << *static_cast<long long*>(field_raw_data);
	}
	else if (target_field->GetType() == etype_info::Id<long>()) {
		json_stream << *static_cast<long*>(field_raw_data);
	}
	else if (target_field->GetType() == etype_info::Id<char>()) {
		json_stream << *static_cast<char*>(field_raw_data);
	}
	else if (target_field->GetType() == etype_info::Id<int>()
		|| target_field->GetType().GetProperties().is_enum) {

		json_stream << *static_cast<int*>(field_raw_data);
	}
	else if (target_field->GetType() == etype_info::Id<short>()) {
		json_stream << *static_cast<short*>(field_raw_data);
	}
	else if (target_field->GetType() == etype_info::Id<unsigned long long>()) {
		json_stream << *static_cast<unsigned long long*>(field_raw_data);
	}
	else if (target_field->GetType() == etype_info::Id<unsigned long>()) {
		json_stream << *static_cast<unsigned long*>(field_raw_data);
	}
	else if (target_field->GetType() == etype_info::Id<unsigned int>()) {
		json_stream << *static_cast<unsigned int*>(field_raw_data);
	}
	else if (target_field->GetType() == etype_info::Id<unsigned char>()) {
		json_stream << *static_cast<unsigned char*>(field_raw_data);
	}
	else {
		json_stream << *static_cast<unsigned short*>(field_raw_data);
	}
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateValueBool() {
	bool val = *static_cast<bool*>(field_raw_data);
	json_stream << (val ? "true" : "false");
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateValueFloat() {
	if (target_field->GetType() == etype_info::Id<float>()) {
		json_stream << *static_cast<float*>(field_raw_data);
	}
	else {
		json_stream << *static_cast<double*>(field_raw_data);
	}
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateValueObject() {
	JSONNodeWriter childWriter(field_raw_data, MarshallingRegistry::GetMarshallingMap(target_field->GetType()), indent);
	MARSHALLING_RESULT marshalling_result = childWriter.ReadNode();
	if (marshalling_result.result == COMPLETE) {
		json_stream << marshalling_result.marshalled_object;
	}
}

std::string replace(const std::string& original, std::string&& replace, std::string&& with) {
	std::string working_copy = original;
	size_t index = working_copy.find(replace);
	while (index != std::string::npos) {

		working_copy = working_copy.substr(0, index) + with + working_copy.substr(index + replace.size());
		size_t start_dex = index + with.size();
		index = working_copy.find(replace, start_dex);
	}
	return working_copy;
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateValueStringOpen() {
	std::string val = *static_cast<std::string*>(field_raw_data);
	val = replace(val, "\n", "\\n");
	val = replace(val, "\r", "\\r");
	val = replace(val, "\t", "\\t");
	val = replace(val, "\"", "\\\"");
	json_stream << '\"' << val << '\"';

	if (target_field->is_reference) { // if we're dealing with a reference, this is a temporary value
		delete field_raw_data;
	}
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateArrayOpen() {
	json_stream << '[' << END_L;
	indent++;
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateArrayCount() {
	unsigned int array_capacity = 0;
	if (target_field->IsBoundedArray(&array_capacity)) {
		field_raw_data = target_field->getter(target);
	}
	else {
		void* vector_data = target_field->getter(target);
		target_field->GetType().GetUnsafeFunctions().GetElementCount(vector_data, target_field->is_pointer, &array_capacity);
		target_field->GetType().GetUnsafeFunctions().GetVectorRawData(vector_data, target_field->is_pointer, &field_raw_data);

		if (target_field->is_pointer && array_capacity > 0) {
			array_data = field_raw_data;
			field_raw_data = *static_cast<void**>(array_data);
		}
	}

	if (array_capacity > 0) {
		node_state_stack.push(NODE_STATE_VALUE_ARRAY_CLOSE);
		node_state_stack.push(NODE_STATE_INDENT);
		node_state_stack.push(NODE_STATE_ARRAY_COUNT_RESET);

		// registering multiple instructions for populating the json array
		NODE_STATE array_type_state = GetNodeStateFromFieldType(target_field);
		for (int i = 0; i < array_capacity; i++) {
			if (i != 0) {
				node_state_stack.push(NODE_STATE_VALUE_ARRAY_DELIMITOR);
			}
			node_state_stack.push(array_type_state);
			node_state_stack.push(NODE_STATE_INDENT);
		}

		node_state_stack.push(NODE_STATE_VALUE_ARRAY_OPEN);

		// instructions needed to open the property name
		node_state_stack.push(NODE_STATE_PROPERTY_VALUE_DELIMITOR);
		node_state_stack.push(NODE_STATE_PROPERTY_OPEN);
		node_state_stack.push(NODE_STATE_INDENT);
		node_state_stack.push(NODE_STATE_PROPERTY_DELIMITOR);
	}
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateArrayCountReset() {
	json_stream << END_L;
	indent--;
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateValueArrayDelimitor() {
	if (target_field->is_pointer) {
		array_data = static_cast<BYTE*>(array_data) + 4;// moving one pointer value, 4 bytes
		field_raw_data = *static_cast<void**>(array_data);
	}
	else {
		field_raw_data = static_cast<BYTE*>(field_raw_data) + target_field->GetType().size;
	}
	json_stream << ',' << END_L;
}

void Jigsaw::Marshalling::JSONNodeWriter::NodeStateArrayClose() {
	json_stream << ']';
}
