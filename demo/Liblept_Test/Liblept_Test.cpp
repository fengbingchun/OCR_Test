#include "funset.hpp"
#include <iostream>

int main()
{
	int ret = test_read_tiff();

	if (ret == 0) fprintf(stdout, "========== test success ==========\n");
	else fprintf(stderr, "########## test fail ##########\n");

	return 0;
}

