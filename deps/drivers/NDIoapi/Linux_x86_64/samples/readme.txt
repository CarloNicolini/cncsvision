================================================================================
NDI - Measurement You Can Trust
================================================================================

Optotrak Sample Programs

sample1      - Get Optotrak status.
sample2      - Retrieve real-time 3D data.
sample3      - Retrieve real-time Raw data and spool 3D data to file.
sample4      - Spool Raw data to memory.
sample5      - Simultaneous real-time data and spool data to file.
sample6      - Simultaneous real-time data and spool data to memory.
sample7      - Use marker to start and stop spooling data to file.
sample8      - Spool raw data to file and later convert data to 3D.
sample9      - Retrieve real-time rigid body data.
sample10     - Retrieve real-time rigid body and 3D data.
sample11     - Display real-time data using a rigid body's coordinate sytem.
sample12     - Retrieve real-time ODAU data.
sample13     - Spool 3D and ODAU data to file.
sample14     - Spool Raw and ODAU data to file and convert to 3D and voltage.
sample15     - Spool data to file, read data file and write new data file.
sample16     - Retrieve real-time Full Raw data as a secondary host.
sample17     - Use marker to start and stop spooling data to file.
sample18     - Change coordinate system to newly measured positions.
sample19     - Spool Raw data to file, read data and convert to 3D and 6D.
sample20     - Generate system configuration externally, then internally.
sample21     - Obtain camera parameter info and change camera parameter set.
sample22     - Set up collection from file.
certus_sample   - Retrieve strober configuration and display 3D data.
certus_sample1  - Retrieve strober configuration and display 3D data.
certus_sample2  - Retrieve 3D data until strober configuration changes.
certus_sample3  - Get number of strober porters, and value of 'PORT' property.
certus_sample4  - Set device properties and verify changes.
certus_sample11 - Make all device VLEDs (except tracking VLED) blink.
certus_sample12 - Make all device beepers beep for 1 second.
certus_sample13 - Retrieve 3D data until switch is pressed.
certus_sample14 - Load rigid body from SROM and display 6D data.
certus_sample15 - Allow user to change name for each strober.
certus_sample16 - Retrieve real-time Centroid and 3D data from strobers.
certus_sample17 - Check device status until number of strobers changes.
certus_sample18 - Retrieve 3D data, no fire markers for first 4 data requests.
certus_sample19 - Retrieve 3D and ODAU data continously.
certus_sample20 - Retrieve 3D frames from Wireless strobers in wireless mode.

--------------------------------------------------------------------------------

sample1
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Request/receive/display the current Optotrak System status.
	- Disconnect from the Optotrak System.


sample2
	- Initiate communications with the Optotrak System.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display 10 frames of real-time 3D data.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


sample3
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display 10 frames of real-time Full Raw data.
	- Initialize data file for spooling 3D data.
	- Collect and spool 2 seconds of 3D data to file.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


sample4
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Initialize a memory block for spooling Raw Optotrak data.
	- Collect and spool 2 seconds of Raw data to memory.
	- De-activate the markers.
	- Display the 100 frames of data stored in the memory block.
	- Disconnect from the Optotrak System.


sample5
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Initialize data file for spooling Raw Optotrak data.
	- Start spooling data to file.
	- Request/receive/display real-time 3D data while at the
	  same time writing spooled Raw data to file for 4 seconds.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


sample6
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Initialize a memory block for spooling 3D data.
	- Start spooling data to memory.
	- Request/receive/display real-time 3D data while at the
	  same time writing spooled 3D data to file for 5 seconds.
	- De-activate the markers.
	- Display the 250 frames of data stored in the memory block.
	- Disconnect from the Optotrak System.


sample7
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Initialize data file for spooling 3D data.
	- Request/receive real-time 3D data until Marker_01 appears.
	- Start spooling data to file.
	- Request/receive/display real-time 3D data while at the
	  same time writing spooled 3D data to file.
	- Stop spooling to file after 100 seconds of data, or when
	  Marker_01 goes out of view.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


