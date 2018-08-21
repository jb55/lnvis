
#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include "commander/commander.h"

static void opt_channels(command_t *self) {
	struct options *options = self->data;
	options->channels = self->arg;
};

static void opt_nodes(command_t *self) {
	struct options *options = self->data;
	options->nodes = self->arg;
};

static void opt_filter(command_t *self) {
	struct options *options = self->data;
	options->filter = self->arg;
};

static void fail(int code, const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(code);
}

void parse_options(int argc, const char *argv[], struct options *options)
{
	command_t cmd;
	options->channels = NULL;
	options->nodes = NULL;
	options->filter = NULL;

	cmd.data = (void*)options;

	command_init(&cmd, argv[0], "0.1");
	cmd.usage = "--channels channels.json --nodes nodes.json [options]";

	command_option(&cmd, "-c", "--channels <path>", "channels json", opt_channels);
	command_option(&cmd, "-n", "--nodes <path>", "nodes json", opt_nodes);
	command_option(&cmd, "-f", "--filter <nodeid>", "nodeid to filter on", opt_filter);

	command_parse(&cmd, argc, (char **)argv);

	if (options->channels == NULL || options->nodes == NULL)
		command_help(&cmd);
}
