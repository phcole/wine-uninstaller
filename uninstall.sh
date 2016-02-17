#!/bin/bash

UNINSTALLER_EXE='app-uninstaller.exe'
BASE_DIR=$(cd `dirname $0`; pwd)

detect_cxoffice()
{
	local tmp=$(grep "X-Created-By=" "$DESKTOP_FILE" | grep '=cxoffice-')
	if [ ! -z "$tmp" ]; then
		return 0
	else
		return 1
	fi
}

proceed_cxoffice()
{
	local tmp=$(grep 'Exec=' "$DESKTOP_FILE")
	tmp=${tmp#Exec=\"}
	tmp=${tmp%\" %u}
	if [ ! -f "$tmp" ]; then
		echo 'CrossOver link missing.'
		return
	fi
	tmp=$(cat $tmp | grep 'exec ')
	local Bottle=${tmp#*--bottle \"}
	Bottle=${Bottle%%\" *}
	local link_file=${tmp#*--start \"}
	link_file=${link_file%%\" *}

	tmp=${link_file/C:/"$HOME/.cxoffice/$Bottle/drive_c"}
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
	echo "shell link file: \"$link_file\""

	/opt/cxoffice/bin/wine --bottle="$Bottle" --verbose --wait-children -- "$BASE_DIR/$UNINSTALLER_EXE" "$link_file"
}

detect_wine()
{
	local tmp=$(grep 'Exec=' "$DESKTOP_FILE" | grep '=env WINEPREFIX="')
	if [ ! -z "$tmp" ]; then
		return 0
	else
		return 1
	fi
}

proceed_wine()
{
	local tmp=$(grep 'Exec=' "$DESKTOP_FILE")
	local prefix=${tmp#Exec=env WINEPREFIX=\"}
	prefix=${prefix%%\" *}
	echo "wine prefix:     $prefix"
	local wine_cmd=${tmp#Exec=env WINEPREFIX=\"*\" }
	wine_cmd=${wine_cmd%% *}
	echo "wine command:    $wine_cmd"
	local link_file=${tmp#*/dosdevices/}
	link_file=$(echo -e $link_file)
	link_file=${link_file//\\ / }
	echo "shell link file: $link_file"

	if [ ! -d "$prefix" ]; then
		echo "invalid wine prefix."
		return
	fi

	echo "Prefix:    $prefix"
	echo "wine cmd:  $wine_cmd"
	echo "link file: $link_file"

	env WINEPREFIX="$prefix" $wine_cmd "$BASE_DIR/$UNINSTALLER_EXE" "$link_file"
}

DESKTOP_FILE=$1
if [ -z "$DESKTOP_FILE" ]; then
	echo "no desktop given."
	exit 1
fi

if [ ! -f "$DESKTOP_FILE" ]; then
	echo "desktop file do not exists."
	exit 1
fi

echo "$DESKTOP_FILE"

detect_cxoffice
if [ $? -eq 0 ]; then
	proceed_cxoffice
	exit 0
fi
detect_wine
if [ $? -eq 0 ]; then
	proceed_wine
	exit 0
fi
exit 1
