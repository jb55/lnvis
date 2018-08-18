
# LNvis

[![Build Status](https://travis-ci.org/jb55/lnvis.svg)](https://travis-ci.org/jb55/lnvis)

A Lightning Network Visualizer


## Usage

LNvis requires a json dump of nodes and channels. Currently only clightning
channel/node output is supported

    lightning-cli listnodes > clightning-nodes.json && \
    lightning-cli listchannels > clightning-channels.json && \
    ./lnvis
    

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

## Building

Install dependencies (`glfw3`, `pkg-config`)

    $ sudo apt-get install libglfw3-dev pkg-config

Build

    $ make
