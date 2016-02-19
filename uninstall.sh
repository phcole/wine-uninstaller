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
		echo "SH: CrossOver link missing."
		return
	fi
	tmp=$(cat $tmp | grep 'exec ')
	local Bottle=${tmp#*--bottle \"}
	Bottle=${Bottle%%\" *}
	local link_file=${tmp#*--start \"}
	link_file=${link_file%%\" *}

	tmp=${link_file/C:/"$HOME/.cxoffice/$Bottle/drive_c"}
	if [ ! -f "$tmp" ]; then
		echo "SH: link file missing."
		return
	fi

	tmp=$(grep '\"Updater\" = ' "$HOME/.cxoffice/$Bottle/cxbottle.conf")
	tmp=${tmp#*\"Updater\" = }
	if [ $tmp != '""' ]; then
		echo "SH: we can't modify a public bottle."
		return
	fi
	echo "SH: $tmp"

	echo "SH: cxoffice bottle: \"$Bottle\""
	echo "SH: shell link file: \"$link_file\""

	/opt/cxoffice/bin/wine --bottle="$Bottle" --verbose --wait-children -- "$BASE_DIR/$UNINSTALLER_EXE" "$link_file"
	echo "SH: proceed end"
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
	echo "SH: wine prefix:     $prefix"
	local wine_cmd=${tmp#Exec=env WINEPREFIX=\"*\" }
	wine_cmd=${wine_cmd%% *}
	echo "SH: wine command:    $wine_cmd"
	local link_file=${tmp#*/dosdevices/}
	link_file=$(echo -e $link_file)
	link_file=${link_file//\\ / }
	echo "SH: shell link file: $link_file"

	if [ ! -d "$prefix" ]; then
		echo "SH: invalid wine prefix."
		return
	fi

	echo "SH: Prefix:    $prefix"
	echo "SH: wine cmd:  $wine_cmd"
	echo "SH: link file: $link_file"

	env WINEPREFIX="$prefix" $wine_cmd "$BASE_DIR/$UNINSTALLER_EXE" "$link_file"
	echo "SH: proceed end"
}

DESKTOP_FILE=$1
if [ -z "$DESKTOP_FILE" ]; then
	echo "SH: no desktop given."
	exit 1
fi

if [ ! -f "$DESKTOP_FILE" ]; then
	echo "SH: desktop file do not exists."
	exit 1
fi

echo "SH: $DESKTOP_FILE"

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
