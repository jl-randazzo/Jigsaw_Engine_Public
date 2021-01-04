#ifndef _JSON_NODE_H_
#define _JSON_NODE_H_

#include <sstream>
#include <stack>
#include "MarshallingMap.h"
#include "MARSHALLING_RESULTS.h"

#define END_L "\r\n"
#define NODE_OPEN '{'
#define NODE_CLOSE '}'
#define ARRAY_OPEN '['
#define ARRAY_CLOSE ']'
#define FORMATTING_JUNK '\t': case '\n': case '\r': case ' '
#define PROPERTY_OPEN_CHAR 'a': case 'b': case 'c':case 'd':case 'e':case 'f':case 'g':case 'h':case 'i':case 'j':case 'k':case 'l':case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':case 's':case 't':case 'u':case 'v':case 'w':case 'x':case 'y': case 'z':\
case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':case 'G':case 'H':case 'I':case 'J':case 'K':case 'L':case 'M':case 'N':case 'O':case 'P':case 'Q':case 'R':case 'S':case 'T':case 'U':case 'V':case 'W':case 'X':case 'Y':case 'Z':case '_'
#define PROPERTY_NUMERIC_CHARS '0': case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9'
#define PROPERTY_OTHER_CHARS PROPERTY_NUMERIC_CHARS:case '$'

namespace Jigsaw {
	namespace Marshalling {
		/// <summary>
		/// NODE_STATE enumeration is used by the JSONNodeReader and JSONNodeWriter to specify what the state of the read/write operation
		/// </summary>
		enum NODE_STATE {
			NODE_STATE_BEGIN,
			NODE_STATE_END,
			NODE_STATE_TYPE_CHECK,
			NODE_STATE_TYPE_SET,
			NODE_STATE_PROPERTY_VALUE_DELIMITOR,
			NODE_STATE_PROPERTY_OPEN,
			NODE_STATE_PROPERTY_FIRST_CHAR,
			NODE_STATE_PROPERTY_SUBSEQUENT_CHARS,
			NODE_STATE_PROPERTY_CLOSE,
			NODE_STATE_VALUE_FOUND,
			NODE_STATE_PROPERTY_DELIMITOR,
			NODE_STATE_VALUE_SEARCH,
			NODE_STATE_VALUE_SIGNED_NUMBER,
			NODE_STATE_VALUE_INT,
			NODE_STATE_VALUE_FLOAT,
			NODE_STATE_VALUE_STRING_OPEN,
			NODE_STATE_VALUE_STRING_CHAR,
			NODE_STATE_VALUE_STRING_ESCAPE,
			NODE_STATE_VALUE_BOOL,
			NODE_STATE_VALUE_OBJECT,
			NODE_STATE_VALUE_SET,
			NODE_STATE_VALUE_ARRAY_OPEN,
			NODE_STATE_ARRAY_COUNT,
			NODE_STATE_VALUE_ARRAY_DELIMITOR,
			NODE_STATE_ARRAY_COUNT_RESET,
			NODE_STATE_VALUE_ARRAY_CLOSE,
			NODE_STATE_FAST_FORWARD,

			NODE_STATE_INDENT
		};

		/// <summary>
		/// JSONNodeReader is used to pull in a json stream and convert it into a runtime object
		/// </summary>
		class JSONNodeReader {
		public:
			JSONNodeReader(std::istringstream& json_stream, const MarshallingMap& marshalling_map);

			/// <summary>
			/// The only publicly available function. The internal state management is relatively complex, and it's not meant to be exposed
			/// to the user. JSONNodeReader will spawn child nodes to read internal objects. 
			/// </summary>
			/// <returns></returns>
			UNMARSHALLING_RESULT BuildNode();

		private:
			/// <summary>
			/// Private constructor is used to establish a link to the parent context. If child classes are context-initialized,
			/// then the context root needs to be passed from parent to child. The move-assignment operations of classes whose
			/// fields are context-initialized will need to be sensitive to the changing context (this) that results from a move operation.
			/// </summary>
			/// <param name="json_stream"></param>
			/// <param name="marshalling_map"></param>
			/// <param name="context_root"></param>
			JSONNodeReader(std::istringstream& json_stream, const MarshallingMap& marshalling_map, void* object_target, void* const context);


			void NodeStateBeginRoutine();
			void NodeStateTypeCheck();
			void NodeStateTypeSet();
			void NodeStatePropertyValueDelimitor();
			void NodeStatePropertyOpen();
			void NodeStatePropertyFirstChar();
			void NodeStatePropertySubsequentChars();
			void NodeStatePropertyClose();
			void NodeStatePropertyDelimitor();
			void NodeStateValueSearch();
			void NodeStateValueFound();
			void NodeStateValueSignedNumber();
			void NodeStateValueInt();
			void NodeStateValueFloat();
			void NodeStateValueStringOpen();
			void NodeStateValueStringChar();
			void NodeStateValueStringEscape();
			void NodeStateValueBool();
			void NodeStateValueObject();
			void NodeStateValueSet();
			void NodeStateValueArrayOpen();
			void NodeStateArrayCount();
			void NodeStateArrayCountReset();
			void NodeStateValueArrayDelimitor();
			void NodeStateValueArrayClose();
			void NodeStateFastForward();

			UNMARSHALLING_RESULT working_result;
			std::istringstream& json_stream;
			const MarshallingMap* marshalling_map;
			std::stack<NODE_STATE> node_state_stack;

			const field_data* target_field;
			std::ostringstream accumulator;
			BYTE* array_building_data;
			unsigned int no_of_array_elements;
			unsigned int array_capacity = 0;

			std::streampos array_start_location;
			void* const context = nullptr;
		};

		class JSONNodeWriter {
		public:
			JSONNodeWriter(void* target, const Jigsaw::Marshalling::MarshallingMap& marshalling_map);

			MARSHALLING_RESULT ReadNode();

		private:
			JSONNodeWriter(void* target, const Jigsaw::Marshalling::MarshallingMap& marshalling_map, unsigned int indent);

			void NodeStateBegin();
			void NodeStateEnd();
			void NodeStateIndent();
			void NodeStateTypeSet();
			void NodeStatePropertyDelimitor();
			void NodeStateValueSearch();
			void NodeStateValueFound();
			void NodeStatePropertyOpen();
			void NodeStatePropertyValueDelimitor();
			void NodeStateValueSet();
			void NodeStateValueInt();
			void NodeStateValueBool();
			void NodeStateValueFloat();
			void NodeStateValueObject();
			void NodeStateValueStringOpen();
			void NodeStateArrayOpen();
			void NodeStateArrayCount();
			void NodeStateArrayCountReset();
			void NodeStateValueArrayDelimitor();
			void NodeStateArrayClose();

			void* target;

			void* field_raw_data;
			void* array_data;

			MARSHALLING_RESULT working_result;
			std::ostringstream json_stream;
			const MarshallingMap* marshalling_map;
			std::stack<NODE_STATE> node_state_stack;

			std::vector<const field_data*> object_fields;
			std::vector<const field_data*>::iterator field_iterator;
			const field_data* target_field;
			unsigned int indent = 0;
			unsigned int array_index;
		};
	}
}
#endif
