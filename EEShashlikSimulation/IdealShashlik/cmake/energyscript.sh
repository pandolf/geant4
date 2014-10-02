#!/bin/bash

for i in {"98","147","196","295","491"}; do
sed -i -e "s#/gun/energy .* MeV#/gun/energy ${i} MeV #g " run.mac
./runEEShashlik -m run.mac 
cp -r "EEShash.root" "EEShashIdeal_${i}.root"
done
