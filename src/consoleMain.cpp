 /*
* Copyright(C) 2010,Hikvision Digital Technology Co., Ltd 
* 
* File   name：consoleMain.cpp
* Discription：
* Version    ：1.0
* Author     ：panyadong
* Create Date：2010_3_25
* Modification History：
*/

#ifndef __APPLE__


#include <stdio.h>
#include <iostream>
#include "GetStream.h"
#include "public.h"
#include "ConfigParams.h"
#include "Alarm.h"
#include "CapPicture.h"
#include "playback.h"
#include "Voice.h"
#include "tool.h"
#include <glog/logging.h>

using namespace std;

static void initGlog(const char *name) {
  google::InitGoogleLogging(name);
  google::SetLogDestination(google::INFO,"log/cameraloginfo");
  google::SetLogDestination(google::WARNING,"log/cameralogwarn");
  google::SetLogDestination(google::GLOG_ERROR,"log/cameralogerror");
  FLAGS_logbufsecs = 10;
}

static void initKafka() {
  KafkaProducer *producer = KafkaService::getKafkaProducer();
  producer->Init("192.168.1.106:9092", "cameral", "cameral_group");
}

int main()
{
    initGlog("camera");
    initKafka();
    NET_DVR_Init();
    Demo_SDK_Version();
    NET_DVR_SetLogToFile(3, "./sdkLog");
    char cUserChoose = 'r';

    // 从配置文件读取设备信息 
    
    //Login device
    NET_DVR_DEVICEINFO_V30 struDeviceInfo = {0};
    //LONG lUserID = NET_DVR_Login_V30("172.9.204.5", 8000, "admin", "12345", &struDeviceInfo);
	//LONG lUserID = NET_DVR_Login_V30("192.168.2.3", 8000, "admin", "ky221data", &struDeviceInfo);
  //  if (lUserID < 0)
  #if 0
    {
        printf("pyd---Login error, %d\n", NET_DVR_GetLastError());
        printf("Press any key to quit...\n");
        cin>>cUserChoose;

        NET_DVR_Cleanup();
        return HPR_ERROR;
    }
  #endif

    

    while ('q' != cUserChoose)
    {
        printf("\n");
        printf("Input 1, Test GetStream\n");
        printf("      2, Test Configure params\n");
        printf("      3, Test Alarm\n");
        printf("      4, Test Capture Picture\n");
        printf("      5, Test play back\n");
        printf("      6, Test Voice\n");
        printf("      7, Test SDK ability\n");
        printf("      8, Test tool interface\n");
		/*
        printf("      7, Test Matrix decode\n");
        printf("      8, Test PTZ\n");
        printf("      9, Test Format\n");
        printf("      0, Test Update\n");
        printf("      a, Test Serial trans\n");
        printf("      b, Test Configure Params\n");
        printf("      c, Test VCA && IVMS\n");
        */
        printf("      q, Quit.\n");
        printf("Input:");

        cin>>cUserChoose;
        switch (cUserChoose)
        {
        case '1':
          //  Demo_GetStream_V30(lUserID); //Get stream.
            break;
        case '2':
           // Demo_ConfigParams(lUserID);  //Setting params.
            break;
        case '3':
            Demo_Alarm();         //Alarm & listen.
            break;
        case '4':
            Demo_Capture();
            break;
        case '5':
          //  Demo_PlayBack((int)lUserID);     //record & playback
            break;
        case '6':
            Demo_Voice();
            break;
        case '7':
            Demo_SDK_Ability();
            break;
		case '8':
			Demo_DVRIPByResolveSvr();
			break;
        default:
            break;
        }
    }

    //logout
    //NET_DVR_Logout_V30(lUserID);
    NET_DVR_Cleanup();
    return 0;
}

#endif
