#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	// Data options
	FILE *data_file;
	FILE *test_file;
	const char *format;

	// Transformation options
	const char *target_column;
	const char *predictors;
	char **interactions;
	size_t n_interactions;

	// Model options
	FILE *load_model;
	FILE *save_model;

	// Optimization options
	const char *loss;
	double lr;
	double l1;
	double l2;

	// Miscellaneous
	int verbose;
} razor_options_t;

void help(razor_options_t const options) {
	printf("Usage: razor-cli -d FILE [options]\n"
	       "\n"
	       "Data:\n"
	       "  -d, --data FILE               Input data path\n"
	       "  -t, --test FILE               Test data path\n"
	       "  -f, --format FORMAT           File format (default: %s)\n"
	       "\n",
	       "csv");

	printf("Transformations:\n"
	       "  -y, --target COL              Target column (default: %s)\n"
	       "  -x, --predictors SET          Predictor columns (default: all)\n"
	       "  -i, --interactions SET:SET    Add pairwise interaction terms\n\n"
	       "  SET      = all | none | COL | WILDCARD | COL[,COL...]\n"
	       "  COL      = exact column name\n"
	       "  WILDCARD = shell-style pattern (*, ?)\n"
	       "\n",
	       options.target_column);

	printf("Model:\n"
	       "  -L, --load FILE               Load initial model\n"
	       "  -S, --save FILE               Save trained model\n"
	       "\n");

	printf("Optimization:\n"
	       "  -r, --lr LR                   Learning rate (default: %.4g)\n"
	       "  -l, --loss LOSS               Loss function (default: %s)\n"
	       "  --l1 VALUE                    L1 regularization (default: %.4g)\n"
	       "  --l2 VALUE                    L2 regularization (default: %.4g)\n"
	       "\n",
	       options.lr, options.loss, options.l1, options.l2);

	printf("Miscellaneous:\n"
	       "  -v, --verbose                 Verbose output\n"
	       "  -h, --help                    Show this help\n"
	       "\n");
}

int main(int argc, char **argv) {
	razor_options_t options = {.format = "csv",
	                           .target_column = "target",
	                           .loss = "MSE",
	                           .lr = 0.01,
	                           .l1 = 1e-2,
	                           .l2 = 1e-3,
	                           .verbose = 0};

	static struct option long_options[] = {
	    // Data options.
	    {"data", required_argument, 0, 'd'},
	    {"test", required_argument, 0, 't'},
	    {"format", required_argument, 0, 'f'},

	    // Transformation options.
	    {"target", required_argument, 0, 'y'},
	    {"predictors", required_argument, 0, 'x'},
	    {"interactions", required_argument, 0, 'i'},

	    // Model options.
	    {"load", required_argument, 0, 'L'},
	    {"save", required_argument, 0, 'S'},

	    // Optimization options.
	    {"lr", required_argument, 0, 'r'},
	    {"loss", required_argument, 0, 'l'},
	    {"l1", required_argument, 0, 0},
	    {"l2", required_argument, 0, 0},

	    // Miscellaneous.
	    {"verbose", no_argument, 0, 'v'},
	    {"help", no_argument, 0, 'h'},
	    {0, 0, 0, 0}};

	int opt, long_index = 0;
	while ((opt = getopt_long(argc, argv, "d:t:f:y:x:i:L:S:r:l:vh",
	                          long_options, &long_index)) != -1) {
		switch (opt) {
		case 'd':
			options.data_file = fopen(optarg, "r");
			break;
		case 't':
			options.test_file = fopen(optarg, "r");
			break;
		case 'f':
			options.format = optarg;
			break;
		case 'L':
			options.load_model = fopen(optarg, "r");
			break;
		case 'S':
			options.save_model = fopen(optarg, "w");
			break;
		case 'y':
			options.target_column = optarg;
			break;
		case 'q':
			if (options.n_interactions < 64)
				options.interactions[options.n_interactions++] = optarg;
			break;
		case 'l':
			options.loss = optarg;
			break;
		case 'r':
			options.lr = atof(optarg);
			break;
		case 'v':
			options.verbose = 1;
			break;
		case 'h':
			help(options);
			exit(0);
			break;
		case 0:
			if (strcmp(long_options[long_index].name, "l1") == 0)
				options.l1 = atof(optarg);
			if (strcmp(long_options[long_index].name, "l2") == 0)
				options.l2 = atof(optarg);
			break;
		default:
			fprintf(stderr, "Unknown option\n");
			help(options);
			exit(1);
		}
	}

	if (options.verbose) {
		printf("Target: %s, Loss: %s, LR: %.4g, L1: %.4g, L2: %.4g\n",
		       options.target_column, options.loss, options.lr, options.l1,
		       options.l2);
		for (size_t i = 0; i < options.n_interactions; i++)
			printf("Interactions: %s\n", options.interactions[i]);
	}

	if (!options.data_file) {
		fprintf(stderr, "Input file is required.\n");
		help(options);
		exit(1);
	}

	// TODO: implement training logic

	return 0;
}
