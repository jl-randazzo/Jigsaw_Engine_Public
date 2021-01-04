#include "BYTE.h"

Jigsaw::Unique<char[]> Jigsaw::Util::BytesToHexStr(BYTE* data, int len) {
	char* hex_str = new char[len * 2 + 1];
	hex_str[len * 2] = '\0';
	for (int i = 0; i < len; ++i) {
		hex_str[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
		hex_str[(2 * i) + 1] = hexmap[data[i] & 0x0F];
	}
	return Jigsaw::Unique<char[]>(hex_str);
}

Jigsaw::Unique<char[]> Jigsaw::Util::BytesToSQLHexStr(BYTE* data, int len) {
	char* hex_str = new char[len * 2 + 3];
	hex_str[0] = '\'';
	hex_str[len * 2 + 1] = '\'';
	hex_str[len * 2 + 2] = '\0';
	char* adj_start = &hex_str[1];
	for (int i = 0; i < len; ++i) {
		adj_start[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
		adj_start[2 * i + 1] = hexmap[data[i] & 0x0F];
	}
	return Jigsaw::Unique<char[]>(hex_str);
}

Jigsaw::Unique<BYTE[]> Jigsaw::Util::HexStrToBytes(const char* hex, int len)
{
	BYTE* byte_array = new BYTE[len / 2];

	for (int i = 0; i < len / 2; i++) {
		byte_array[i] = (BYTE)0;
		byte_array[i] |= (ConvertToBits(hex[i * 2]) << 4);
		byte_array[i] |= ConvertToBits(hex[i * 2 + 1]);
	}

	return Jigsaw::Unique<BYTE[]>(byte_array);
}
