#!/bin/bash

UNINSTALLER_EXE="app-uninstaller.exe"

Desktop_File=$1
if [ -z $Desktop_File ]; then
	echo "no desktop given."
	exit 1
fi

Exec=$(cat "$Desktop_File" | grep 'Exec=')
# we suppose this is a CrossOver desktop file. WINE support will be later.
Exec=${Exec#Exec=\"}
Exec=${Exec%\" %u}
Exec=$(cat $Exec | grep 'exec ')
Bottle=${Exec#*--bottle \"}
Bottle=${Bottle%%\" *}
linkfile=${Exec#*--start \"}
linkfile=${linkfile%%\" *}

echo "cxoffice bottle: \"$Bottle\""
echo "shell link file: \"$linkfile\""

/opt/cxoffice/bin/wine --bottle="$Bottle" --verbose --wait-children -- $UNINSTALLER_EXE "$linkfile"
