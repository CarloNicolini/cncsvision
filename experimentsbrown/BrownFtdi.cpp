#include "BrownFtdi.h"

struct ftdi_context *ftdi;
int f,i;
unsigned char buf[1];

namespace BrownFtdi
{
    void switch_connect()
    {
        #ifndef SIMULATION
            bool done = false;
            while(!done)
            {
                if ((ftdi = ftdi_new()) == 0)
                {
                    fprintf(stderr, "ftdi_new failed\n");
                    exit(0);
                }

                f = ftdi_usb_open(ftdi, 0x0403, 0x6001);

                if (f < 0 && f != -5)
                {
                    fprintf(stderr, "unable to open ftdi device: %d (%s)\n", f, ftdi_get_error_string(ftdi));
                    exit(0);
                }

                printf("ftdi open succeeded: %d\n",f);

                printf("enabling bitbang mode\n");
                ftdi_set_bitmode(ftdi, 0xFF, BITMODE_BITBANG);

                buf[0] = 0x0;
                printf("turning everything off\n");
                f = ftdi_write_data(ftdi, buf, 1);
                if (f < 0)
                {
                    fprintf(stderr,"write failed for 0x%x, error %d (%s)\n",buf[0],f, ftdi_get_error_string(ftdi));
                    cerr << "Disconnecting..." << endl;
                    switch_disconnect();
                    sleep(1);
                } else
                    break;
            }
        #else
            cerr << "FTDI connected." << endl;
        #endif
    }

    void switch_on(int switch_number)
    {
        #ifndef SIMULATION
            bool done = false;
            while(!done)
            {
                buf[0] =  0 & (0xFF ^ 1 << ((switch_number-1) % 8));

                printf("%02hhx ",buf[0]);
                fflush(stdout);
                f = ftdi_write_data(ftdi, buf, 1);
                if (f < 0)
                {
                    errorHandlingFunction();
                } else
                    done = true;
            }
        #else
            cerr << "Switch #" << switch_number << " on." << endl;
        #endif
    }

    void switch_off(int switch_number)
    {
        #ifndef SIMULATION
            bool done = false;
            while(!done)
            {
                buf[0] =  0 | (0x0 ^ 1 << ((switch_number-1) % 8));

                printf("%hhx ",buf[0]);
                fflush(stdout);
                f = ftdi_write_data(ftdi, buf, 1);
                if (f < 0)
                {
                    errorHandlingFunction();
                } else
                    done = true;
            }
        #else
            cerr << "Switch #" << switch_number << " off." << endl;
        #endif
    }

    void errorHandlingFunction()
    {
        fprintf(stderr,"write failed for 0x%x, error %d (%s)\n",buf[0],f, ftdi_get_error_string(ftdi));
        cerr << "Disconnecting..." << endl;
        switch_disconnect();
        sleep(1);
        cerr << "Attempting to reconnect..." << endl;
        switch_connect();
    }

    void switch_disconnect()
    {
        #ifndef SIMULATION
            ftdi_disable_bitbang(ftdi);

            ftdi_usb_close(ftdi);

            ftdi_free(ftdi);
        #else
            cerr << "FTDI disconnected." << endl;
        #endif   
    }

    void switches_on()
	{
        bool done = false;
        while(!done)
        {
    		buf[0] =  0 & ((0xFF ^ 1 << ((1-1) % 8)) & (0xFF ^ 1 << ((2-1) % 8)));

    	    printf("%02hhx ",buf[0]);
    	    fflush(stdout);
    	    f = ftdi_write_data(ftdi, buf, 1);
    	    if (f < 0)
            {
                errorHandlingFunction();
            } else
                done = true;
        }
	}

	void switches_off()
	{
        bool done = false;
        while(!done)
        {
    		buf[0] =  0 | ((0x0 ^ 1 << ((1-1) % 8)) | (0x0 ^ 1 << ((2-1) % 8)));

    	    printf("%02hhx ",buf[0]);
    	    fflush(stdout);
    	    f = ftdi_write_data(ftdi, buf, 1);
    	    if (f < 0)
            {
                errorHandlingFunction();
            } else
                done = true;
    	}
    }
}