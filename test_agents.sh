#!/bin/bash
echo "compilef('aggregate')." | ./emulator/oagents -b ./environment/boot.pam -b ./oldcompiler/comp.pam -b ./version.pam
