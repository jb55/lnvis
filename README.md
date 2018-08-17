
# LNvis

A Lightning Network Visualizer

## Usage

LNvis requires a json dump of nodes and channels. Currently only clightning
channel/node output is supported

    lightning-cli listnodes > clightning-nodes.json && \
    lightning-cli listchannels > clightning-channels.json && \
    ./lnvis
