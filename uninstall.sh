#!/bin/bash

UNINSTALLER_EXE='app-uninstaller.exe'
BASE_DIR=$(cd `dirname $0`; pwd)

detect_cxoffice()
{
	local tmp=$(grep "X-Created-By" $Desktop_File | grep '=cxoffice-')
	if [ ! -z "$tmp" ]; then
		return 0
	else
		return 1
	fi
}

proceed_cxoffice()
{
	local tmp=${1#Exec=\"}
	tmp=${tmp%\" %u}
	if [ ! -f "$tmp" ]; then
		echo 'CrossOver link missing.'
		return
	fi
	tmp=$(cat $tmp | grep 'exec ')
	Bottle=${tmp#*--bottle \"}
	Bottle=${Bottle%%\" *}
	linkfile=${tmp#*--start \"}
	linkfile=${linkfile%%\" *}

	tmp=${linkfile/C:/"$HOME/.cxoffice/$Bottle/drive_c"}
	if [ ! -f "$tmp" ]; then
		echo "link file missing."
		return
	fi

	tmp=$(grep '\"Updater\" = ' "$HOME/.cxoffice/$Bottle/cxbottle.conf")
	tmp=${tmp#*\"Updater\" = }
	if [ $tmp != '""' ]; then
		echo "we can't modify a public bottle."
		return
	fi
	echo "$tmp"

	echo "cxoffice bottle: \"$Bottle\""
	echo "shell link file: \"$linkfile\""

	/opt/cxoffice/bin/wine --bottle="$Bottle" --verbose --wait-children -- "$BASE_DIR/$UNINSTALLER_EXE" "$linkfile"
}

detect_wine()
{
	return 1
}

proceed_wine()
{
	return
}

Desktop_File=$1
if [ -z $Desktop_File ]; then
	echo "no desktop given."
	exit 1
fi

if [ ! -f $Desktop_File ]; then
	echo "desktop file do not exists."
	exit 1
fi

echo "$Desktop_File"

Exec=$(cat "$Desktop_File" | grep 'Exec=')
detect_cxoffice "$Exec"
if [ $? -eq 0 ]; then
	proceed_cxoffice "$Exec"
	exit 0
fi
detect_wine
if [ $? -eq 0 ]; then
	proceed_wine "$Exec"
	exit 0
fi
exit 1
