/*
 QHY Video Test CCD

 Copyright (C) 2019 Jasem Mutlaq

 Based on Single Frame Test by Jan Soldan

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

#include <iostream>
#include <thread>
#include <atomic>

#define VERSION 1.00

static std::atomic<bool> exit_thread { false };


void decodeGPSHeader(unsigned char *pImgData)
{

    //char ts[64] = {0}, iso8601[64] = {0}, data[64] = {0};

    uint8_t gpsarray[64] = {0};
    memcpy(gpsarray, pImgData, 64);
/*
    // Sequence Number
    GPSHeader.seqNumber = gpsarray[0] << 24 | gpsarray[1] << 16 | gpsarray[2] << 8 | gpsarray[3];
    snprintf(data, 64, "%u", GPSHeader.seqNumber);
    IUSaveText(&GPSDataHeaderT[GPS_DATA_SEQ_NUMBER], data);

    GPSHeader.tempNumber = gpsarray[4];

    // Width
    GPSHeader.width = gpsarray[5] << 8 | gpsarray[6];
    snprintf(data, 64, "%u", GPSHeader.width);
    IUSaveText(&GPSDataHeaderT[GPS_DATA_WIDTH], data);

    // Height
    GPSHeader.height = gpsarray[7] << 8 | gpsarray[8];
    snprintf(data, 64, "%u", GPSHeader.height);
    IUSaveText(&GPSDataHeaderT[GPS_DATA_HEIGHT], data);

    // Latitude
    GPSHeader.latitude = gpsarray[9] << 24 | gpsarray[10] << 16 | gpsarray[11] << 8 | gpsarray[12];
    snprintf(data, 64, "%u", GPSHeader.latitude);
    IUSaveText(&GPSDataHeaderT[GPS_DATA_LATITUDE], data);

    // Longitude
    GPSHeader.longitude = gpsarray[13] << 24 | gpsarray[14] << 16 | gpsarray[15] << 8 | gpsarray[16];
    snprintf(data, 64, "%u", GPSHeader.longitude);
    IUSaveText(&GPSDataHeaderT[GPS_DATA_LONGITUDE], data);

*/

    uint32_t latitude = gpsarray[9] << 24 | gpsarray[10] << 16 | gpsarray[11] << 8 | gpsarray[12];
    printf("lat: %d\n", latitude);

    uint32_t longitude = gpsarray[13] << 24 | gpsarray[14] << 16 | gpsarray[15] << 8 | gpsarray[16];
    printf("lon: %u\n", longitude);

/*
    // Start Flag
    GPSHeader.start_flag = gpsarray[17];
    snprintf(data, 64, "%u", GPSHeader.start_flag);
    IUSaveText(&GPSDataStartT[GPS_DATA_START_FLAG], data);

    // Start Seconds
    GPSHeader.start_sec = gpsarray[18] << 24 | gpsarray[19] << 16 | gpsarray[20] << 8 | gpsarray[21];
    snprintf(data, 64, "%u", GPSHeader.start_sec);
    IUSaveText(&GPSDataStartT[GPS_DATA_START_SEC], data);

    // Start microseconds
    // It's a 10Mhz crystal so we divide by 10 to get microseconds
    GPSHeader.start_us = (gpsarray[22] << 16 | gpsarray[23] << 8 | gpsarray[24]) / 10.0;
    snprintf(data, 64, "%.1f", GPSHeader.start_us);
    IUSaveText(&GPSDataStartT[GPS_DATA_START_USEC], data);
*/

    uint32_t startSec = gpsarray[18] << 24 | gpsarray[19] << 16 | gpsarray[20] << 8 | gpsarray[21];
    double startUSec =  (gpsarray[22] << 16 | gpsarray[23] << 8 | gpsarray[24]) / 10.0;
    printf("start: %u, %08.1f\n", startSec, startUSec);


