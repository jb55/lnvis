
#ifndef LNVIS_LN_H
#define LNVIS_LN_H

#include "nanovg/nanovg.h"
#include "defs.h"

#define CELL_MAX_ELEMS 32

struct node {
	const char *alias;
	const char *id; // pubkey

	union {
		float rgba[4];
		struct {
			float r, g, b, a;
		};
	} color;

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

struct channel {
	struct node *nodes[2];

	u64 our_msatoshi;
	/* Statistics for min and max our_msatoshi. */
	u64 msatoshi_to_us_min;
	u64 msatoshi_to_us_max;

	u32 last_update;
};

struct ln {
	NVGcontext *vg;

	int clicked;
	int mdown;
	double mx, my;
	int window_width;
	int window_height;

	int grid_div;
	struct cell *grid;

	struct node *drag_target;

	struct node *nodes;
	u32 node_count;

	struct channel *channels;
	u32 channel_count;
};

void init_ln(struct ln *ln, int grid_div);
void free_ln(struct ln *ln);
void random_network(int ww, int wh, int max_per_node, int num_nodes, struct ln *ln);

#endif /* LNVIS_LN_H */