sample8
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Initialize data file for spooling Raw Optotrak data.
	- Collect and spool 5 seconds of Raw data to file.
	- De-activate the markers.
	- Convert the data in the Raw data file to a 3D data file.
	- Disconnect from the Optotrak System.


sample9
	- Set processing flags to perform data conversions on the host computer.
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Add rigid body to the tracking list using a rigid body file.
	- Activate the markers.
	- Request/receive/display 10 frames of real-time rigid body transformations.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


sample10
	- Initiate communications with the Optotrak System.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Add rigid body to the tracking list using a rigid body file.
	- Change the settings for the rigid body.
	- Request/receive/display 10 frames of real-time rigid body
	  transformations and associated 3D data.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


sample11
	- Initiate communications with the Optotrak System.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Add rigid body to the tracking list using an array of 3D points.
	- Add rigid body to the tracking list using a rigid body file.
	- Change to use the first rigid body's coordinate system.
	- Request/receive/display 10 frames of real-time rigid body
	  transformations and associated 3D data.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


sample12
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an ODAU collection.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display 10 frames of real-time ODAU Raw data.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


sample13
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an ODAU collection.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Initialize data file for spooling 3D data.
	- Initialize data file for spooling ODAU data.
	- Collect and spool 2 seconds of 3D and ODAU data to file.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


sample14
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an ODAU collection.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Initialize data file for spooling Raw data.
	- Initialize data file for spooling ODAU data.
	- Collect and spool 2 seconds of Raw and ODAU data to file.
	- De-activate the markers.
	- Convert the data in the Raw data file to a 3D data file.
	- Convert the data in the ODAU data file to a Voltage data file.
	- Disconnect from the Optotrak System.


sample15
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive real-time 3D data until Marker_01 appears.
	- Initialize data file for spooling 3D data.
	- Collect and spool 5 seconds of 3D data to file.
	- De-activate the markers.
	- Disconnect from the Optotrak System.
	- Open the 3D data file for reading.
	- Open a new 3D data file for writing.
	- Normalize the contents of the collected data file and write
	  the results to the new data file.
	- Close the input and output data files.


sample16
	- Initiate communications with the Optotrak System
	  as a secondary host.
	- Request the current Optotrak System status.
	- Request/receive/display 10 frames of Full Raw data.
	- Disconnect from the Optotrak System.


sample17
	- Initiate communications with the Optotrak System.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up the strober port table.
	- Set up an Optotrak collection.
	- Initialize data file for spooling 3D data.
	- Request/receive real-time 3D data until Marker_01 appears.
	- Start spooling data to file.
	- Request/receive real-time 3D data while at the same time writing
	  spooled 3D data to file.
	- Stop spooling to file after 5 seconds of data, or when
	  Marker_01 goes out of view.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


sample18
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive 50 frames of 3D data.
	- De-activate the markers.
	- Average all valid frames of data.
	- Create a new coordinate system defined by the averaged positions.
	- Change the measurement coordinate system.
	- Activate the markers.
	- Request/receive/display 10 frames of 3D data.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


sample19
	- Initiate communications with the Optotrak System.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Add rigid body to the tracking list using an array of 3D points.
	- Activate the markers.
	- Initialize a data file for spooling Raw Optotrak data.
	- Collect and spool 5 seconds of Raw data to file.
	- De-activate the markers.
	- Open the Raw data file for reading.
	- Read the entire contents of the raw data file.
	- Close the data file.
	- Convert the raw data to 3D data and display the data.
	- Convert the 3D data to transformation data and display the data.
	- Disconnect from the Optotrak System.


sample20
	- Set processing flags to generate system configuration externally.
	- Generate the system configuration externally and load the transputer code.
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display 10 frames of real-time 3D data.
	- De-activate the markers.
	- Stop the Optotrak collection.
	- Disconnect from the Optotrak System.
	- Set processing flags to generate system configuration internally.
	- Generate the system configuration internally and load the transputer code.
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display 10 frames of real-time 3D data.
	- De-activate the markers.
	- Stop the Optotrak collection.
	- Disconnect from the Optotrak System.
	- Set processing flags to perform data conversions on the host computer.


