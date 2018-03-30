#include <iostream>
#include "funset.hpp"

int main()
{
	int ret = test_recognize_image_content_1();

	if (ret == 0) fprintf(stdout, "========== test success ==========\n");
	else fprintf(stderr, "########## test fail ##########\n");

	return 0;
}


