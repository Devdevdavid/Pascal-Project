#!/bin/bash
# =================================
# Date		: 15 June 2020
# Author	: David DEVANT & Bastian BOUCHARDON
# File		: achdr_updater.sh
# Version	: 1.0
# Desc.		: Updater for ACHDR
# 			  Application
# =================================

CONFIG_FILE_NAME=achdr_soft.config
INSTALL_PATH=/home/pi/Scripts
SERVICE_PATH=/etc/systemd/system

# Pinout (Wiring Pi notation) for status led
PIN_RED_LED=27
PIN_GREEN_LED=26

# Get the path of the current file
# https://stackoverflow.com/a/4774063
USBKEY_PACK_PATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

# Build paths
INSTALLED_CONFIG_FILE_PATH=$INSTALL_PATH/$CONFIG_FILE_NAME
USBKEY_CONFIG_FILE_PATH=$USBKEY_PACK_PATH/$CONFIG_FILE_NAME

# Macro

is_arg_a_number() {
	regex='^[0-9]+$'
	if [[ $1 =~ $regex ]]; then
		return 1
	else
		return 0
	fi
}

# Parse a simple configuration file
# See https://stackoverflow.com/a/20815951
parse_config_file() {
	shopt -s extglob
	configfile=$1 # set the actual path name of your (DOS or Unix) config file
	tr -d '\r' < $configfile > $configfile.unix
	while IFS='= ' read -r lhs rhs
	do
	    if [[ ! $lhs =~ ^\ *# && -n $lhs ]]; then
	        rhs="${rhs%%\#*}"    # Del in line right comments
	        rhs="${rhs%%*( )}"   # Del trailing spaces
	        rhs="${rhs%\"*}"     # Del opening string quotes
	        rhs="${rhs#\"*}"     # Del closing string quotes
	        export $lhs="$rhs"
	    fi
	done < $configfile.unix

	# Remove tmp file
	rm $configfile.unix

	# Tell no error
	return 0
}

# Read the config file given in params and
# Check the values integrity
read_config() {
	CONFIG_PATH=$1

	# Check if config file exist on the system
	if [[ ! -f "$CONFIG_PATH" ]]; then
		echo "[W] $CONFIG_PATH : no such file"
		return 1
	fi

	# Parse the config file
	parse_config_file $CONFIG_PATH
	if [[ $? -ne 0 ]]; then
		echo "[W] Invalid config data in the installed file"
		return 1
	fi

	# Check params
	is_arg_a_number $VERSION_MAJOR
	if [[ $? -ne 1 ]]; then
		echo "[E] VERSION_MAJOR is not a number: $VERSION_MAJOR"
		return 1
	fi

	is_arg_a_number $VERSION_MINOR
	if [[ $? -ne 1 ]]; then
		echo "[E] VERSION_MINOR is not a number: $VERSION_MINOR"
		return 1
	fi

	return 0
}

# Compare 2 versions composed of a major and
# a minor version number
compare_version() {
	V_MAJOR_CUR=$1
	V_MINOR_CUR=$2
	V_MAJOR_AVAIL=$3
	V_MINOR_AVAIL=$4

	if [[ "$V_MAJOR_AVAIL" -gt "$V_MAJOR_CUR" ]]; then
		return 1		# Update needed
	elif [[ "$V_MAJOR_AVAIL" -eq "$V_MAJOR_CUR" ]]; then
		if [[ "$V_MINOR_AVAIL" -gt "$V_MINOR_CUR" ]]; then
			return 1 	# Update needed
		elif [[ "$V_MINOR_AVAIL" -eq "$V_MINOR_CUR" ]]; then
			return 0	# Up to date
		fi
	fi

	return 2			# Available is depreciated
}

check_requirement_program() {
	PROG=$1
	NAME=$2
	OPTIONAL=$3

	command -v $PROG > /dev/null
	if [[ $? -ne 0 ]]; then
		# Define de gravity of the message
		if [[ "$OPTIONAL" == "true" ]]; then GRAVITY=W; else GRAVITY=E; fi

		echo "[$GRAVITY] \"$NAME\" is not installed on the system"
		echo "[I] Use \"sudo apt install $NAME\" to resolve this"

		# Return an error only if program is not an option
		[[ "$OPTIONAL" == "true" ]] || return 1
	fi
	return 0
}

check_requirements() {
	# Test if programs are installed
	check_requirement_program python python || return 1
	check_requirement_program gpio wiringpi || return 1
	check_requirement_program unclutter unclutter || return 1
	check_requirement_program omxplayer.bin omxplayer || return 1
	check_requirement_program soffice libreoffice true || return 1
	check_requirement_program feh feh || return 1

	return 0
}

# Copy a file with checks and log messages
# Also set file permission
copy_file() {
	SRC_FILE_PATH=$1
	DEST_FILE_PATH=$2
	DEST_FILE_PERM=$3

	FILE_NAME=$(basename $SRC_FILE_PATH)

	cp $SRC_FILE_PATH $DEST_FILE_PATH
	if [[ $? -ne 0 ]]; then
		echo "[E] Failed to copy $FILE_NAME"
		return 1
	else
		echo "[I] $FILE_NAME copied"
	fi

	chmod $DEST_FILE_PERM $DEST_FILE_PATH/$FILE_NAME
	if [[ $? -ne 0 ]]; then
		echo "[E] Failed to set permission of $FILE_NAME to $DEST_FILE_PERM"
		return 1
	else
		echo "[I] Permission of $FILE_NAME set"
	fi

	return 0
}

# Install a service file on the system
install_service() {
	SERVICE_NAME=$1
	NO_START=$2 # start or nostart

	# Copy the service
	copy_file $USBKEY_PACK_PATH/$SERVICE_NAME.service $SERVICE_PATH/ 644 || return 1

	# Enable the service
	systemctl --quiet enable $SERVICE_NAME 2>&1 > /dev/null
	if [[ $? -ne 0 ]]; then
		echo "[E] Failed to enable $SERVICE_NAME.service"
		return 1
	fi

	# Start the service
	if [[ "$NO_START" != "nostart" ]]; then
		systemctl --quiet start $SERVICE_NAME 2>&1 > /dev/null
		if [[ $? -ne 0 ]]; then
			echo "[E] Failed to start $SERVICE_NAME"
			return 1
		fi
	fi

	echo "[I] $SERVICE_NAME.service installed"

	return 0
}

# Install the pack from the usb key
install_pack() {
	# =====================
	# UPDATER
	# =====================

	# We copy the updater to be able to uninstall later
	copy_file $USBKEY_PACK_PATH/achdr_launcher.sh $INSTALL_PATH/ 755 || return 1
	install_service ACHDRLauncher nostart || return 1

	# We copy the updater to be able to uninstall later
	copy_file $USBKEY_PACK_PATH/achdr_updater.sh $INSTALL_PATH/ 755 || return 1

	# =====================
	# MAIN APPLICATION
	# =====================

	# Copy the main application
	copy_file $USBKEY_PACK_PATH/achdr_main_app.py $INSTALL_PATH/ 644 || return 1
	install_service ACHDRMainApp || return 1

	# =====================
	# SHUTDOWN BUTTON
	# =====================

	copy_file $USBKEY_PACK_PATH/shutdown_button.sh $INSTALL_PATH/ 755 || return 1
	install_service ACHDRShutdownButton || return 1

	# =====================
	# HIDE CURSOR
	# =====================

	install_service ACHDRHideCursor || return 1

	# =====================
	# CONFIG FILE
	# =====================

	# Update is doing well, copy the config file
	# to take the update into account
	copy_file $USBKEY_PACK_PATH/$CONFIG_FILE_NAME $INSTALL_PATH/ 644 || return 1

	return 0
}

# Remove a file with checks and log messages
remove_file() {
	FILE_PATH=$1

	FILE_NAME=$(basename $FILE_PATH)

	if [[ -f $FILE_PATH ]]; then
		rm $FILE_PATH
		if [[ $? -ne 0 ]]; then
			echo "[E] Failed to remove $FILE_NAME"
			return 1
		else
			echo "[I] $FILE_NAME removed"
		fi
	else
		echo "[I] No $FILE_NAME to remove"
	fi

	return 0
}

uninstall_service() {
	SERVICE_NAME=$1

	# Stop the service
	systemctl --quiet stop $SERVICE_NAME 2>&1 > /dev/null
	if [[ $? -ne 0 ]]; then
		echo "[W] Failed to stop $SERVICE_NAME, continuing..."
	fi

	# Disable the service
	systemctl --quiet disable $SERVICE_NAME 2>&1 > /dev/null
	if [[ $? -ne 0 ]]; then
		echo "[W] Failed to disable $SERVICE_NAME.service, continuing..."
	fi

	# Remove the service
	remove_file $SERVICE_PATH/$SERVICE_NAME.service || return 1

	echo "[I] $SERVICE_NAME.service removed"
	return 0
}

uninstall_pack() {
	# =====================
	# MAIN APPLICATION
	# =====================

	uninstall_service ACHDRMainApp || return 1
	remove_file $INSTALL_PATH/achdr_main_app.py || return 1

	# =====================
	# SHUTDOWN BUTTON
	# =====================

	uninstall_service ACHDRShutdownButton || return 1
	remove_file $INSTALL_PATH/shutdown_button.sh || return 1

	# =====================
	# HIDE CURSOR
	# =====================

	uninstall_service ACHDRHideCursor || return 1

	# =====================
	# CONFIG FILE
	# =====================

	# Remove the configuration file
	remove_file $INSTALL_PATH/$CONFIG_FILE_NAME || return 1

	# =====================
	# UPDATER
	# =====================

	# We do not want to remove the launcher to be
	# able to install futur version with USB

	# Remove the updater
	remove_file $INSTALL_PATH/achdr_updater.sh || return 1

	return 0
}

# =====================
# MAIN
# =====================

# Configure LED pins
gpio mode $PIN_RED_LED out
gpio mode $PIN_GREEN_LED out

# Check for root privileges
if [[ "$EUID" -ne 0 ]]; then
	echo "[I] Please run as root"
  	exit 1
fi

# Move to the updater script
cd $USBKEY_PACK_PATH

# Create the install directory if not existing
if [[ ! -d $INSTALL_PATH ]]; then
	mkdir -p $INSTALL_PATH
fi

# Default params value
CMD="install"

# Check script params
# See https://linuxhint.com/bash_getopts_example/
while getopts "iu" option; do
	case ${option} in
	i )
		CMD="install"
		;;
	u )
		CMD="uninstall"
		;;
	\? )
		echo "usage: achdr_updater.sh [-ui]"
		exit 1
		;;
	esac
