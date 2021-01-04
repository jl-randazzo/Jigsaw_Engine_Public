#ifndef _BYTE_H_
#define _BYTE_H_
#include "Ref.h"

 typedef unsigned char BYTE;

 __declspec(align(alignof(max_align_t))) struct _RAW_DATA {};

namespace Jigsaw {
	namespace Util {
		template <typename T>
		constexpr BYTE* CastToByteArray(T* element) {
			void* raw = static_cast<void*>(element);
			return static_cast<BYTE*>(raw);
		}

		template <typename T>
		constexpr T* CastToTypeObject(BYTE* data) {
			void* raw = static_cast<void*>(data);
			return static_cast<T*>(raw);
		}

		constexpr const char hexmap[] = { '0', '1', '2', '3', '4', '5', '6', '7',
						   '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };


		inline BYTE ConvertToBits(char character) {
			return character > '9' ? character > 'Z' ? character - 0x57 : character - 0x37 : character - 0x30;
		}

		Jigsaw::Unique<char[]> BytesToHexStr(BYTE* data, int len);

		Jigsaw::Unique<char[]> BytesToSQLHexStr(BYTE* data, int len);

		Jigsaw::Unique<BYTE[]> HexStrToBytes(const char* hex, int len);
	}
}
#endif