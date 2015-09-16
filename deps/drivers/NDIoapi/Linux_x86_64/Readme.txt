NDI Optotrak Application Programmer's Interface v3.05.01
----------------------------------------------------------------------

The Linux version of the OPTOTRAK Application Programmer's Interface
for Linux platforms is stored in a compressed tar file format.

The Linux version will run under Linux 2.x or higher; we have tested 
the libraries under Ubuntu Linux with kernel 2.6.24.

The contents of the compressed tar file are:

NDIoapi/ndlib/lib              - the OPTOTRAK API shared library and interface libraries.
NDIoapi/ndlib/include          - required include files for application
                                 programs.
NDIoapi/samples                - source code and makefile for sample programs.
NDIoapi/ndigital/programs	   - contains optsetup and download programs
NDIoapi/ndigital/realtime      - transputer loadable files (TLD).
NDIoapi/ndigital/rigid         - sample rigid body file used by some sample
                                 programs.
NDIoapi/ndigital/settings      - contains a sample parameter file optotrak.ini.
NDIoapi/readme.txt             - this file.


Installation
------------
To install the Optotrak API for the Linux platform:

    1. Copy the compressed tar file to the /usr/ directory.

    2. Expand the compressed tar archive:

                tar -xvf oapi_3.5.1-linux.tgz

    3. Starting from this distribution the OAPI is used as a shared library. To set it up,
 	   you will need superuser permissions. Use these commands:
 	   
       cd /usr/NDIoapi/samples
       sudo make install
 
    4. This relates only to some 3020 users running old API applications. This version of the API 
       is incompatible with older versions (pre-2005) of the transputer TLD files. New versions 
       of the TLD files can be found in the NDIoapi/ndigital/realtime subdirectory.
       
       If you are running older software requiring the older TLD files, you should create 
       a copy of /usr/NDIoapi into a new directory /usr/NDIoapiOld. Copy your old TLD files 
       into /usr/NDIoapiOld/ndigital/realtime and set the environment variable ND_DIR 
       to /usr/NDIoapiOld/ndigital/ when running old API applications requiring old TLD files.
       Set ND_DIR back to /usr/NDIoapi/ndigital when running samples or applications based on 
       a new version of API.

    5. Then you must set and export shell environment variable ND_DIR
       for each user who will be running the OPTOTRAK.  The shell environment variable
       ND_DIR should be set to the value of /usr/NDIoapi/ndigital, i.e
       the directory where you have located the subdirectories programs, realtime, settings, and rigid.
       The following procedure explains how to set and export shell environment variables 
       in the bash shell under Linux, in such a way that they are available everytime the
       user logs in:
      		a. Log in as the user
      		b. cd to the user's home directory using the command:
            		cd
      		c. If the user's default shell is the bash shell, then the user's home directory 
         	   should contain the file .bash_profile.  Edit the user's .bash_profile file using
         	   a text editor (such as vi), and insert the following three lines:

            		ND_DIR=/usr/NDIoapi/ndigital
            		PATH=$PATH:$ND_DIR/programs

      		d. Then, while still editing the .bash_profile file, edit the line that starts with
         	   the word EXPORT, adding the words ND_DIR at the end.  If this line
         	   is not already present, then add it to the .bash_profile file, after the two lines
           	   inserted in the previous step.
	 	   EXPORT (other variables could already be here) ND_DIR
         	   Save the .bash_profile file, log out and log back in as the user.

    6. Edit the following entry in optotrak.ini to the interface you are physically connected through:

		[Optotrak System]
		LinkDriver=/usr/NDIoapi/ndigital/pcilink.so  (or etherlink.so, scsilink.so, or usblink.so )
		
		If you are using etherlink.so, read more details how to set it up in 
		"Ethernet support for Linux applications" below.
		
		If you are using SCSI interface, [SCSI] section may already contain values, but these values 
		are not	required.  For Linux installations, the only value that should appear in the [SCSI]
		section of the optotrak.ini file, is:
	
		ScsiPath  = /dev/sgX
		
		Where X corresponds to the generic SCSI device assigned by the system to the 
		OPTOTRAK SCSI interface.  The generic SCSI IDs are assigned by the system starting with 
		the lowest SCSI ID, and they are assigned in order starting with 0.   Therefore you
		would enter the line:
		
		ScsiPath  = /dev/sg0
		
		in the [SCSI] section.


    7. Run 'optsetup'. If everything was set up correctly then optsetup will list on the screen
       the various OPTOTRAK components which it found and it will create the system.nif and
       standard.cam files in the directory /usr/NDIoapi/ndigital/realtime.  These files will have their
       attributes set so that only the user who created them, can edit or delete them.  However,
       for proper operation is it required that all other users who use the OPTOTRAK system, be
       able to read and write to these files, therefore you now must log is as root and change
       the permissions on these files to give everyone read and write privileges on these files.
       Use the following command:

       		chmod 666 system.nif
       		chmod 666 standard.cam

       If everything was setup correctly, then you should be able to power up the SCU and log in
       as a regular user and run the command 'optsetup'.  The SCU should beep, and a short 
       message should appear on the screen indicating that an SCU was found (and a camera if 
       one is connected), and a system.nif file should be created or updated in thedirectory
       /usr/NDIoapi/ndigital/realtime.


