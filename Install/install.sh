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
      y)answer=y;;
      Y)answer=y;;
      n)answer=n;;
      N)answer=n;;
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
    num_done=`cat "$DONE" | sort -u | wc -l`
    num_skipped=`cat "$SKIP" | sort -u | wc -l`
    total=$((NUM_MODULES - num_skipped))
    color green "Step $num_done/$total ($item) done!"
  fi
  mark "$DONE" "$item"
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
  if command -v systemctl > /dev/null && systemctl | grep -q '\-\.mount'; then
    SYSTEMD=1
  elif [ -f /etc/init.d/cron ] && [ ! -h /etc/init.d/cron ]; then
    SYSTEMD=0
  else
    error "Unrecognised init system"
  fi


  BOOTOPT="$1"
  if [[ -z "$BOOTOPT" ]]
  then
    BOOTOPT=$(whiptail --title "Raspberry Pi Software Configuration Tool (raspi-config)" --menu "Boot Options" $WT_HEIGHT $WT_WIDTH $WT_MENU_HEIGHT \
      "B1 Console" "Text console, requiring user to login" \
      "B2 Console Autologin" "Text console, automatically logged in as 'pi' user" \
      "B3 Desktop" "Desktop GUI, requiring user to login" \
      "B4 Desktop Autologin" "Desktop GUI, automatically logged in as 'pi' user" \
      3>&1 1>&2 2>&3)
  fi
#  if [ $? -eq 0 ]; then
    case "$BOOTOPT" in
      B1*) #"B1 Console" "Text console, requiring user to login"
        if [ $SYSTEMD -eq 1 ]; then
          systemctl set-default multi-user.target
          ln -fs /lib/systemd/system/getty@.service /etc/systemd/system/getty.target.wants/getty@tty1.service
        else
          [ -e /etc/init.d/lightdm ] && update-rc.d lightdm disable 2
          sed /etc/inittab -i -e "s/1:2345:respawn:\/bin\/login -f pi tty1 <\/dev\/tty1 >\/dev\/tty1 2>&1/1:2345:respawn:\/sbin\/getty --noclear 38400 tty1/"
        fi
        ;;
      B2*) #"B2 Console Autologin" "Text console, automatically logged in as 'pi' user"
        if [ $SYSTEMD -eq 1 ]; then
          systemctl set-default multi-user.target
          ln -fs /etc/systemd/system/autologin@.service /etc/systemd/system/getty.target.wants/getty@tty1.service
        else
          [ -e /etc/init.d/lightdm ] && update-rc.d lightdm disable 2
          sed /etc/inittab -i -e "s/1:2345:respawn:\/sbin\/getty --noclear 38400 tty1/1:2345:respawn:\/bin\/login -f pi tty1 <\/dev\/tty1 >\/dev\/tty1 2>&1/"
        fi
        ;;
      B3*) #"B3 Desktop" "Desktop GUI, requiring user to login"
        if [ -e /etc/init.d/lightdm ]; then
          if [ $SYSTEMD -eq 1 ]; then
            systemctl set-default graphical.target
            ln -fs /lib/systemd/system/getty@.service /etc/systemd/system/getty.target.wants/getty@tty1.service
          else
            update-rc.d lightdm enable 2
          fi
          sed /etc/lightdm/lightdm.conf -i -e "s/^autologin-user=pi/#autologin-user=/"
          disable_raspi_config_at_boot
        else
          whiptail --msgbox "Do sudo apt-get install lightdm to allow configuration of boot to desktop" 20 60 2
          return 1
        fi
        ;;
      B4*) #"B4 Desktop Autologin" "Desktop GUI, automatically logged in as 'pi' user"
        if [ -e /etc/init.d/lightdm ]; then
          if id -u pi > /dev/null 2>&1; then
            if [ $SYSTEMD -eq 1 ]; then
              systemctl set-default graphical.target
              ln -fs /etc/systemd/system/autologin@.service /etc/systemd/system/getty.target.wants/getty@tty1.service
            else
              update-rc.d lightdm enable 2
            fi
            sed /etc/lightdm/lightdm.conf -i -e "s/^#autologin-user=.*/autologin-user=pi/"
            disable_raspi_config_at_boot
          else
            whiptail --msgbox "The pi user has been removed, can't set up boot to desktop" 20 60 2
          fi
        else
          whiptail --msgbox "Do sudo apt-get install lightdm to allow configuration of boot to desktop" 20 60 2
          return 1
        fi
        ;;
      *)
        whiptail --msgbox "Programmer error, unrecognised boot option" 20 60 2
        return 1
        ;;
    esac
  #fi
}

MODULES=""
ASK=""
NUM_MODULES=0

