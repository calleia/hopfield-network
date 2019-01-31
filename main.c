#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "hopfield.h"

void memorize(char* input_path, char* output_path) {
	Pattern* pPattern = load_image(input_path);

	Network* pNetwork = memorize_pattern(pPattern);

	save_network(output_path, pNetwork);
}

void memorize_many(char** input_paths, char* output_path) {
	int i = 0;

	while (input_paths[i] != NULL) {
		fprintf(stdout, "{%s}\n", input_paths[i]);
		i++;
	}
}

void retrieve(char* input_path, char* output_path, char* network_path) {
	Pattern* pPattern = load_image(input_path);

	Network* pNetwork = load_network(network_path);

	Pattern* pRecovered = retrieve_pattern(pPattern, pNetwork);

	save_image(output_path, pRecovered);
}

int main(int argc, char** argv) {
	// Options struct
	static struct option long_options[] = {
		{"output", required_argument, NULL, 'o'},
		{NULL, 0, NULL, 0}
	};

	int option;
	char* output_filename = "output.file";

	// Parsing options
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

	// Remaining args that are not options
	if (optind < argc) {
		if (strcmp("memorize", argv[optind]) == 0) {
			if ((argc - optind) == 2) {
				memorize(argv[++optind], output_filename);
			} else if ((argc - optind) > 2) {
				char** list;
				list = malloc((argc - optind) * sizeof(*list));

				int listCount = 0;

				for (int i = optind + 1; i < argc; i++) {
					list[listCount] = argv[i];
					listCount++;
				}

				list[listCount] = NULL;

				memorize_many(list, output_filename);
			} else {
				fprintf(stderr, "No input was provided.\n");
			}
		} else if (strcmp("retrieve", argv[optind]) == 0) {
			if ((argc - optind) == 3) {
				retrieve(argv[optind + 1], output_filename, argv[optind + 2]);
			} else {
				fprintf(stderr, "You must provide an input and a network model.\n");
			}
		} else {
			fprintf(stderr, "Command '%s' not recognized.\n", argv[optind]);
		}
	} else {
		fprintf(stderr, "No command was provided.\n");
	}

	return 0;
}