Compiling the sample programs
-----------------------------
Updated versions of twenty-one sample programs are in the NDIoapi/samples
directory. To make the sample programs, change to the NDIoapi/samples
subdirectory and run make. This will make all of the sample programs. If you
only want to make a specific sample program, then append the program name to
the make command. For example, to make sample program #1 use:

    make sample1


Compiling application programs
------------------------------
You must include the appropriate header files in the include subdirectory
ndlib/include in your application program source code, and link your program
with -loapi. See Makefile for more details.

Ethernet support for Linux applications
--------------------------------------------------------
You may choose to communicate with the Optotrak System using an ethernet
connection.  To access the Ethernet interface with your application programs,
add the following line to the [OPTOTRAK System] section of your OPTOTRAK.INI
file (with the appropriate path):

	[OPTOTRAK System]
	LinkDriver = /usr/NDIoapi/ndigital/etherlink.so

and add the following section (with ID values appropriate to your system) to
your OPTOTRAK.INI file:

	[Ethernet Options]
	Server Address=172.16.1.104
	Control Port=9999
	Data Port=10000
	Data Timeout=30000
	Reset Delay=5000

The Parameter Data Timeout is the number of milliseconds allowed for
Ethernet commands to complete.  Commands taking longer are assumed to be
"stuck" and so will be aborted.

Troubleshooting Tips
====================

1. If you type 'optsetup' and the shell immediately returns the prompt without producing
   any other output, and there was no beep from the SCU, then:
      a. Check that the SCU is powered up
      b. Check all the link and communications cables are connected
      c. check that the ND_DIR shell variable is set and exported in the
         user's .bash_profile file (or equivalent file if using another shell).

2. If you type 'optsetup' and you get a message that starts with 'Errors Detected', but 
   the SCU did not beep, then you may have entered the wrong device driver name in the file
   /usr/NDIoapi/ndigital/settings/optotrak.ini. 
   
   Edit the 
   /usr/NDIoapi/ndigital/settings/optotrak.ini file, enter

   LinkDriver=/usr/NDIoapi/ndigital/pcilink.so and re-run optsetup.
   
   Otherwise if you are using the usb link enter

   LinkDriver=LinkDriver=/usr/NDIoapi/ndigital/usblink.so and re-run.

   If you obtain the following error:
   
	libusb couldn't open USB device /dev/bus/usb/004/005: Permission denied.
	libusb requires write access to USB device nodes.
	libusb failed to find the Optotrak USB device.  Are you sure you have set the udev permissions correctly?
	PING BAD

   then you have to set the correct read and write settings to all usb devices on that port, in this case:
	sudo chmod 777 /dev/bus/usb/004/005

	If you want to make this solution permanent, then you have to create a custom rule in /etc/udev
	On Ubuntu 12.04 LTS, I have tested that the following solution works:

	1) Create the file 12-ndi.rules inside the folder /etc/udev/rules.d/
	   (you need root privileges)
	    
	    $> sudo nano /etc/udev/rules.d/12-ndi.rules

	2) Edit the file to have this content:

	SUBSYSTEM=="usb", ATTR{idVendor}=="0403", ATTR{idProduct}=="da77", GROUP="plugdev", MODE="0666"
	
	3) If reloading the rule with:
	
	    $> sudo udevadm control --reload-rules
	
	still does not allow to communicate with the Optotrak, then reboot.
	3) Reboot and then try to run a sample program on the Optotrak:

	    $> download

3. If you type 'optsetup' and the SCU beeps, there are no error messages, but the output 
   on the screen indicates that no SCU and no cameras were found, then it's possible that
   the files 
      /usr/NDIoapi/ndigital/realtime/system.nif and 
      /usr/NDIoapi/ndigital/realtime/standard.cam 
   have the wrong permissions and this user is not allowed to write to them.
   Log as the root and change permissions of the system.nif and standard.cam files to 
   allow all uses to read and write to them.  Use the following command
      chmod 666 system.nif
      chmod 666 standard.cam

Also if you have problems, create a logs/opto.err file in /usr/NDIoapi/realtime/ and set read and write permissions to it:
      chmod 666 /usr/NDIoapi/ndigital/realtime/logs/opto.err
 

4.  If you have problems with the camera files, remember that on Linux, camera files have to be lower case.
