
# LNvis

A Lightning Network Visualizer

<img src="https://jb55.com/s/a50cb0ae69cd2032.png"/>

## Usage

LNvis requires a json dump of nodes and channels. Currently only clightning
channel/node output is supported

    lightning-cli listnodes > clightning-nodes.json && \
    lightning-cli listchannels > clightning-channels.json && \
    ./lnvis --channels clightning-channels.json \
            --nodes clightning-nodes.json \
            --filter <starting-node-id>

## Controls

```
b             view multiple channels between nodes
t             toggle dark/light theme
f             toggle fps
a             toggle aliases
g             toggle grid
s             toggle stroked nodes (small perf boost)
left click    move + focus node
right click   filter node
```

## How it works

LNvis renders the Lightning Network channel gossip, which include nodes and the
edges (channels) between those nodes.

* Channels are colored by the node that opened the channel

* Channel widths are rendered proportional to the capacity

* Right clicking a node filters the view to that node and its neighbors

* Dragging a node in any view will focus that node and its neihbors

## Building

Install dependencies (`glfw3`, `pkg-config`)

    # Debian / Ubuntu
    $ sudo apt-get install libglfw3-dev pkg-config
    
    # MacOS
    brew install glfw3

Build

    $ make
