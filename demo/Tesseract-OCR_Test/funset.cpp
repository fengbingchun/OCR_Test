#include "funset.hpp"

#include <string.h>
#include <iostream>
#include <string>
#include <vector>

#include <allheaders.h>

#include <baseapi.h>
#include <basedir.h>
#include <renderer.h>
#include <strngs.h>
#include <tprintf.h>
#include <openclwrapper.h>

namespace {

void utf8_to_gbk(const char* utf8, char* gbk)
{
#ifdef _MSC_VER
	const int maxlen = 128;
	wchar_t unicode_str[maxlen];
	int outlen = MultiByteToWideChar(CP_UTF8, 0, utf8, strlen(utf8), unicode_str, maxlen);
	outlen = WideCharToMultiByte(CP_ACP, 0, unicode_str, outlen, gbk, 128, nullptr, nullptr);
	gbk[outlen] = '\0';
#else
	strcpy(gbk, utf8);
#endif
}

} // namespace

int test_recognize_image_content_1()
{
	// Blog: https://blog.csdn.net/fengbingchun/article/details/79757639

	// reference: https://github.com/tesseract-ocr/tesseract/wiki/APIExample
{ // chinese
	// Initialize tesseract-ocr
	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
#ifdef _MSC_VER
	const char* datapath{ "E:/GitCode/OCR_Test/test_data/tessdata/" };
#else
	const char* datapath{ "test_data/tessdata/" };
#endif
	if (api->Init(datapath, "chi_sim")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		return -1;
	}

	// Open input image with leptonica library
#ifdef _MSC_VER
	Pix* image = pixRead("E:/GitCode/OCR_Test/test_data/chi_sim_1.png");
#else
	Pix* image = pixRead("test_data/chi_sim_1.png");
#endif
	api->SetImage(image);
	// Get OCR result
	char* outText = api->GetUTF8Text();
	char gbk[256];
	utf8_to_gbk(outText, gbk);
	fprintf(stdout, "OCR output: %s\n", gbk);

	// Destroy used object and release memory
	api->End();
	delete api;
	delete[] outText;
	pixDestroy(&image);
}

{ // english
	// Initialize tesseract-ocr
	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
#ifdef _MSC_VER
	const char* datapath{ "E:/GitCode/OCR_Test/test_data/tessdata/" };
#else
	const char* datapath{ "test_data/tessdata/" };
#endif
	if (api->Init(datapath, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		return -1;
	}

	// Open input image with leptonica library
#ifdef _MSC_VER
	Pix* image = pixRead("E:/GitCode/OCR_Test/test_data/eng_1.png");
#else
	Pix* image = pixRead("test_data/eng_1.png");
#endif
	api->SetImage(image);
	// Get OCR result
	char* outText = api->GetUTF8Text();
	char gbk[256];
	utf8_to_gbk(outText, gbk);
	fprintf(stdout, "OCR output: %s\n", gbk);

	// Destroy used object and release memory
	api->End();
	delete api;
	delete[] outText;
	pixDestroy(&image);
}

	return 0;
}

//////////////////////////////////////////////////////////////////
namespace {

void PrintVersionInfo()
{
	char *versionStrP;

	printf("tesseract %s\n", tesseract::TessBaseAPI::Version());

	versionStrP = getLeptonicaVersion();
	printf(" %s\n", versionStrP);
	lept_free(versionStrP);

	versionStrP = getImagelibVersions();
	printf("  %s\n", versionStrP);
	lept_free(versionStrP);
}

void PrintUsage(const char* program)
{
	printf(
		"Usage:\n"
		"  %s --help | --help-psm | --version\n"
		"  %s --list-langs [--tessdata-dir PATH]\n"
		"  %s --print-parameters [options...] [configfile...]\n"
		"  %s imagename|stdin outputbase|stdout [options...] [configfile...]\n",
		program, program, program, program);
}

void PrintHelpForPSM()
{
	const char* msg =
		"Page segmentation modes:\n"
		"  0    Orientation and script detection (OSD) only.\n"
		"  1    Automatic page segmentation with OSD.\n"
		"  2    Automatic page segmentation, but no OSD, or OCR.\n"
		"  3    Fully automatic page segmentation, but no OSD. (Default)\n"
		"  4    Assume a single column of text of variable sizes.\n"
		"  5    Assume a single uniform block of vertically aligned text.\n"
		"  6    Assume a single uniform block of text.\n"
		"  7    Treat the image as a single text line.\n"
		"  8    Treat the image as a single word.\n"
		"  9    Treat the image as a single word in a circle.\n"
		" 10    Treat the image as a single character.\n";

	printf("%s", msg);
}

void PrintHelpMessage(const char* program)
{
	PrintUsage(program);

	const char* ocr_options =
		"OCR options:\n"
		"  --tessdata-dir PATH   Specify the location of tessdata path.\n"
		"  --user-words PATH     Specify the location of user words file.\n"
		"  --user-patterns PATH  Specify the location of user patterns file.\n"
		"  -l LANG[+LANG]        Specify language(s) used for OCR.\n"
		"  -c VAR=VALUE          Set value for config variables.\n"
		"                        Multiple -c arguments are allowed.\n"
		"  -psm NUM              Specify page segmentation mode.\n"
		"NOTE: These options must occur before any configfile.\n";

	printf("\n%s\n", ocr_options);
	PrintHelpForPSM();

	const char *single_options =
		"Single options:\n"
		"  -h, --help            Show this help message.\n"
		"  --help-psm            Show page segmentation modes.\n"
		"  -v, --version         Show version information.\n"
		"  --list-langs          List available languages for tesseract engine.\n"
		"  --print-parameters    Print tesseract parameters to stdout.\n";

	printf("\n%s", single_options);
}

void SetVariablesFromCLArgs(tesseract::TessBaseAPI* api, int argc, char** argv)
{
	char opt1[256], opt2[255];
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
			strncpy(opt1, argv[i + 1], 255);
			opt1[255] = '\0';
			char *p = strchr(opt1, '=');
			if (!p) {
				fprintf(stderr, "Missing = in configvar assignment\n");
				exit(1);
			}
			*p = 0;
			strncpy(opt2, strchr(argv[i + 1], '=') + 1, 255);
			opt2[254] = 0;
			++i;

			if (!api->SetVariable(opt1, opt2)) {
				fprintf(stderr, "Could not set option: %s=%s\n", opt1, opt2);
			}
		}
	}
}

