
#ifndef LNVIS_LN_H
#define LNVIS_LN_H

#include "nanovg/nanovg.h"
#include "defs.h"

#define CELL_MAX_ELEMS 32
#define MAX_ALIAS_SIZE 32

// TODO: parse pubkeys
#define PUBKEY_SIZE 66

union color {
	float rgba[4];
	struct {
		float r, g, b, a;
	};
	NVGcolor nvg_color;
};

struct node {
	char alias[MAX_ALIAS_SIZE+1];
	char id[PUBKEY_SIZE+1];

	int filtered, mark_filtered;

	union color color;

	struct cell *cell;
	double x, y;
	double vx, vy;
	double ax, ay;

	float size;
};


// REMOTE: remote node opened the channel with local
// LOCAL: local node opened the channel with remote
enum side {
	LOCAL,
	REMOTE,
	NUM_SIDES
};

/*
 *   type: 256 (`channel_announcement`)
 *     - [`64`:`node_signature_1`]
 *     - [`64`:`node_signature_2`]
 *     - [`64`:`bitcoin_signature_1`]
 *     - [`64`:`bitcoin_signature_2`]
 *     - [`2`:`len`]
 *     - [`len`:`features`]
 *     - [`32`:`chain_hash`]
 *     - [`8`:`short_channel_id`]
 *     - [`33`:`node_id_1`]
 *     - [`33`:`node_id_2`]
 *     - [`33`:`bitcoin_key_1`]
 *     - [`33`:`bitcoin_key_2`]
 */

/* type: 258 (`channel_update`)
 *    - [`64`:`signature`]
 *    - [`32`:`chain_hash`]
 *    - [`8`:`short_channel_id`]
 *    - [`4`:`timestamp`]
 *    - [`2`:`flags`]
 *    - [`2`:`cltv_expiry_delta`]
 *    - [`8`:`htlc_minimum_msat`]
 *    - [`4`:`fee_base_msat`]
 *    - [`4`:`fee_proportional_millionths`]
 */

struct short_channel_id {
	u32 blocknum;
	u32 txnum;
	u16 outnum;
};

struct channel {
	struct node *nodes[2];

	char source[PUBKEY_SIZE+1];
	char destination[PUBKEY_SIZE+1];

	struct short_channel_id short_channel_id;
	u8 public;
	u8 active;

	u16 flags;

	u32 last_update_timestamp;
	u32 delay;
	u32 base_fee_msat;
	u32 fee_per_millionth;

	u64 satoshis;

	// app specific stuff
	int draw_last;
};

enum display_flags {
	DISP_DARK         = 1UL << 0,
	DISP_GRID         = 1UL << 1,
	DISP_ALIASES      = 1UL << 2,
	DISP_STROKE_NODES = 1UL << 3,
};

struct ln {
	NVGcontext *vg;

	int clicked;
	int mdown;
	double mx, my;
	int window_width;
	int window_height;

	char *filter;
	union color clear_color;
	u64 display_flags;

	int grid_div;
	struct cell *grid;

	struct node *drag_target;
	struct node *last_drag_target;

	struct node *nodes;
	u32 node_count;

	struct channel *channels;
	u32 channel_count;
};

void init_ln(struct ln *ln, int grid_div);
void free_ln(struct ln *ln);
void random_network(int ww, int wh, int max_per_node, int num_nodes, struct ln *ln);
void init_network(int ww, int wh, struct ln *ln);
void filter_network(const char *nodeid, struct node *filter_node, struct ln *ln);

#endif /* LNVIS_LN_H */