/*
    // Start JD
    GPSHeader.start_jd = JStoJD(GPSHeader.start_sec, GPSHeader.start_us);
    // Get ISO8601
    JDtoISO8601(GPSHeader.start_jd, iso8601);
    // Add millisecond
    snprintf(ts, sizeof(ts), "%s.%03d", iso8601, static_cast<int>(GPSHeader.start_us / 1000.0));
    IUSaveText(&GPSDataStartT[GPS_DATA_START_TS], ts);

    // End Flag
    GPSHeader.end_flag = gpsarray[25];
    snprintf(data, 64, "%u", GPSHeader.end_flag);
    IUSaveText(&GPSDataEndT[GPS_DATA_END_FLAG], data);

    // End Seconds
    GPSHeader.end_sec = gpsarray[26] << 24 | gpsarray[27] << 16 | gpsarray[28] << 8 | gpsarray[29];
    snprintf(data, 64, "%u", GPSHeader.end_sec);
    IUSaveText(&GPSDataEndT[GPS_DATA_END_SEC], data);

    // End Microseconds
    GPSHeader.end_us = (gpsarray[30] << 16 | gpsarray[31] << 8 | gpsarray[32]) / 10.0;
    snprintf(data, 64, "%.1f", GPSHeader.end_us);
    IUSaveText(&GPSDataEndT[GPS_DATA_END_USEC], data);

    // End JD
    GPSHeader.end_jd = JStoJD(GPSHeader.end_sec, GPSHeader.end_us);
    // Get ISO8601
    JDtoISO8601(GPSHeader.end_jd, iso8601);
    // Add millisecond
    snprintf(ts, sizeof(ts), "%s.%03d", iso8601, static_cast<int>(GPSHeader.end_us / 1000.0));
    IUSaveText(&GPSDataEndT[GPS_DATA_END_TS], ts);

    // Now Flag
    GPSHeader.now_flag = gpsarray[33];
    snprintf(data, 64, "%u", GPSHeader.now_flag);
    IUSaveText(&GPSDataNowT[GPS_DATA_NOW_FLAG], data);

    // Now Seconds
    GPSHeader.now_sec = gpsarray[34] << 24 | gpsarray[35] << 16 | gpsarray[36] << 8 | gpsarray[37];
    snprintf(data, 64, "%u", GPSHeader.now_sec);
    IUSaveText(&GPSDataNowT[GPS_DATA_NOW_SEC], data);

    // Now microseconds
    GPSHeader.now_us = (gpsarray[38] << 16 | gpsarray[39] << 8 | gpsarray[40]) / 10.0;
    snprintf(data, 64, "%.1f", GPSHeader.now_us);
    IUSaveText(&GPSDataNowT[GPS_DATA_NOW_USEC], data);

    // Now JD
    GPSHeader.now_jd = JStoJD(GPSHeader.now_sec, GPSHeader.now_us);
    // Get ISO8601
    JDtoISO8601(GPSHeader.now_jd, iso8601);
    // Add millisecond
    snprintf(ts, sizeof(ts), "%s.%03d", iso8601, static_cast<int>(GPSHeader.now_us / 1000.0));
    IUSaveText(&GPSDataNowT[GPS_DATA_NOW_TS], ts);

    // PPS
    GPSHeader.max_clock = gpsarray[41] << 16 | gpsarray[42] << 8 | gpsarray[43];
    snprintf(data, 64, "%u", GPSHeader.max_clock);
    IUSaveText(&GPSDataHeaderT[GPS_DATA_MAX_CLOCK], data);

    IDSetText(&GPSDataHeaderTP, nullptr);
    IDSetText(&GPSDataStartTP, nullptr);
    IDSetText(&GPSDataEndTP, nullptr);
    IDSetText(&GPSDataNowTP, nullptr);
*/

    uint8_t state = (gpsarray[33] & 0xF0) >> 4;
    printf("GPS state: %u\n", state);

/*
    GPSState newGPState = static_cast<GPSState>((GPSHeader.now_flag & 0xF0) >> 4);
    if (GPSStateL[newGPState].s == IPS_IDLE)
    {
        GPSStateL[GPS_ON].s = IPS_IDLE;
        GPSStateL[GPS_SEARCHING].s = IPS_IDLE;
        GPSStateL[GPS_LOCKING].s = IPS_IDLE;
        GPSStateL[GPS_LOCKED].s = IPS_IDLE;

        GPSStateL[newGPState].s = IPS_BUSY;
        GPSStateLP.s = IPS_OK;
        IDSetLight(&GPSStateLP, nullptr);
    }
    */
}


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


int videoThread(qhyccd_handle *pCamHandle, unsigned char *pImgData)
{
    std::chrono::high_resolution_clock timer;
    using fsec = std::chrono::duration<float>;
    uint32_t frames = 0, w, h, bpp, channels;

    auto start = timer.now();

    while (!exit_thread)
    {
        if (GetQHYCCDLiveFrame(pCamHandle, &w, &h, &bpp, &channels, pImgData) == QHYCCD_SUCCESS)
        {
            frames++;
            hexDump("pImgData", pImgData, 32, 16);
            decodeGPSHeader(pImgData);

            auto stop = timer.now();
            fsec duration = (stop - start);
            if (duration.count() >= 3)
            {
                fprintf(stderr, "Frames: %d Duration: %.3f seconds FPS: %.3f\n", frames, duration.count(), frames / duration.count());
                start = timer.now();
                frames = 0;
            }
            usleep(3000);
        }
        else
            usleep(1000);
    }

    return 0;
}