void PrintLangsList(tesseract::TessBaseAPI* api)
{
	GenericVector<STRING> languages;
	api->GetAvailableLanguagesAsVector(&languages);
	printf("List of available languages (%d):\n", languages.size());
	for (int index = 0; index < languages.size(); ++index) {
		STRING& string = languages[index];
		printf("%s\n", string.string());
	}
	api->End();
}

void PrintBanner()
{
	tprintf("Tesseract Open Source OCR Engine v%s with Leptonica\n",
		tesseract::TessBaseAPI::Version());
}

void FixPageSegMode(tesseract::TessBaseAPI* api, tesseract::PageSegMode pagesegmode)
{
	if (api->GetPageSegMode() == tesseract::PSM_SINGLE_BLOCK)
		api->SetPageSegMode(pagesegmode);
}

// NOTE: arg_i is used here to avoid ugly *i so many times in this function
void ParseArgs(const int argc, char** argv, const char** lang, const char** image, const char** outputbase, const char** datapath,
	bool* list_langs, bool* print_parameters, GenericVector<STRING>* vars_vec, GenericVector<STRING>* vars_values, int* arg_i, tesseract::PageSegMode* pagesegmode)
{
	if (argc == 1) {
		PrintHelpMessage(argv[0]);
		exit(0);
	}

	if (argc == 2) {
		if ((strcmp(argv[1], "-h") == 0) ||
			(strcmp(argv[1], "--help") == 0)) {
			PrintHelpMessage(argv[0]);
			exit(0);
		}
		if ((strcmp(argv[1], "--help-psm") == 0)) {
			PrintHelpForPSM();
			exit(0);
		}
		if ((strcmp(argv[1], "-v") == 0) ||
			(strcmp(argv[1], "--version") == 0)) {
			PrintVersionInfo();
			exit(0);
		}
	}

	bool noocr = false;
	int i = 1;
	while (i < argc && (*outputbase == NULL || argv[i][0] == '-')) {
		if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
			*lang = argv[i + 1];
			++i;
		}
		else if (strcmp(argv[i], "--tessdata-dir") == 0 && i + 1 < argc) {
			*datapath = argv[i + 1];
			++i;
		}
		else if (strcmp(argv[i], "--user-words") == 0 && i + 1 < argc) {
			vars_vec->push_back("user_words_file");
			vars_values->push_back(argv[i + 1]);
			++i;
		}
		else if (strcmp(argv[i], "--user-patterns") == 0 && i + 1 < argc) {
			vars_vec->push_back("user_patterns_file");
			vars_values->push_back(argv[i + 1]);
			++i;
		}
		else if (strcmp(argv[i], "--list-langs") == 0) {
			noocr = true;
			*list_langs = true;
		}
		else if (strcmp(argv[i], "-psm") == 0 && i + 1 < argc) {
			*pagesegmode = static_cast<tesseract::PageSegMode>(atoi(argv[i + 1]));
			++i;
		}
		else if (strcmp(argv[i], "--print-parameters") == 0) {
			noocr = true;
			*print_parameters = true;
		}
		else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
			// handled properly after api init
			++i;
		}
		else if (*image == NULL) {
			*image = argv[i];
		}
		else if (*outputbase == NULL) {
			*outputbase = argv[i];
		}
		++i;
	}

	*arg_i = i;

	if (argc == 2 && strcmp(argv[1], "--list-langs") == 0) {
		*list_langs = true;
		noocr = true;
	}

	if (*outputbase == NULL && noocr == false) {
		PrintHelpMessage(argv[0]);
		exit(1);
	}
}

