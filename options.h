
#ifndef LNVIS_OPTIONS_H
#define LNVIS_OPTIONS_H


struct options {
	const char *channels;
	const char *nodes;
	const char *filter;
};


void parse_options(int argc, const char *argv[], struct options *options);

#endif /* LNVIS_OPTIONS_H */
