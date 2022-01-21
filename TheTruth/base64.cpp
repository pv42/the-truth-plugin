#include "base64.h"

static const string base64chars =
"ABCDEFGHIJKLMNOP"
"QRSTUVWXYZabcdef"
"ghijklmnopqrstuv"
"wxyz0123456789\\_";


static inline bool is_base64(char c) {
	return isalnum(c) || c == '\\' || c == '_';
}

string base64encode(const void* buffer, size_t len) {
	string ret;
	char buff4[4];
	char buff3[3];
	int i = 0;
	int bufferPos = 0;
	while (len--) {
		buff3[i++] = ((char*)buffer)[bufferPos];
		if (i == 3) {
			buff4[0] = (buff3[0] & 0xfc) >> 2;
			buff4[1] = ((buff3[0] & 0x03) << 4) + ((buff3[1] & 0xf0) >> 4);
			buff4[2] = ((buff3[1] & 0x0f) << 2) + ((buff3[2] & 0xc0) >> 6);
			buff4[3] = buff3[2] & 0x3f;
			for (int j = 0; j < 4; j++) {
				ret += base64chars[buff4[j]];
			}
			i = 0;
		}
		bufferPos++;
	}
	if (i) {
		for (int j = i; j < 3; j++) {
			buff3[j] = '\0';
		}
		buff4[0] = (buff3[0] & 0xfc) >> 2;
		buff4[1] = ((buff3[0] & 0x03) << 4) + ((buff3[1] & 0xf0) >> 4);
		buff4[2] = ((buff3[1] & 0x0f) << 2) + ((buff3[2] & 0xc0) >> 6);
		buff4[3] = buff3[2] & 0x3f;
		for (int j = 0; j < 4; j++) {
			ret += base64chars[buff4[j]];
		}
		while ((i++ < 3))
			ret += '/';
	}
	return ret;
}

vector<char> base64decode(string const& base64) {
	int inputLen = base64.length();
	vector<char> out;
	char buff4[4];
	int inPos = 0;
	int i = 0;
	while (inputLen-- && is_base64(base64[inPos])) {
		buff4[i++] = base64chars.find(base64[inPos++]);
		if (i == 4) {
			out.push_back( (buff4[0]         << 2) + ((buff4[1] & 0x30) >> 4));
			out.push_back(((buff4[1] & 0x0f) << 4) + ((buff4[2] & 0x3c) >> 2));
			out.push_back(((buff4[2] & 0x03) << 6) +   buff4[3]);
			i = 0;
		}
	}
	if (i) {
		for (int j = i; j < 4; j++) {
			buff4[j] = 0;
		}
		out.push_back((buff4[0]          << 2) + ((buff4[1] & 0x30) >> 4));
		out.push_back(((buff4[1] & 0x0f) << 4) + ((buff4[2] & 0x3c) >> 2));
		out.push_back(((buff4[2] & 0x03) << 6) +   buff4[3]);
	}
	return out;
}
