#!/bin/bash
VERSION="1.0"

#todo:
#update panel
#test the script

#UTILITIES
#function for terminal colors that supports color names and nested colors
color() {
  color="$1"
  shift
  text="$@"
  case "$color" in
    # text attributes
#    end) num=0;;
    bold) num=1;;
    special) num=2;;
    italic) num=3;;
    underline|uline) num=4;;
    reverse|rev|reversed) num=7;;
    concealed) num=8;;
    strike|strikethrough) num=9;;
    # foreground colors
    black) num=30;;
    D_red) num=31;;
    D_green) num=32;;
    D_yellow) num=33;;
    D_orange) num=33;;
    D_blue) num=34;;
    D_magenta) num=35;;
    D_cyan) num=36;;
    gray) num=37;;
    D_gray) num=30;;
    red) num=31;;
    green) num=32;;
    yellow) num=33;;
    orange) num=33;;
    blue) num=34;;
    magenta) num=35;;
    cyan) num=36;;
    # background colors
    B_black) num=40;;
    BD_red) num=41;;
    BD_green) num=42;;
    BD_yellow) num=43;;
    BD_orange) num=43;;
    BD_blue) num=44;;
    BD_magenta) num=45;;
    BD_cyan) num=46;;
    BL_gray) num=47;;
    B_gray) num=5;;
    B_red) num=41;;
    B_green) num=42;;
    B_yellow) num=43;;
    B_orange) num=43;;
    B_blue) num=44;;
    B_magenta) num=45;;
    B_cyan) num=46;;
    B_white) num=47;;
#    +([0-9])) num="$color";;
#    [0-9]+) num="$color";;
    *) num="$color";;
#    *) echo "$text"
#       return;;
  esac


  mycode='\033['"$num"'m'
  text=$(echo "$text" | sed -e 's,\[0m,\[0m\\033\['"$num"'m,g')
  echo -e "$mycode$text\033[0m"
}

#display a message to stderr in bold red and exit with error status
error(){
  #bold red
  color bold `color red "$@"` 1>&2
  exit 1
}

#display a message to stderr in bold yellow
warning(){
  #bold yellow
  color bold `color yellow "$@"` 1>&2
}

#a yes or no prompt
yes_or_no(){
  prompt="$@ [y/n]"
  answer=
  while [[ -z "$answer" ]] #repeat until a valid answer is given
  do
    read -p "$prompt" -n 1 response #read 1 char
    case "$response" in
      y|Y)answer=y;;
      n|N)answer=n;;
      *)color yellow "
Enter y or n.";;
    esac
  done
  echo
}

#settings for apt-get to automatically answer yes
my_apt-get(){
  #DEBIAN _FRONTEND disables interactive mode
  DEBIAN_FRONTEND=noninteractive apt-get --yes --force-yes $@
}

#manage the errors of apt-get install
my_install(){
  my_apt-get install $@ || error "Error installing $@"
}

#manage the errors of cp
my_cp(){
  destination="$1"
  source=`basename "$1"`
  cp "$source" "$destination" || error "Error writing to $destination"
}

#manage the errors of mkdir
my_mkdir(){
  dir="$1"
  if [[ -d "$dir" ]]
  then
    warning "$dir already exists!"
  else
    mkdir "$dir" || error "Error creating $dir"
  fi
}

#display a message before rebooting
my_reboot(){
  if [[ -f auto_rerun ]]
  then
    enable_auto_rerun
  else
    echo "Press ENTER to reboot. When the pi boots up, log in and run this script again."
  fi
  color magenta "Rebooting..."
  reboot
  exit
}

#Write text to a file. Exit with an error if it fails.
write_text_to_file(){
  content="$1"
  file="$2"
  color magenta "Overwriting $file"
  echo "$content" > "$file" || error "Error writing to $file"
}

#Append to a file. Exit with an error if it fails.
append_text_to_file(){
  content="$1"
  file="$2"
  color magenta "Modifying $file"
  echo "$content" >> "$file" || error "Error writing to $file"
}

#mark an item in file
mark(){
  filename="$1"
  shift
  item="$@"
  if ! check "$filename" "$item"
  then
    append_text_to_file "$item" "$filename"
  fi
}

