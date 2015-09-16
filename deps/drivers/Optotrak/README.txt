FUU v1.02.02.0111

Contains the firmware upgrade utility to update the optotrak to specific firmware.
In August 2014, we updated firmware of the Optotrak to package number 17 in Rovereto LAB.
The firmware upgrade file is:

Optotrak_PKG017_LSD.fuu

FIRMWARE UPGRADE PROCEDURE:

************************************************
You have to upgrade the units 1 camera at a time (this is a very important step).
************************************************

So what you want to do is unplug one camera from the system leaving just one camera plugged in to the SCU.
To upgrade, follow the steps below:
 
1.       Run fuu.exe
2.       Click on Settings > Communication
3.       Choose the right communication interface
4.       Click Refresh (Make sure that only the SCU and one camera is listed)
5.       Click on Open Package and load the PKG17_LSD.fuu(attached)
6.       Click Update Firmware.
 
Once you’ve done the above steps, you should now see the camera and the SCU to have package 17.
To upgrade the other camera, unplug the camera that you just upgraded from the SCU and then 
plug the other camera that still needs to be upgraded to the SCU. Repeat the steps above.
 
Once you have upgraded both cameras and the SCU, you can now try to run the same functions 
and see if you get the same error. Please let me know if you have any question or concern.

OPTOUPDATE to Update the OAPI
Hi Robert,
I'm glad you got up and running now. 
As for the Windows OAPI, I have uploaded a file to your support site account which you can use to update your Windows OAPI. To update:
1. Run the "OptoUpdate_v3.05.01.exe"
2. Click on "Update the system to v3.05"
3. Choose "Do Not Search for Hardware Components (Update Software Only)"
4. Hit "Next"
5. Choose the appropriate drive/s
6. Select the files you want to update. To make sure that all the appropriate files get updated, uncheck the "Select All Files" box. This will leave     a list of files that need to be updated.
7. Click "Install"
Upon completion, a dialogue box will appear notifying you that the installation was successful.
Please let me know if you have any other question.
Best regards,
Richard