sample21
	- Set processing flags to perform data conversions on the host computer.
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Obtain/display the camera parameter status.
	- Select a new camera parameter set.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display 10 frames of real-time 3D data.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


sample22
	- Initiate communications with the Optotrak System.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection using the settings in 'optotrak.ini'.
	- Activate the markers.
	- Request/receive/display 10 frames of real-time 3D data.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


certus_sample
	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Set number of markers to activate on each strober.
	- Retrieve/display device properties.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display 5 frames of real-time 3D data.
	- De-activate the markers.
	- Stop the Optotrak collection.
	- Disconnect from the Optotrak System.
	- Loop this program to show changes in strober configuration.


certus_sample1
	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Set number of markers to activate on each strober.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display 1 frame of real-time 3D data.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


certus_sample2
	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Set number of markers to activate on each strober.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display 3D data until the strober configuration changes.
	- De-activate the markers.
	- Stop the Optotrak collection.
	- Disconnect from the Optotrak System.
	- Re-start collection and wait until configuration changes again.
	- Receive latest strober configuration.


certus_sample3
	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Retrieve the strober properties and display the number of properties.
	- Retrieve/display the property 'PORT' for each strober.
	- Disconnect from the Optotrak System.


certus_sample4
	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Display each strober's properties.
	- Set number of markers to activate on each strober.
	- Retrieve the new strober properties.
	- Display the new strober properties.
	- Display all the properties again.
	- Disconnect from the Optotrak System.


certus_sample11
	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Set any (except tracking VLED) VLED to BLINK.
	- Disconnect from the Optotrak System.


certus_sample12
	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Set any available beepers on for 1 second.
	- Disconnect from the Optotrak System.


certus_sample13
	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Set number of markers to activate on each strober.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Retrieve 3D data until a switch status change is detected.
	- Display current frame of 3D data.
	- Retrieve/display switch status.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


certus_sample14
	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Set number of markers to activate on each strober.
	- Add rigid body to tracking list from device SROMs.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Retrieve/display 1 frame of 6D data.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


certus_sample15
	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Request new name for each strober.
	- Set the name in memory for each strober.
	- Disconnect from the Optotrak System.


certus_sample16
	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Set number of markers to activate on each strober.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Retrieve number of sensors in the system.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display 1 frame of Centroid data.
	- Request/receive/display 1 frame of 3D data.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


certus_sample17
	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration until configuration is changed.
	- Set number of markers to activate on each strober.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Retrieve/display 1 frame of 3D data.
	- De-activate the markers.
	- Disconnect from the Optotrak System.


certus_sample18
	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Set number of markers to activate on each strober.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- De-activate the markers.
	- Loop, displaying real-time 3D data.  After the third data request,
	  Activate the markers.
	- Continue loop until tool configuration changes or switch is pressed.
	- De-activate the markers.
	- Disconnect from the Optotrak System.

certus_sample19
	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Set number of markers to activate on each strober.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Odau collection.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Loop, displaying real-time 3D and ODAU data.
	- Continue loop until tool configuration changes or switch is pressed.
	- De-activate the markers.
	- Disconnect from the Optotrak System.

certus_sample20
	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- If it finds Wireless strobers, it goes to the wireless mode
	  and saves collection properties to Wireless strobers
	- It asks user to replace Wireless strobers with Wireless transmitter
	  and checks if it was done.
	- Sets number of markers previously detected on Wireless strobers to
	  the Wireless transmitter
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display frames of real-time 3D data using wireless configuration
	  until the first marker is visible.
	- De-activate the markers.
	- Sets the system back to the wired mode.
	- Disconnect from the Optotrak System.

================================================================================
Copyright 2003-2008, Northern Digital Inc.
Optotrak is a registered trademark of Northern Digital Inc.
Certus is a trademark of Northern Digital Inc.
NDI is a registered trademark of Northern Digital Inc.
Measurment You Can Trust is a trademark of Northern Digital Inc.
Product names listed are trademarks of their respective manufacturer.
Company names listed are trademarks or tradenames of their respective companies.
================================================================================

