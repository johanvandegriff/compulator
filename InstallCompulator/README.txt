You will need:
	Raspberry Pi 0
	Micro SD card
	Micro SD card reader
	HDMI to mini HDMI cable
	Screen with HDMI port
	micro USB hub
	USB keyboard and mouse
	wifi dongle

	pi tft shield
	attiny84
	3V latching relay
	5x10 button matrix with diodes


I. Download NOOBS and Prepare SD Card
	A. Download the "Offline and network install" version of NOOBS from
		https://downloads.raspberrypi.org/NOOBS_latest
	B. Move the NOOBS zip file to its own folder and extract the files from it.
	C. Format an SD card that is 4GB or larger as FAT.
	D. Copy the extracted files onto the SD card.
	E. Copy the install script folder (install_compulator) to the root directory of the SD card.

	Downloads page: https://www.raspberrypi.org/downloads/noobs/
	Reference Page: https://www.raspberrypi.org/documentation/installation/noobs.md

III. Set Up Pi Hardware
	A. Connect a screen to the HDMI port.
	B. Connect a USB hub, keyboard, mouse, and Wifi dongle.
	C. Plug in the SD card.

	Wifi Dongle: https://www.amazon.com/Edimax-EW-7811Un-150Mbps-Raspberry-Supports/dp/B003MTTJOY/

IV. Install Raspbian
	A. Apply power to the pi and wait for it to load.
	B. Select your Language and Keyboard.
	C. Check the box next to "Raspbian".
	D. Click "Install", then "Yes".
	E. The installation will take about 15 minutes.
	F. When it is done, click "OK" and the pi will reboot into Raspbian

V. Run the Install Script
	A. Open a terminal window.
	B. Run the following commands:
		mkdir tmp
		sudo mount /dev/mmcblk0p1 tmp
		cp -r tmp/install_compulator/ .
		chmod +x install_compulator/install.sh
		sudo install_compulator/install.sh
	C. The script will:
		1. Ask which components of the installation to skip
		2. Ask for wifi network information
		3. Install without further user interaction (unless there is an error)
		4. Done!




