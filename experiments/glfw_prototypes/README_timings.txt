Segui queste istruzioni per generare le modeline corrette per potere usare un monitor anche su linux 
se le modeline correnti non sono lette da linux:


http://www.x.org/wiki/FAQVideoModes/#index4h2



In breve:

 the pixel timings are taken from the log.
Obtaining modelines from Windows program PowerStrip

If you have a dual boot system and a monitor / graphics card that works fine under MS Windows but you can't figure out the equivalent modeline parameters for Linux, you can use the Windows program PowerStrip.

I used this to configure X.org to work correctly with my Sun X7200A 20.1" LCD monitor and my Dell C400 Latitude notebook.

Download and install PowerStrip from http://entechtaiwan.net/util/ps.shtm

Once installed and running you will see a PowerStrip icon in the system tray.

With the display using the required settings (for me this was 1600x1200).

Right Click on the PowerStrip icon in the system tray to display the pop up menu.

Select sub menu "Display Profiles" then select menu item "Configure"

The "Display profiles" window will be displayed, click button "Advanced timing options".

The "Advanced Timing Options" window will be displayed, click button copy timings to clip board (this button is the icon to the right of the "Cancel" button.

Paste your clipboard somewhere (notepad will do) and have a look. You will see at the end of the pasted data will be the information you need for an Xorg modeline.

For my setup PowerStrip put the following information in the clipboard

PowerStrip timing parameters:
1600x1200=1600,8,64,104,1200,5,10,24,132000,512

Generic timing details for 1600x1200:
HFP=8 HSW=64 HBP=104 kHz=74 VFP=5 VSW=10 VBP=24 Hz=60

VESA detailed timing:
PClk=132.00 H.Active=1600 H.Blank=176 H.Offset=-8 HSW=64 V.Active=1200 V.Blank=39 V.Offset=5 VSW=10

Linux modeline parameters:
"1600x1200" 132.000 1600 1608 1672 1776 1200 1205 1215 1239 +hsync +vsync

As you can see the last line is all you need to know to create a modeline.

With that information you can boot back into Linux and add the modeline to the monitor section. Within my xorg.conf file I now have:

Section "Monitor"
        Identifier      "Generic Monitor"
        VendorName      "Sun"
        ModelName       "X7200A"
        Option          "DPMS"
        ModeLine        "1600x1200" 132.000 1600 1608 1672 1776 1200 1205 1215 1239 +hsync +vsync
EndSection