#find out if an item is in the file
check(){
  filename="$1"
  shift
  grep "^$@$" "$filename" > /dev/null
}

mark_as_done(){
  item="$@"
  if ! is_done "$item"
  then
    append_text_to_file "$item" "$DONE"
    num_done=`cat "$DONE" | sort -u | wc -l`
    num_skipped=`cat "$SKIP" | sort -u | wc -l`
    total=$((NUM_MODULES - num_skipped))
    color green "Step $num_done/$total ($item) done!"
  fi
}

is_done(){
  check "$DONE" "$@"
}

mark_as_skipped(){
  mark "$SKIP" "$@"
}

is_skipped(){
  check "$SKIP" "$@"
}

#change the profile so it runs this script automatically at login
enable_auto_rerun(){
  do_boot_behaviour B2 #auto log in to console
  grep "$AUTO_RERUN_COMMENT" "$PROFILE" > /dev/null || \
  append_text_to_file "$AUTO_RERUN" "$PROFILE"
}

#disable running this at login
disable_auto_rerun(){
  do_boot_behaviour B1 #console with login
  new_contents=`grep -v "$AUTO_RERUN_COMMENT" "$PROFILE"`
  write_text_to_file "$new_contents" "$PROFILE"
}

#copied from raspi-config
#used for enabling/disabling auto-login
do_boot_behaviour() {
  case "$1" in
#"B1 Console" "Text console, requiring user to login"
    B1*)systemctl set-default multi-user.target
        ln -fs /lib/systemd/system/getty@.service /etc/systemd/system/getty.target.wants/getty@tty1.service
        ;;

#"B2 Console Autologin" "Text console, automatically logged in as 'pi' user"
    B2*)systemctl set-default multi-user.target
        ln -fs /etc/systemd/system/autologin@.service /etc/systemd/system/getty.target.wants/getty@tty1.service
        ;;

#"B3 Desktop" "Desktop GUI, requiring user to login"
    B3*)systemctl set-default graphical.target
        ln -fs /lib/systemd/system/getty@.service /etc/systemd/system/getty.target.wants/getty@tty1.service
        sed /etc/lightdm/lightdm.conf -i -e "s/^autologin-user=pi/#autologin-user=/"
        disable_raspi_config_at_boot
        ;;

#"B4 Desktop Autologin" "Desktop GUI, automatically logged in as 'pi' user"
    B4*)if id -u pi > /dev/null 2>&1; then
          systemctl set-default graphical.target
          ln -fs /etc/systemd/system/autologin@.service /etc/systemd/system/getty.target.wants/getty@tty1.service
          sed /etc/lightdm/lightdm.conf -i -e "s/^#autologin-user=.*/autologin-user=pi/"
          disable_raspi_config_at_boot
        else
          error "The pi user has been removed, can't set up boot to desktop"
        fi
      ;;
      *)error "Programmer error, unrecognised boot option"
        return 1
        ;;
  esac
}

declare -A desc
declare -A depend

MODULES=
ASK=
NUM_MODULES=0

register_module(){
  module="$1"
  description="$2"
  MODULES="$MODULES $module"
  desc["$module"]="$description"
  if echo "$module" | grep '[[:space:]]' > /dev/null
  then
    warning "Warning: whitespace in module \"$module\" may cause installation to fail."
  fi

  while [[ ! -z "$3" ]]
  do
    case "$3" in
         -a|--ask)ASK="$ASK $module";;
      -d|--depend)depend["$module"]="${depend[$module]} $4"
                  shift;;
                *)warning "register_module: unrecognised option \"$3\"";;
    esac
    shift
  done
  NUM_MODULES=$((NUM_MODULES + 1))
}