void PreloadRenderers(tesseract::TessBaseAPI* api, tesseract::PointerVector<tesseract::TessResultRenderer>* renderers,
	tesseract::PageSegMode pagesegmode, const char* outputbase)
{
	if (pagesegmode == tesseract::PSM_OSD_ONLY) {
		renderers->push_back(new tesseract::TessOsdRenderer(outputbase));
	}
	else {
		bool b;
		api->GetBoolVariable("tessedit_create_hocr", &b);
		if (b) {
			bool font_info;
			api->GetBoolVariable("hocr_font_info", &font_info);
			renderers->push_back(
				new tesseract::TessHOcrRenderer(outputbase, font_info));
		}

		api->GetBoolVariable("tessedit_create_tsv", &b);
		if (b) {
			bool font_info;
			api->GetBoolVariable("hocr_font_info", &font_info);
			renderers->push_back(
				new tesseract::TessTsvRenderer(outputbase, font_info));
		}

		api->GetBoolVariable("tessedit_create_pdf", &b);
		if (b) {
			renderers->push_back(new tesseract::TessPDFRenderer(outputbase,
				api->GetDatapath()));
		}

		api->GetBoolVariable("tessedit_write_unlv", &b);
		if (b) {
			renderers->push_back(new tesseract::TessUnlvRenderer(outputbase));
		}

		api->GetBoolVariable("tessedit_create_boxfile", &b);
		if (b) {
			renderers->push_back(new tesseract::TessBoxTextRenderer(outputbase));
		}

		api->GetBoolVariable("tessedit_create_txt", &b);
		if (b || renderers->empty()) {
			renderers->push_back(new tesseract::TessTextRenderer(outputbase));
		}
	}

	if (!renderers->empty()) {
		// Since the PointerVector auto-deletes, null-out the renderers that are
		// added to the root, and leave the root in the vector.
		for (int r = 1; r < renderers->size(); ++r) {
			(*renderers)[0]->insert((*renderers)[r]);
			(*renderers)[r] = NULL;
		}
	}
}

} // namespace