int main(int, char **)
{
    int USB_TRAFFIC = 20;
    int USB_SPEED = 2;
    int CHIP_GAIN = 1;
    int CHIP_OFFSET = 180;
    double EXPOSURE_TIME = 1;
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
    //unsigned int channels;

    unsigned char *pImgData = nullptr;

    EnableQHYCCDLogFile(false);
    EnableQHYCCDMessage(false);

    fprintf(stderr, "QHY Video Test using VideoFrameMode, Version: %.2f\n", VERSION);

    // init SDK
    int rc = InitQHYCCDResource();
    if (QHYCCD_SUCCESS == rc)
    {
        fprintf(stderr, "SDK resources initialized.\n");
    }
    else
    {
        fprintf(stderr, "Cannot initialize SDK resources, error: %d\n", rc);
        return 1;
    }

    // scan cameras
    int camCount = ScanQHYCCD();
    if (camCount > 0)
    {
        fprintf(stderr, "Number of QHYCCD cameras found: %d \n", camCount);
    }
    else
    {
        fprintf(stderr, "No QHYCCD camera found, please check USB or power.\n");
        return 1;
    }

    // iterate over all attached cameras
    bool camFound = false;
    char camId[32];

    for (int i = 0; i < camCount; i++)
    {
        rc = GetQHYCCDId(i, camId);
        if (QHYCCD_SUCCESS == rc)
        {
            fprintf(stderr, "Application connected to the following camera from the list: Index: %d,  cameraID = %s\n", (i + 1), camId);
            camFound = true;
            break;
        }
    }

    if (!camFound)
    {
        fprintf(stderr, "The detected camera is not QHYCCD or other error.\n");
        // release sdk resources
        rc = ReleaseQHYCCDResource();
        if (QHYCCD_SUCCESS == rc)
        {
            fprintf(stderr, "SDK resources released.\n");
        }
        else
        {
            fprintf(stderr, "Cannot release SDK resources, error %d.\n", rc);
        }
        return 1;
    }

    // open camera
    qhyccd_handle *pCamHandle = OpenQHYCCD(camId);
    if (pCamHandle != nullptr)
    {
        fprintf(stderr, "Open QHYCCD success.\n");
    }
    else
    {
        fprintf(stderr, "Open QHYCCD failure.\n");
        return 1;
    }

    // get overscan area
    rc = GetQHYCCDOverScanArea(pCamHandle, &overscanStartX, &overscanStartY, &overscanSizeX, &overscanSizeY);
    if (QHYCCD_SUCCESS == rc)
    {
        fprintf(stderr, "GetQHYCCDOverScanArea:\n");
        fprintf(stderr, "Overscan Area startX x startY : %d x %d\n", overscanStartX, overscanStartY);
        fprintf(stderr, "Overscan Area sizeX  x sizeY  : %d x %d\n", overscanSizeX, overscanSizeY);
    }
    else
    {
        fprintf(stderr, "GetQHYCCDOverScanArea failure, error: %d\n", rc);
        return 1;
    }

    // get effective area
    rc = GetQHYCCDEffectiveArea(pCamHandle, &effectiveStartX, &effectiveStartY, &effectiveSizeX, &effectiveSizeY);
    if (QHYCCD_SUCCESS == rc)
    {
        fprintf(stderr, "GetQHYCCDEffectiveArea:\n");
        fprintf(stderr, "Effective Area startX x startY: %d x %d\n", effectiveStartX, effectiveStartY);
        fprintf(stderr, "Effective Area sizeX  x sizeY : %d x %d\n", effectiveSizeX, effectiveSizeY);
    }
    else
    {
        fprintf(stderr, "GetQHYCCDOverScanArea failure, error: %d\n", rc);
        return 1;
    }

    // get chip info
    rc = GetQHYCCDChipInfo(pCamHandle, &chipWidthMM, &chipHeightMM, &maxImageSizeX, &maxImageSizeY, &pixelWidthUM,
                           &pixelHeightUM, &bpp);
    if (QHYCCD_SUCCESS == rc)
    {
        fprintf(stderr, "GetQHYCCDChipInfo:\n");
        fprintf(stderr, "Effective Area startX x startY: %d x %d\n", effectiveStartX, effectiveStartY);
        fprintf(stderr, "Chip  size width x height     : %.3f x %.3f [mm]\n", chipWidthMM, chipHeightMM);
        fprintf(stderr, "Pixel size width x height     : %.3f x %.3f [um]\n", pixelWidthUM, pixelHeightUM);
        fprintf(stderr, "Image size width x height     : %d x %d\n", maxImageSizeX, maxImageSizeY);
    }
    else
    {
        fprintf(stderr, "GetQHYCCDChipInfo failure, error: %d\n", rc);
        return 1;
    }

    // set ROI
    roiStartX = 0;
    roiStartY = 0;
    roiSizeX = maxImageSizeX;
    roiSizeY = maxImageSizeY;

    // check color camera
    rc = IsQHYCCDControlAvailable(pCamHandle, CAM_COLOR);
    if (rc == BAYER_GB || rc == BAYER_GR || rc == BAYER_BG || rc == BAYER_RG)
    {
        fprintf(stderr, "This is a color camera.\n");
        SetQHYCCDDebayerOnOff(pCamHandle, true);
        SetQHYCCDParam(pCamHandle, CONTROL_WBR, 20);
        SetQHYCCDParam(pCamHandle, CONTROL_WBG, 20);
        SetQHYCCDParam(pCamHandle, CONTROL_WBB, 20);
    }
    else
    {
        fprintf(stderr, "This is a mono camera.\n");
    }

    // set exposure time
    rc = SetQHYCCDParam(pCamHandle, CONTROL_EXPOSURE, EXPOSURE_TIME);
    fprintf(stderr, "SetQHYCCDParam CONTROL_EXPOSURE set to: %.3f us, success.\n", EXPOSURE_TIME);
    if (QHYCCD_SUCCESS == rc)
    {
    }
    else
    {
        fprintf(stderr, "SetQHYCCDParam CONTROL_EXPOSURE failure, error: %d us\n", rc);
        getchar();
        return 1;
    }

    // N.B. SetQHYCCDStreamMode must be called immediately after CONTROL_EXPOSURE is SET
    // 1. Exposure
    // 2. Stream Mode
    // 3. Speed
    // 4. Traffic
    // 5. 8-bit
    rc = SetQHYCCDStreamMode(pCamHandle, 1);
    if (rc != QHYCCD_SUCCESS)
    {
        fprintf(stderr, "SetQHYCCDStreamMode failed: %d", rc);
    }

    // initialize camera after setting stream mode
    rc = InitQHYCCD(pCamHandle);
    if (QHYCCD_SUCCESS == rc)
    {
        printf("InitQHYCCD success.\n");
    }
    else
    {
        printf("InitQHYCCD faililure, error: %d\n", rc);
        return 1;
    }

    // check traffic
    rc = IsQHYCCDControlAvailable(pCamHandle, CONTROL_USBTRAFFIC);
    if (QHYCCD_SUCCESS == rc)
    {
        rc = SetQHYCCDParam(pCamHandle, CONTROL_USBTRAFFIC, USB_TRAFFIC);
        if (QHYCCD_SUCCESS == rc)
        {
            fprintf(stderr, "SetQHYCCDParam CONTROL_USBTRAFFIC set to: %d, success.\n", USB_TRAFFIC);
        }
        else
        {
            fprintf(stderr, "SetQHYCCDParam CONTROL_USBTRAFFIC failure, error: %d\n", rc);
            getchar();
            return 1;
        }
    }

    // check speed
    rc = IsQHYCCDControlAvailable(pCamHandle, CONTROL_SPEED);
    if (QHYCCD_SUCCESS == rc)
    {
        rc = SetQHYCCDParam(pCamHandle, CONTROL_SPEED, USB_SPEED);
        if (QHYCCD_SUCCESS == rc)
        {
            fprintf(stderr, "SetQHYCCDParam CONTROL_SPEED set to: %d, success.\n", USB_SPEED);
        }
        else
        {
            fprintf(stderr, "SetQHYCCDParam CONTROL_SPEED failure, error: %d\n", rc);
            getchar();
            return 1;
        }
    }

    // check gain
    rc = IsQHYCCDControlAvailable(pCamHandle, CONTROL_GAIN);
    if (QHYCCD_SUCCESS == rc)
    {
        rc = SetQHYCCDParam(pCamHandle, CONTROL_GAIN, CHIP_GAIN);
        if (rc == QHYCCD_SUCCESS)
        {
            fprintf(stderr, "SetQHYCCDParam CONTROL_GAIN set to: %d, success\n", CHIP_GAIN);
        }
        else
        {
            fprintf(stderr, "SetQHYCCDParam CONTROL_GAIN failure, error: %d\n", rc);
            getchar();
            return 1;
        }
    }

    // check offset
    rc = IsQHYCCDControlAvailable(pCamHandle, CONTROL_OFFSET);
    if (QHYCCD_SUCCESS == rc)
    {
        rc = SetQHYCCDParam(pCamHandle, CONTROL_OFFSET, CHIP_OFFSET);
        if (QHYCCD_SUCCESS == rc)
        {
            fprintf(stderr, "SetQHYCCDParam CONTROL_GAIN set to: %d, success.\n", CHIP_OFFSET);
        }
        else
        {
            fprintf(stderr, "SetQHYCCDParam CONTROL_GAIN failed.\n");
            getchar();
            return 1;
        }
    }

    // set image resolution
    rc = SetQHYCCDResolution(pCamHandle, roiStartX, roiStartY, roiSizeX, roiSizeY);
    if (QHYCCD_SUCCESS == rc)
    {
        fprintf(stderr, "SetQHYCCDResolution roiStartX x roiStartY: %d x %d\n", roiStartX, roiStartY);
        fprintf(stderr, "SetQHYCCDResolution roiSizeX  x roiSizeY : %d x %d\n", roiSizeX, roiSizeY);
    }
    else
    {
        fprintf(stderr, "SetQHYCCDResolution failure, error: %d\n", rc);
        return 1;
    }

    rc = IsQHYCCDControlAvailable(pCamHandle, CONTROL_TRANSFERBIT);
    if(rc == QHYCCD_SUCCESS)
    {
        rc = SetQHYCCDBitsMode(pCamHandle, 8);
        if(rc != QHYCCD_SUCCESS)
        {
            fprintf(stderr, "SetQHYCCDParam CONTROL_TRANSFERBIT failed\n");
            return 1;
        }
    }

    // set binning mode
    rc = SetQHYCCDBinMode(pCamHandle, camBinX, camBinY);
    if (QHYCCD_SUCCESS == rc)
    {
        fprintf(stderr, "SetQHYCCDBinMode set to: binX: %d, binY: %d, success.\n", camBinX, camBinY);
    }
    else
    {
        fprintf(stderr, "SetQHYCCDBinMode failure, error: %d\n", rc);
        return 1;
    }

    // Query GPS
    rc = IsQHYCCDControlAvailable(pCamHandle, CAM_GPS);
    if (QHYCCD_SUCCESS == rc)
    {
        fprintf(stderr, "GPS available\n");

        double gpsStatus = GetQHYCCDParam(pCamHandle, CAM_GPS);
        fprintf(stderr, "GPS status: %d\n", (int)gpsStatus);
        
        rc = SetQHYCCDParam(pCamHandle, CAM_GPS, 1);
        if (QHYCCD_SUCCESS == rc)
        {
            gpsStatus = GetQHYCCDParam(pCamHandle, CAM_GPS);
            fprintf(stderr, "GPS status: %d\n", (int)gpsStatus);
        }
        else
        {
            fprintf(stderr, "Could not set GPS mode, error: %d\n", rc);
            return 1;
        }
    }
    else
    {
        printf("GPS not available\n");
    }

    // get requested memory lenght
    uint32_t length = GetQHYCCDMemLength(pCamHandle);

    if (length > 0)
    {
        pImgData = new unsigned char[length];
        memset(pImgData, 0, length);
        fprintf(stderr, "Allocated memory for frame: %d [uchar].\n", length);
    }
    else
    {
        fprintf(stderr, "Cannot allocate memory for frame.\n");
        return 1;
    }

    rc = BeginQHYCCDLive(pCamHandle);
    if (rc != QHYCCD_SUCCESS)
    {
        fprintf(stderr, "BeginQHYCCDLive failed: %d", rc);
    }

    fprintf(stderr, "Press any key to exit...\n");

    // Video Frame
    std::thread t(&videoThread, pCamHandle, pImgData);

    // wait for user key
    std::getchar();

    if (!exit_thread)
    {
        exit_thread = true;
        t.join();
    }

    StopQHYCCDLive(pCamHandle);
    SetQHYCCDStreamMode(pCamHandle, 0x0);

    // close camera handle
    rc = CloseQHYCCD(pCamHandle);
    if (QHYCCD_SUCCESS == rc)
    {
        fprintf(stderr, "Close QHYCCD success.\n");
    }
    else
    {
        fprintf(stderr, "Close QHYCCD failure, error: %d\n", rc);
    }

    // release sdk resources
    rc = ReleaseQHYCCDResource();
    if (QHYCCD_SUCCESS == rc)
    {
        fprintf(stderr, "SDK resources released.\n");
    }
    else
    {
        fprintf(stderr, "Cannot release SDK resources, error %d.\n", rc);
        return 1;
    }

    return 0;
}
