#include <stdio.h>
#include <getopt.h>

int main(int argc, char** argv) {

	static struct option long_options[] = {
		{"output", required_argument, NULL, 'o'},
		{NULL, 0, NULL, 0}
	};

	int option;
	char* output_filename = "output.file";

	while ((option = getopt_long(argc, argv, "o:", long_options, NULL)) != -1) {
		switch (option) {
			case 'o':
				output_filename = optarg;
				break;
			default:
				fprintf(stderr, "Invalid parameter.\n");
				// TODO: call show_help()
		}
	}

	return 0;
}