SKIP=skip.txt
install_modules(){
  if [[ ! -f "$SKIP" ]]
  then
    > "$SKIP"
    for module in $ASK
    do
      yes_or_no "Install module \"$module\" (${desc[$module]})?"
      [[ "$answer" == n ]] && mark_as_skipped "$module"
    done

    for module in $MODULES
    do
      if ! is_skipped "$module"
      then
        missing=
        for dependency in ${depend[$module]}
        do
          if is_skipped "$dependency"
          then
            missing="$missing $dependency"
          fi
        done
        if [[ ! -z "$missing" ]]
        then
          warning "Dependencies missing for module $module:$missing"
          warning "Disabling $module"
          mark_as_skipped "$module"
        fi
      fi
    done
  fi

  for module in $MODULES
  do
    if is_done "$module"
    then
      color green "$module is done."
    elif is_skipped "$module"
    then
      color yellow "$module has been disabled."
    else
      color yellow "$module is NOT done."
      REBOOT=no
      "$module" && mark_as_done "$module" #run the module
      [[ "$REBOOT" == "yes" ]] && my_reboot
    fi
  done
  color bold `color green "All steps complete!"`
}

register_module setup "ask about the auto-rerun and shutdown when done functions"
setup(){
  umount /dev/mmcblk0p1 #unmount the recovery partition
  yes_or_no "Do you want the script to automatically log in and re-run itself after rebooting? If not, you will have to manually log in each time and run the script again."
  [[ "$answer" == y ]] && > auto_rerun
  yes_or_no "Do you want to shutdown after the installation is complete?"
  [[ "$answer" == y ]] && > shutdown_after
}

#Wifi
#https://www.maketecheasier.com/setup-wifi-on-raspberry-pi/
register_module wifi "set up wifi using a wifi USB device" --ask
wifi(){
  color B_blue "<======= Network devices =======>"
  ifconfig #list network devices
  color B_blue "<======= USB devices =======>"
  lsusb #list usb devices

  my_cp /etc/network/interfaces

  color B_blue "<======= Networks =======>"
  iwlist wlan0 scan #list networks

  #prompt user for network name and password
  read -p "Enter your network SSID (name): " networkSSID
  read -p "Enter your network password: " networkPassword

  yes_or_no "Is your network configured for WEP?"
  if [[ "$answer" == "y" ]]
  then
    key_mgmt=NONE
  else
    key_mgmt=WPA-PSK
  fi

  echo "key_mgmt=$key_mgmt"

  #add network to the networks file
  append_text_to_file "
network={
    ssid=\"$networkSSID\"
    psk=\"$networkPassword\"
    key_mgmt=$key_mgmt
}" /etc/wpa_supplicant/wpa_supplicant.conf

  REBOOT=yes
}

#Adafruit tft
#https://learn.adafruit.com/adafruit-2-8-pitft-capacitive-touch/easy-install

#Configure the pi to display everything on the tft screen
register_module tft_screen "configure the pi to display everything on the tft screen" --ask
tft_screen(){
  color green "Updating the pi..."
  my_apt-get update && \
  my_apt-get upgrade || \
  warning "Unable to update the pi!"

  #https://learn.adafruit.com/adafruit-pitft-28-inch-resistive-touchscreen-display-raspberry-pi/easy-install-2
  sudo apt install python3-venv git python3-pip || error "Error installing apt deps"
  python -m venv env --system-site-packages || error "Error creating venv"
  source env/bin/activate || error "Error activating venv"
  pip3 install --upgrade adafruit-python-shell click || error "Error installing python deps"
  git clone https://github.com/adafruit/Raspberry-Pi-Installer-Scripts.git || error "Error cloning repo"
  sudo -E env PATH=$PATH python3 Raspberry-Pi-Installer-Scripts/adafruit-pitft.py --display=28c --rotation=90 --install-type=mirror --reboot=no

  #If you just want a Linux text console to appear on the display, use one of the following commands:
  #For the PiTFT 2.8" Capacitive touchscreen, use the following command:
  #sudo -E env PATH=$PATH python3 adafruit-pitft.py --display=28c --rotation=90 --install-type=console

  #If you want to mirror the HDMI output to the display, known as “FrameBuffer Copy” or FBCP for short, use one of the following commands:
  #For the PiTFT 2.8" Capacitive touchscreen, use the following command:
  #sudo -E env PATH=$PATH python3 adafruit-pitft.py --display=28c --rotation=90 --install-type=mirror

  #For an interactive install, you can just run the script without any options:
  #sudo -E env PATH=$PATH python3 adafruit-pitft.py


#  #https://github.com/adafruit/Raspberry-Pi-Installer-Scripts/blob/master/adafruit-pitft.sh
#  color green "Downloading the tft setup script from adafruit.com..."
#  wget -N https://raw.githubusercontent.com/adafruit/Raspberry-Pi-Installer-Scripts/master/adafruit-pitft.sh || error "Error downloading the script."
#  chmod +x adafruit-pitft.sh || error "Error changing the permissions on the script."
#  echo "3
#3
#y
#> " | sudo ./adafruit-pitft.sh || error "Error setting up the tft screen."


#  color green "Adding adafruit to the package list..."
#  curl -SLs https://apt.adafruit.com/add-pin | bash || error "Unable to download from adafruit!"
#
#  color green "Installing the adafruit tft kernel patch..."
#  my_apt-get remove raspberrypi-bootloader
#  my_install raspberrypi-bootloader
#
#  color green "Installing the tft setup helper..."
#  my_install adafruit-pitft-helper
#
#  color green "Running the tft setup helper..."
#  echo 'y
#n
#' | adafruit-pitft-helper -t 28c || error "Error setting up /boot/config.txt"

  REBOOT=yes
}