done

# =====================
# INSTALLATION
# =====================
if [[ "$CMD" == "install" ]]; then

	# Read usbkey file
	read_config $USBKEY_CONFIG_FILE_PATH
	if [[ $? -ne 0 ]]; then
		echo "[E] USBKEY config file is corrupted"
		exit 0
	else
		# Save the value
		USBKEY_VMAJOR=$VERSION_MAJOR
		USBKEY_VMINOR=$VERSION_MINOR
	fi

	# Read installed file
	read_config $INSTALLED_CONFIG_FILE_PATH
	if [[ $? -ne 0 ]]; then
		echo "[W] Unable to read installed config file due to previous error"

		# Config read failed, continue update
		INSTALLED_VMAJOR=0
		INSTALLED_VMINOR=0
	else
		# Save the value
		INSTALLED_VMAJOR=$VERSION_MAJOR
		INSTALLED_VMINOR=$VERSION_MINOR
	fi

	# Test the version
	compare_version $INSTALLED_VMAJOR $INSTALLED_VMINOR $USBKEY_VMAJOR $USBKEY_VMINOR
	ret=$?
	if [[ "$ret" -eq 2 ]]; then
		echo "[E] The USB key version is depreciated !"
		echo "[E] Installed: v$INSTALLED_VMAJOR.$INSTALLED_VMINOR - USB key: v$USBKEY_VMAJOR.$USBKEY_VMINOR"
		exit 1
	elif [[ "$ret" -eq 0 ]]; then
		echo "[I] Installed version is up to date : v$INSTALLED_VMAJOR.$INSTALLED_VMINOR"
		exit 0
	elif [[ "$ret" -eq 1 ]]; then
		echo "[I] Update available : v$INSTALLED_VMAJOR.$INSTALLED_VMINOR -> v$USBKEY_VMAJOR.$USBKEY_VMINOR"
	else
		echo "[E] compare_version() failed"
		exit 1
	fi

	check_requirements
	if [[ $? -ne 0 ]]; then
		echo "[E] --- Update failed ---"
		exit 1
	fi

	# First uninstall current version if any with the updater
	# located on the system
	if [[ -f $INSTALL_PATH/achdr_updater.sh ]]; then
		echo "[I] Using system updater to uninstall current version..."
		bash $INSTALL_PATH/achdr_updater.sh -u
	else
		echo "[I] No updater found to uninstall current version."
	fi

	# Then install the new version with this current updater
	# located in the USB key
  # turn on led status in amber
  gpio write $PIN_RED_LED 1
  gpio write $PIN_GREEN_LED 1
	echo "[I] Installing USB key version..."
	install_pack
	if [[ $? -ne 0 ]]; then
		echo "[E] --- Update failed ---"
		exit 1
	fi

	echo "[I] --- Update done ! ---"


# =====================
# UNINSTALLATION
# =====================
elif [[ "$CMD" == "uninstall" ]]; then

	uninstall_pack
	if [[ $? -ne 0 ]]; then
		echo "[E] --- Uninstallation failed ---"
		exit 1
	fi

	echo "[I] --- Uninstallation done ! ---"

# =====================
# OTHERS
# =====================
else
	exit 1
fi

exit 0
