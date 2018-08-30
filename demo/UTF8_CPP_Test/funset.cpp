#include "funset.hpp"
#include <string.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <assert.h>
#include "utf8.h"

namespace {

// checks whether the content of a file is valid UTF-8 encoded text without reading the content into the memory
bool valid_utf8_file(const char* file_name)
{
	std::ifstream ifs(file_name);
	if (!ifs)
		return false; // even better, throw here

	std::istreambuf_iterator<char> it(ifs.rdbuf());
	std::istreambuf_iterator<char> eos;

	return utf8::is_valid(it, eos);
}

// The function will replace any invalid UTF-8 sequence with a Unicode replacement character
void fix_utf8_string(std::string& str)
{
	std::string temp;
	utf8::replace_invalid(str.begin(), str.end(), back_inserter(temp));
	str = temp;
}

} // namespace

int test_utf8_1()
{
#ifdef _MSC_VER
	const char* test_file_path = "E:/GitCode/OCR_Test/test_data/utf8.txt";
#else
	const char* test_file_path = "test_data/utf8.txt";
#endif
	// Open the test file(contains UTF-8 encoded text)
	std::ifstream fs8(test_file_path);
	if (!fs8.is_open()) {
		std::cout << "Could not open " << test_file_path << std::endl;
		return -1;
	}

	if (!valid_utf8_file(test_file_path))
		return -1;

	unsigned line_count = 1;
	std::string line;
	// Play with all the lines in the file
	while (getline(fs8, line)) {
		// check for invalid utf-8 (for a simple yes/no check, there is also utf8::is_valid function)
		std::string::iterator end_it = utf8::find_invalid(line.begin(), line.end());
		if (end_it != line.end()) {
			std::cout << "Invalid UTF-8 encoding detected at line " << line_count << "\n";
			std::cout << "This part is fine: " << std::string(line.begin(), end_it) << "\n";
		}

		// Get the line length (at least for the valid part)
		int length = utf8::distance(line.begin(), end_it);
		std::cout << "Length of line " << line_count << " is " << length << "\n";

		// Convert it to utf-16
		std::vector<unsigned short> utf16line;
		utf8::utf8to16(line.begin(), end_it, back_inserter(utf16line));

		// And back to utf-8
		std::string utf8line;
		utf8::utf16to8(utf16line.begin(), utf16line.end(), back_inserter(utf8line));

		// Confirm that the conversion went OK:
		if (utf8line != std::string(line.begin(), end_it))
			std::cout << "Error in UTF-16 conversion at line: " << line_count << "\n";

		line_count++;
	}

	std::string str = "ABCD";
	std::vector<unsigned short> utf16result;
	utf8::utf8to16(str.begin(), str.end(), std::back_inserter(utf16result));
	size_t size1 = utf16result.size();

	std::string str2 = "濦粿夿旴";
	std::string utf8str;
	utf8::utf16to8(str2.begin(), str2.end(), std::back_inserter(utf8str));
	size_t size2 = utf8str.length();

	char utf8_with_surrogates[] = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
	std::vector <unsigned short> utf16result1;
	utf8::utf8to16(utf8_with_surrogates, utf8_with_surrogates + 9, back_inserter(utf16result1));
	assert(utf16result1.size() == 4);
	assert(utf16result1[2] == 0xd834);
	assert(utf16result1[3] == 0xdd1e);

	unsigned short utf16string[] = { 0x41, 0x0448, 0x65e5, 0xd834, 0xdd1e };
	std::vector<unsigned char> utf8result;
	utf8::utf16to8(utf16string, utf16string + 5, back_inserter(utf8result));
	assert(utf8result.size() == 10);

	char* szSex = "\xe7\x94\xb7\x00";
	std::basic_string<wchar_t> sex;
	utf8::utf8to16(szSex, szSex + strlen(szSex), back_inserter(sex));

	if (sex != L"男") {
		std::cout << "unicode char utf16 error" << std::endl;
		return -1;
	}

	return 0;
}