#Touch
#enable capacitive touch on the tft screen
register_module tft_touch "set up the capacitive touchscreen" --ask --depend tft_screen
tft_touch(){
  color green "Setting up touch screen input device rules..."
  my_cp /etc/udev/rules.d/95-ft6206.rules

  color green "Starting the device..."
  modprobe ft6x06_ts || error "Error starting ft6x06_ts"

  color green "Auto-calibrating the touchscreen..."
  echo 'y' | adafruit-pitft-touch-cal || error "Error calibrating"
}

#Serial communication to ATTINY
#http://elinux.org/RPi_Serial_Connection#Connection_to_a_microcontroller_or_other_peripheral

#enable serial communication
register_module serial "enable serial communication" --ask
serial(){
  CMDLINE=/boot/firmware/cmdline.txt
  color green "Removing serial from $CMDLINE"
  new_contents=`cat $CMDLINE | sed 's/console=serial[0-9]*,[0-9]\+\s\+//g'`
  write_text_to_file "$new_contents" "$CMDLINE"

  CONFIG=/boot/firmware/config.txt
  color green "Setting enable_uart=1 in $CONFIG"
  new_contents=`grep -v enable_uart "$CONFIG"`"
enable_uart=1"

  write_text_to_file "$new_contents" "$CONFIG"

#  color green "Installing wiringPi..."
#  my_install wiringPi

  REBOOT=yes
}

register_module install_wiring_pi "install wiringPi (required for keyboard and power off script)"
install_wiring_pi(){
  color green "Installing wiringPi..."

  git clone https://github.com/WiringPi/WiringPi.git || error "Error cloning wiringPi"
  cd WiringPi || error "Error changing directory"
  ./build || error "Error building wiringPi"
  cd .. || error "Error changing directory"

#  wget https://project-downloads.drogon.net/wiringpi-latest.deb || error "Error downloading wiringPi"
#  dpkg -i wiringpi-latest.deb || error "Error installing wiringPi"

#  my_install wiringPi
}

#https://www.raspberrypi.org/forums/viewtopic.php?f=48&t=70520
register_module keyboard_daemon "set up the keyboard daemon to run on startup" --ask --depend serial --depend install_wiring_pi
keyboard_daemon(){
  cd ../KeyboardDaemon/ || error "Error changing directory"
  make || error "Error compiling the KeyboardDaemon"
  # update boot / reboot files
  my_cp /etc/init.d/KeyboardDaemon
  # do it as soon as the device is going down, both for shutdown and reboot
  update-rc.d KeyboardDaemon defaults || error "Error setting up KeyboardDaemon"
  service KeyboardDaemon start || error "Error starting KeyboardDaemon"
  sudo sed 's,^exit 0$,/etc/init.d/KeyboardDaemon \&\n exit 0,g' -i /etc/rc.local || error "Error having KeyboardDaemon start on boot"
  cd ../Install || error "Error changing directory"
}