int test_tesseract_ocr_1()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/51628957
	const char* lang = "eng";
	const char* image = NULL;
	const char* outputbase = NULL;
	const char* datapath = NULL;
	bool list_langs = false;
	bool print_parameters = false;
	GenericVector<STRING> vars_vec, vars_values;
	int arg_i = 1;
	tesseract::PageSegMode pagesegmode = tesseract::PSM_AUTO;

	int argc = 1;
	std::vector<const char*> str;
#ifdef _MSC_VER
#ifdef _DEBUG
	str.emplace_back("E:/GitCode/OCR_Test/lib/dbg/x64_vc12/Tesseract-OCR_Test.exe");
#else
	str.emplace_back("E:/GitCode/OCR_Test/lib/rel/x64_vc12/Tesseract-OCR_Test.exe");
#endif
#else
	str.emplace_back("Tesseract-OCR_Test");
#endif
	char** argv = (char**)(&str[0]);

	ParseArgs(argc, argv,
		&lang, &image, &outputbase, &datapath,
		&list_langs, &print_parameters,
		&vars_vec, &vars_values, &arg_i, &pagesegmode);

	bool banner = false;
	if (outputbase != NULL && strcmp(outputbase, "-") && strcmp(outputbase, "stdout")) {
		banner = true;
	}

	PERF_COUNT_START("Tesseract:main")
		tesseract::TessBaseAPI api;

	api.SetOutputName(outputbase);

	int init_failed = api.Init(datapath, lang, tesseract::OEM_DEFAULT,
		&(argv[arg_i]), argc - arg_i, &vars_vec, &vars_values, false);
	if (init_failed) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	SetVariablesFromCLArgs(&api, argc, argv);

	if (list_langs) {
		PrintLangsList(&api);
		exit(0);
	}

	if (print_parameters) {
		FILE* fout = stdout;
		fprintf(stdout, "Tesseract parameters:\n");
		api.PrintVariables(fout);
		api.End();
		exit(0);
	}

	FixPageSegMode(&api, pagesegmode);

	if (pagesegmode == tesseract::PSM_AUTO_ONLY) {
		int ret_val = 0;

		Pix* pixs = pixRead(image);
		if (!pixs) {
			fprintf(stderr, "Cannot open input file: %s\n", image);
			exit(2);
		}

		api.SetImage(pixs);

		tesseract::Orientation orientation;
		tesseract::WritingDirection direction;
		tesseract::TextlineOrder order;
		float deskew_angle;

		tesseract::PageIterator* it = api.AnalyseLayout();
		if (it) {
			it->Orientation(&orientation, &direction, &order, &deskew_angle);
			tprintf("Orientation: %d\nWritingDirection: %d\nTextlineOrder: %d\n" \
				"Deskew angle: %.4f\n",
				orientation, direction, order, deskew_angle);
		}
		else {
			ret_val = 1;
		}

		delete it;

		pixDestroy(&pixs);
		exit(ret_val);
	}

	// set in_training_mode to true when using one of these configs:
	// ambigs.train, box.train, box.train.stderr, linebox, rebox
	bool b = false;
	bool in_training_mode =
		(api.GetBoolVariable("tessedit_ambigs_training", &b) && b) ||
		(api.GetBoolVariable("tessedit_resegment_from_boxes", &b) && b) ||
		(api.GetBoolVariable("tessedit_make_boxes_from_boxes", &b) && b);

	tesseract::PointerVector<tesseract::TessResultRenderer> renderers;

	if (in_training_mode) {
		renderers.push_back(NULL);
	}
	else {
		PreloadRenderers(&api, &renderers, pagesegmode, outputbase);
	}

	if (!renderers.empty()) {
		if (banner) PrintBanner();
		bool succeed = api.ProcessPages(image, NULL, 0, renderers[0]);
		if (!succeed) {
			fprintf(stderr, "Error during processing.\n");
			exit(1);
		}
	}

	PERF_COUNT_END
		return 0; // Normal exit
}

