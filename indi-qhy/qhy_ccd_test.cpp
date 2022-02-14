/*
 QHY Test CCD

 Copyright (C) 2017 Jan Soldan

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <qhyccd.h>

#define VERSION 1.00

void hexDump (
    const char * desc,
    const void * addr,
    const int len,
    int perLine
) {
    // Silently ignore silly per-line values.

    if (perLine < 4 || perLine > 64) perLine = 16;

    int i;
    unsigned char buff[perLine+1];
    const unsigned char * pc = (const unsigned char *)addr;

    // Output description if given.

    if (desc != NULL) printf ("%s:\n", desc);

    // Length checks.

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    // Process every byte in the data.

    for (i = 0; i < len; i++) {
        // Multiple of perLine means new or first line (with line offset).

        if ((i % perLine) == 0) {
            // Only print previous-line ASCII buffer for lines beyond first.

            if (i != 0) printf ("  %s\n", buff);

            // Output the offset of current line.

            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.

        printf (" %02x", pc[i]);

        // And buffer a printable ASCII character for later.

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % perLine] = '.';
        else
            buff[i % perLine] = pc[i];
        buff[(i % perLine) + 1] = '\0';
    }

    // Pad out last line if not exactly perLine characters.

    while ((i % perLine) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII buffer.

    printf ("  %s\n", buff);
}


int main(int, char **)
{

    int USB_TRAFFIC = 10;
    int CHIP_GAIN = 10;
    int CHIP_OFFSET = 140;
    double EXPOSURE_TIME = 0.1;
    int camBinX = 2;
    int camBinY = 2;

    double chipWidthMM;
    double chipHeightMM;
    double pixelWidthUM;
    double pixelHeightUM;

    unsigned int roiStartX;
    unsigned int roiStartY;
    unsigned int roiSizeX;
    unsigned int roiSizeY;

    unsigned int overscanStartX;
    unsigned int overscanStartY;
    unsigned int overscanSizeX;
    unsigned int overscanSizeY;

    unsigned int effectiveStartX;
    unsigned int effectiveStartY;
    unsigned int effectiveSizeX;
    unsigned int effectiveSizeY;

    unsigned int maxImageSizeX;
    unsigned int maxImageSizeY;
    unsigned int bpp;
    unsigned int channels;

    uint8_t *pImgData = 0;

    printf("QHY Test CCD using SingleFrameMode, Version: %.2f\n", VERSION);

    // init SDK
    int retVal = InitQHYCCDResource();
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("SDK resources initialized.\n");
    }
    else
    {
        printf("Cannot initialize SDK resources, error: %d\n", retVal);
        return 1;
    }

    // scan cameras
    int camCount = ScanQHYCCD();
    if (camCount > 0)
    {
        printf("Number of QHYCCD cameras found: %d \n", camCount);
    }
    else
    {
        printf("No QHYCCD camera found, please check USB or power.\n");
        return 1;
    }

    // iterate over all attached cameras
    bool camFound = false;
    char camId[32];

    for (int i = 0; i < camCount; i++)
    {
        retVal = GetQHYCCDId(i, camId);
        if (QHYCCD_SUCCESS == retVal)
        {
            printf("Application connected to the following camera from the list: Index: %d,  cameraID = %s\n", (i + 1), camId);
            camFound = true;
            break;
        }
    }

    if (!camFound)
    {
        printf("The detected camera is not QHYCCD or other error.\n");
        // release sdk resources
        retVal = ReleaseQHYCCDResource();
        if (QHYCCD_SUCCESS == retVal)
        {
            printf("SDK resources released.\n");
        }
        else
        {
            printf("Cannot release SDK resources, error %d.\n", retVal);
        }
        return 1;
    }

    // open camera
    qhyccd_handle *pCamHandle = OpenQHYCCD(camId);
    if (pCamHandle != nullptr)
    {
        printf("Open QHYCCD success.\n");
    }
    else
    {
        printf("Open QHYCCD failure.\n");
        return 1;
    }

    // set single frame mode
    int mode = 0;
    retVal = SetQHYCCDStreamMode(pCamHandle, mode);
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("SetQHYCCDStreamMode set to: %d, success.\n", mode);
    }
    else
    {
        printf("SetQHYCCDStreamMode: %d failure, error: %d\n", mode, retVal);
        return 1;
    }

    // initialize camera
    retVal = InitQHYCCD(pCamHandle);
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("InitQHYCCD success.\n");
    }
    else
    {
        printf("InitQHYCCD faililure, error: %d\n", retVal);
        return 1;
    }

    // get overscan area
    retVal = GetQHYCCDOverScanArea(pCamHandle, &overscanStartX, &overscanStartY, &overscanSizeX, &overscanSizeY);
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("GetQHYCCDOverScanArea:\n");
        printf("Overscan Area startX x startY : %d x %d\n", overscanStartX, overscanStartY);
        printf("Overscan Area sizeX  x sizeY  : %d x %d\n", overscanSizeX, overscanSizeY);
    }
    else
    {
        printf("GetQHYCCDOverScanArea failure, error: %d\n", retVal);
        return 1;
    }

    // get effective area
    retVal = GetQHYCCDEffectiveArea(pCamHandle, &effectiveStartX, &effectiveStartY, &effectiveSizeX, &effectiveSizeY);
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("GetQHYCCDEffectiveArea:\n");
        printf("Effective Area startX x startY: %d x %d\n", effectiveStartX, effectiveStartY);
        printf("Effective Area sizeX  x sizeY : %d x %d\n", effectiveSizeX, effectiveSizeY);
    }
    else
    {
        printf("GetQHYCCDOverScanArea failure, error: %d\n", retVal);
        return 1;
    }

    // get chip info
    retVal = GetQHYCCDChipInfo(pCamHandle, &chipWidthMM, &chipHeightMM, &maxImageSizeX, &maxImageSizeY, &pixelWidthUM,
                               &pixelHeightUM, &bpp);
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("GetQHYCCDChipInfo:\n");
        printf("Effective Area startX x startY: %d x %d\n", effectiveStartX, effectiveStartY);
        printf("Chip  size width x height     : %.6f x %.6f [mm]\n", chipWidthMM, chipHeightMM);
        printf("Pixel size width x height     : %.3f x %.3f [um]\n", pixelWidthUM, pixelHeightUM);
        printf("Image size width x height     : %d x %d\n", maxImageSizeX, maxImageSizeY);
    }
    else
    {
        printf("GetQHYCCDChipInfo failure, error: %d\n", retVal);
        return 1;
    }

    // set ROI
    roiStartX = 0;
    roiStartY = 0;
    roiSizeX = maxImageSizeX;
    roiSizeY = maxImageSizeY;

    // check color camera
    retVal = IsQHYCCDControlAvailable(pCamHandle, CAM_COLOR);
    if (retVal == BAYER_GB || retVal == BAYER_GR || retVal == BAYER_BG || retVal == BAYER_RG)
    {
        printf("This is a color camera.\n");
        SetQHYCCDDebayerOnOff(pCamHandle, true);
        SetQHYCCDParam(pCamHandle, CONTROL_WBR, 20);
        SetQHYCCDParam(pCamHandle, CONTROL_WBG, 20);
        SetQHYCCDParam(pCamHandle, CONTROL_WBB, 20);
    }
    else
    {
        printf("This is a mono camera.\n");
    }

    // check traffic
    retVal = IsQHYCCDControlAvailable(pCamHandle, CONTROL_USBTRAFFIC);
    if (QHYCCD_SUCCESS == retVal)
    {
        double min;
        double max = USB_TRAFFIC;
        double step;

        retVal = GetQHYCCDParamMinMaxStep(pCamHandle, CONTROL_USBTRAFFIC, &min, &max, &step);
        if (QHYCCD_SUCCESS == retVal)
        {
            printf("USB min/max/step: %.2f %.2f %.2f \n", min, max, step);
        }
        else
        {
            printf("Could not get USB min/max\n");
        }

        retVal = SetQHYCCDParam(pCamHandle, CONTROL_USBTRAFFIC, 0.0);
        if (QHYCCD_SUCCESS == retVal)
        {
            printf("SetQHYCCDParam CONTROL_USBTRAFFIC set to: %.2f, success.\n", 0.0);
        }
        else
        {
            printf("SetQHYCCDParam CONTROL_USBTRAFFIC failure, error: %d\n", retVal);
            getchar();
            return 1;
        }
    }

    // check gain
    retVal = IsQHYCCDControlAvailable(pCamHandle, CONTROL_GAIN);
    if (QHYCCD_SUCCESS == retVal)
    {
        retVal = SetQHYCCDParam(pCamHandle, CONTROL_GAIN, CHIP_GAIN);
        if (retVal == QHYCCD_SUCCESS)
        {
            printf("SetQHYCCDParam CONTROL_GAIN set to: %d, success\n", CHIP_GAIN);
        }
        else
        {
            printf("SetQHYCCDParam CONTROL_GAIN failure, error: %d\n", retVal);
            getchar();
            return 1;
        }
    }

    // check offset
    retVal = IsQHYCCDControlAvailable(pCamHandle, CONTROL_OFFSET);
    if (QHYCCD_SUCCESS == retVal)
    {
        retVal = SetQHYCCDParam(pCamHandle, CONTROL_OFFSET, CHIP_OFFSET);
        if (QHYCCD_SUCCESS == retVal)
        {
            printf("SetQHYCCDParam CONTROL_OFFSET set to: %d, success.\n", CHIP_OFFSET);
        }
        else
        {
            printf("SetQHYCCDParam CONTROL_OFFSET failed.\n");
            getchar();
            return 1;
        }
    }

    // check read mode in QHY42
    uint32_t currentReadMode = 0;
    char *modeName = (char *)malloc((200) * sizeof(char));;
    retVal = GetQHYCCDReadMode(pCamHandle, &currentReadMode);
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("Default read mode: %d \n", currentReadMode);
        retVal = GetQHYCCDReadModeName(pCamHandle, currentReadMode, modeName);
        if (QHYCCD_SUCCESS == retVal)
        {
            printf("Default read mode name %s \n", modeName);
        }
        else
        {
            printf("Error reading mode name \n");
            getchar();
            return 1;
        }

        // Set read modes and read resolution for each one
        uint32_t readModes = 0;
        uint32_t imageRMw, imageRMh;
        uint32_t i = 0;
        retVal = GetQHYCCDNumberOfReadModes(pCamHandle, &readModes);
        for(i = 0; i < readModes; i++)
        {
            // Set read mode and get resolution
            retVal = SetQHYCCDReadMode(pCamHandle, i);
            if (QHYCCD_SUCCESS == retVal)
            {
                // Get resolution
                retVal = GetQHYCCDReadModeName(pCamHandle, i, modeName);
                if (QHYCCD_SUCCESS == retVal)
                {
                    printf("Read mode name %s \n", modeName);
                }
                else
                {
                    printf("Error reading mode name \n");
                    getchar();
                    return 1;
                }
                retVal = GetQHYCCDReadModeResolution(pCamHandle, i, &imageRMw, &imageRMh);
                printf("GetQHYCCDChipInfo in this ReadMode: imageW: %d imageH: %d \n", imageRMw, imageRMh);
            }
        }

    }


    // set exposure time
    retVal = SetQHYCCDParam(pCamHandle, CONTROL_EXPOSURE, EXPOSURE_TIME);
    printf("SetQHYCCDParam CONTROL_EXPOSURE set to: %.3f, success.\n", EXPOSURE_TIME);
    if (QHYCCD_SUCCESS == retVal)
    {
    }
    else
    {
        printf("SetQHYCCDParam CONTROL_EXPOSURE failure, error: %d\n", retVal);
        getchar();
        return 1;
    }

    // Query GPS
    retVal = IsQHYCCDControlAvailable(pCamHandle, CAM_GPS);
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("GPS available\n");

        double gpsStatus = GetQHYCCDParam(pCamHandle, CAM_GPS);
        printf("GPS status: %d\n", (int)gpsStatus);
        
        retVal = SetQHYCCDParam(pCamHandle, CAM_GPS, 1);
        if (QHYCCD_SUCCESS == retVal)
        {
            gpsStatus = GetQHYCCDParam(pCamHandle, CAM_GPS);
            printf("GPS status: %d\n", (int)gpsStatus);
        }
        else
        {
            printf("Could not set GPS mode, error: %d\n", retVal);
            return 1;
        }
    }
    else
    {
        printf("GPS not available\n");
    }


    // Query binning
    retVal = IsQHYCCDControlAvailable(pCamHandle, CAM_BIN1X1MODE);
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("Bin 1x1 available\n");
    }
    else
    {
        printf("Bin 1x1 not available\n");
    }

    retVal = IsQHYCCDControlAvailable(pCamHandle, CAM_BIN2X2MODE);
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("Bin 2x2 available\n");
    }
    else
    {
        printf("Bin 2x2 not available\n");
    }

    retVal = IsQHYCCDControlAvailable(pCamHandle, CAM_BIN3X3MODE);
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("Bin 3x3 available\n");
    }
    else
    {
        printf("Bin 3x3 not available\n");
    }

    retVal = IsQHYCCDControlAvailable(pCamHandle, CAM_BIN4X4MODE);
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("Bin 4x4 available\n");
    }
    else
    {
        printf("Bin 4x4 not available\n");
    }

    // set binning mode
    retVal = SetQHYCCDBinMode(pCamHandle, camBinX, camBinY);
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("SetQHYCCDBinMode set to: binX: %d, binY: %d, success.\n", camBinX, camBinY);
    }
    else
    {
        printf("SetQHYCCDBinMode failure, error: %d\n", retVal);
        return 1;
    }

    // set bit resolution
    retVal = IsQHYCCDControlAvailable(pCamHandle, CONTROL_TRANSFERBIT);
    if (QHYCCD_SUCCESS == retVal)
    {
        retVal = SetQHYCCDBitsMode(pCamHandle, 16);
        if (QHYCCD_SUCCESS == retVal)
        {
            printf("SetQHYCCDParam CONTROL_TRANSFERBIT set to: %d, success.\n", 16);
        }
        else
        {
            printf("SetQHYCCDParam CONTROL_TRANSFERBIT failure, error: %d\n", retVal);
            getchar();
            return 1;
        }
    }

    // get single frame
    for(int i = 0; i < 100; ++i)
    {
        printf("img %d\n", i);

        // single frame
        printf("ExpQHYCCDSingleFrame(pCamHandle) - start...\n");
        retVal = ExpQHYCCDSingleFrame(pCamHandle);
        printf("ExpQHYCCDSingleFrame(pCamHandle) - end...\n");
        if (QHYCCD_ERROR != (uint32_t) retVal)
        {
            printf("ExpQHYCCDSingleFrame success (%d).\n", retVal);
            if (QHYCCD_READ_DIRECTLY != retVal)
            {
                sleep(1);
            }
        }
        else
        {
            printf("ExpQHYCCDSingleFrame failure, error: %d\n", retVal);
            return 1;
        }

        // get requested memory lenght
        uint32_t length = GetQHYCCDMemLength(pCamHandle);

        if (length > 0)
        {
            pImgData = new uint8_t[length];
            memset(pImgData, 0, length);
            printf("Allocated memory for frame: %d [uint8_t].\n", length);
        }
        else
        {
            printf("Cannot allocate memory for frame.\n");
            return 1;
        }

        retVal = GetQHYCCDSingleFrame(pCamHandle, &roiSizeX, &roiSizeY, &bpp, &channels, pImgData);
        if (QHYCCD_SUCCESS == retVal)
        {
            printf("GetQHYCCDSingleFrame: %d x %d, bpp: %d, channels: %d, success.\n", roiSizeX, roiSizeY, bpp, channels);
            //process image here

            hexDump("pImgData", pImgData, 256, 16);
        }
        else
        {
            printf("GetQHYCCDSingleFrame failure, error: %d\n", retVal);
        }

        delete [] pImgData;

    }



    retVal = CancelQHYCCDExposingAndReadout(pCamHandle);
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("CancelQHYCCDExposingAndReadout success.\n");
    }
    else
    {
        printf("CancelQHYCCDExposingAndReadout failure, error: %d\n", retVal);
        return 1;
    }

    // close camera handle
    retVal = CloseQHYCCD(pCamHandle);
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("Close QHYCCD success.\n");
    }
    else
    {
        printf("Close QHYCCD failure, error: %d\n", retVal);
    }

    // release sdk resources
    retVal = ReleaseQHYCCDResource();
    if (QHYCCD_SUCCESS == retVal)
    {
        printf("SDK resources released.\n");
    }
    else
    {
        printf("Cannot release SDK resources, error %d.\n", retVal);
        return 1;
    }

    return 0;
}
