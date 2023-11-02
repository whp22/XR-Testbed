#!/bin/sh
ifconfig br-left down
ifconfig br-right down
brctl delif br-left tap-left enp6s0
brctl delif br-right tap-right enx3c18a0992c61
brctl delbr br-left
brctl delbr br-right
ifconfig tap-left down
ifconfig tap-right down
tunctl -d tap-left
tunctl -d tap-right
