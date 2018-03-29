#include <iostream>
#include "funset.hpp"

int main()
{
	int ret = test_tesseract_ocr_1();

	if (ret == 0) fprintf(stdout, "========== test success ==========\n");
	else fprintf(stderr, "########## test fail ##########\n");

	return 0;
}


