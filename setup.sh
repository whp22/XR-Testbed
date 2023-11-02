#!/bin/sh
brctl addbr br-left
brctl addbr br-right
tunctl -t tap-left
tunctl -t tap-right
ifconfig tap-left 0.0.0.0 promisc up
ifconfig tap-right 0.0.0.0 promisc up
ifconfig enp6s0 0.0.0.0 promisc up
ifconfig enx3c18a0992c61 0.0.0.0 promisc up
brctl addif br-left tap-left enx3c18a0992c61
ifconfig br-left up
brctl addif br-right tap-right enp6s0
ifconfig br-right up