register_module(){
  MODULES="$MODULES $1"
  [[ "$2" == --ask ]] && ASK="$ASK $1"
  NUM_MODULES=$((NUM_MODULES + 1))
}

SKIP=skip.txt
install_modules(){
  if [[ ! -f "$SKIP" ]]
  then
    > "$SKIP"
    for module in $ASK
    do
      yes_or_no "Install module \"$module\"?"
      [[ "$answer" == n ]] && mark_as_skipped "$module"
    done
  fi

  for module in $MODULES
  do
    if is_done "$module"
    then
      color green "$module is done."
    elif ! is_skipped "$module"
    then
      color yellow "$module is NOT done."
      REBOOT=no
      "$module" && mark_as_done "$module"
      [[ "$REBOOT" == "yes" ]] && my_reboot
    fi
  done
  color bold `color green "All steps complete!"`
}

#ask which items to install
register_module setup
setup(){
  umount /dev/mmcblk0p1 #unmount the recovery partition
  yes_or_no "Do you want the script to automatically log in and re-run itself after rebooting? If not, you will have to manually log in each time and run the script again."
  [[ "$answer" == y ]] && > auto_rerun
}

#Wifi
#https://www.maketecheasier.com/setup-wifi-on-raspberry-pi/
register_module wifi --ask
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
register_module tft_screen --ask
tft_screen(){
  color green "Updating the pi..."
  my_apt-get update && \
  my_apt-get upgrade && \
  rpi-update || \
  warning "Unable to update the pi!"

  color green "Adding adafruit to the package list..."
  curl -SLs https://apt.adafruit.com/add-pin | bash || error "Unable to download from adafruit!"

  color green "Installing the adafruit tft kernel patch..."
  my_apt-get remove raspberrypi-bootloader
  my_install raspberrypi-bootloader

  color green "Installing the tft setup helper..."
  my_install adafruit-pitft-helper

  color green "Running the tft setup helper..."
  echo 'y
n
' | adafruit-pitft-helper -t 28c || error "Error setting up /boot/config.txt"

  REBOOT=yes
}

#Touch
#enable capacitive touch on the tft screen
register_module tft_touch --ask
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
register_module serial --ask
serial(){
  CMDLINE=/boot/cmdline.txt
  color green "Removing serial from $CMDLINE"
  new_contents=`cat $CMDLINE | sed 's/console=serial[0-9]*,[0-9]\+\s\+//g'`
  write_text_to_file "$new_contents" "$CMDLINE"

  color green "Setting enable_uart=1 in $CONFIG"
  new_contents=`grep -v enable_uart "$CONFIG"`"
enable_uart=1"

  write_text_to_file "$new_contents" "$CONFIG"

  REBOOT=yes
}

#https://www.raspberrypi.org/forums/viewtopic.php?f=48&t=70520
register_module keyboard_daemon --ask
keyboard_daemon(){
  cd ../KeyboardDaemon/ || error "Error changing directory"
  make || error "Error compiling the KeyboardDaemon"
  # update boot / reboot files
  my_cp /etc/init.d/KeyboardDaemon
  # do it as soon as the device is going down, both for shutdown and reboot
  update-rc.d /etc/init.d/KeyboardDaemon defaults || error "Error setting up KeyboardDaemon"
  service KeyboardDaemon start || error "Error starting KeyboardDaemon"
  cd ../Install || error "Error changing directory"
}

#Power Off Script
#add a script to send a power off signal to the ATTINY when the pi shuts down
register_module power_off --ask
power_off(){
  color green "Installing wiringPi..."
  my_install wiringPi

  color green "Testing serial communication using wiringPi..."
  gcc -o serial_test serial_test.c -lwiringPi || error "Error compiling the serial test!"
  color green "The backlight should change brightness (if the circuit is assembled and the ATTiny is configured)"
  ./serial_test || error "Serial test script failed!"

  color green "Compiling power off script..."
  gcc -o power_off power_off.c -lwiringPi || error "Error compiling power off script!"

  color green "Placing script in /lib/systemd/system-shutdown/"
  my_cp "$POWER_OFF"
}

#Desktop adjustments
#adjust settings such as icon and text size to make the Desktop more usable
register_module desktop_adjustments --ask
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

  color green "Adding PrintScreen button trigger to run screenshot program"
  my_cp "$USER_HOME"/.config/openbox/lxde-pi-rc.xml

  color green "Changing the login message"
  my_cp /etc/motd
}

#Tilp and Tilem
register_module emulator --ask
emulator(){
  color green "Installing tilp and tilem.."
  my_install tilp2 tilem
}

#register_module cleanup
#cleanup(){
#  color green "The boot options selector is available by running 'sudo raspi-config'"
#}

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
AUTO_RERUN="YOUR_NAME=$me sudo $me $AUTO_RERUN_COMMENT"

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