#Power Off Script
#add a script to send a power off signal to the ATTINY when the pi shuts down
register_module power_off "add the power off script to the shutdown sequence" --ask --depend serial --depend install_wiring_pi
power_off(){
  color green "Testing serial communication using wiringPi..."
  gcc -o serial_test serial_test.c -lwiringPi || error "Error compiling the serial test!"
  color green "The backlight should change brightness (if the circuit is assembled and the ATTiny is configured)"
  ./serial_test || error "Serial test script failed!"

  color green "Compiling power off script..."
  gcc -o power_off power_off.c -lwiringPi || error "Error compiling power off script!"

  color green "Placing script in /lib/systemd/system-shutdown/"
  my_cp "/lib/systemd/system-shutdown/power_off"
}

#Desktop adjustments
#adjust settings such as icon and text size to make the Desktop more usable
register_module desktop_adjustments "adjust the desktop to display better on the tft screen" --ask
desktop_adjustments(){
  color green "Changing pcmanfm desktop settings"
  my_cp "$USER_HOME"/.config/pcmanfm/LXDE-pi/desktop-items-0.conf

  color green "Changing pcmanfm file explorer settings"
  my_cp "$USER_HOME"/.config/pcmanfm/LXDE-pi/pcmanfm.conf

  color green "Changing x server options"
  my_cp /etc/X11/xinit/xserverrc

  color green "Changing LXDE panel settings"
  my_cp "$USER_HOME"/.config/lxpanel/LXDE-pi/panels/panel

  color green "Changing LXTerminal settings"
  my_cp "$USER_HOME"/.config/lxterminal/lxterminal.conf

  color green "Creating $USER_BIN"
  my_mkdir "$USER_BIN"

  color green "Copying screenshot program to $USER_BIN"
  my_cp "$USER_BIN"/screenshot

  sudo chown -R "$SUDO_USER" "$USER_BIN"

  color green "Adding PrintScreen button trigger to run screenshot program"
  my_cp "$USER_HOME"/.config/openbox/lxde-pi-rc.xml

  color green "Changing the login message"
  my_cp /etc/motd
}

register_module autostart_terminal "autostart a terminal window when the user logs in" --ask
autostart_terminal(){
  cp /usr/share/applications/lxterminal.desktop "$USER_HOME"/.config/autostart/lxterminal.desktop || error "Error copying desktop file"
}

#Tilp and Tilem
register_module emulator "install tilp and tilem2" --ask
emulator(){
  color green "Installing tilp and tilem.."
  my_install tilp2 tilem
}

#try to connect to google.com
test_internet(){
  wget -q --tries=10 --timeout=20 --spider http://google.com
}

############################
# main part of the program #
############################

color green "Welcome to the compulator install script version $VERSION"

#if not run as root, do not run
if [ $(id -u) -ne 0 ]
then
  error "Script must be run as root. Try 'sudo $0'"
fi

USER_HOME=`eval echo ~$SUDO_USER` #the home dir of the user who ran this script with sudo
USER_BIN="$USER_HOME"/bin/ #the user's ~/bin directory
PROFILE="$USER_HOME"/.profile #the user's .profile file

#if YOUR_NAME was not set by the auto run at login
if [[ -z "$YOUR_NAME" ]]
then
  #find the absolute path to this script
  cd `dirname $0`
  me=`pwd`/`basename $0`
else
  #take YOUR_NAME to be the absolute path to this script
  cd `dirname $YOUR_NAME`
  me="$YOUR_NAME"
fi

DONE=done.txt #the file that contains which modules are done
[[ ! -f "$DONE" ]] && > "$DONE"

#the line to add to enable auto run at login
AUTO_RERUN_COMMENT="#run the compulator install script (added by the script)"
AUTO_RERUN="YOUR_NAME=$me sudo bash $me $AUTO_RERUN_COMMENT"

[[ -f auto_rerun ]] && disable_auto_rerun

#test the internet
if ! test_internet
then
  warning "Not connected to the internet. Waiting 10 seconds to try again."
  sleep 10
fi

if test_internet
then
  color green "You are connected to the internet."
else
  color yellow "You are NOT connected to the internet."
  if [[ -f "$DONE" ]] && is_done "wifi"
  then
    warning "Cannot connect to the internet and the wifi setup has been disabled."
  fi
fi

#go through each module and run it if it is not done/skipped
install_modules

[[ -f auto_rerun ]] && sudo shutdown now
