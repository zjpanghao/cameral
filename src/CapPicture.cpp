/*
* Copyright(C) 2010,Hikvision Digital Technology Co., Ltd 
* 
* File   name£ºCapPicture.cpp
* Discription£º
* Version    £º1.0
* Author     £ºpanyd
* Create Date£º2010_3_25
* Modification History£º
*/

#include "public.h"
#include "CapPicture.h"
#include <stdio.h>
#include "stdlib.h"
#include "unistd.h"
#include "time.h"
#include <string>
#include <set>
#include <vector>
#include <string.h>
#include "json/json.h"
#include "glog/logging.h"
#include <fstream>
#include "base64.h"
#include "face.h"
#define MAX_IMAGE_BUF_LEN (1024 * 1024)
class CameralInfo {
  public:
    CameralInfo(std::string ip, int id) {
       id_ = id;
       ip_ = ip;
    }
      
    std::string getIp() const {
      return ip_;
    }
    int getId() const {
       return id_;
    }

    void setNum(int num) {
      num_ = num;
    }

    int getNum() {

      return num_;
    }

    bool operator <(const CameralInfo &info) const {
      return id_ < info.getId();
    }

    char *getImageBuf() {
      return data_;
    }
    
    int getImageBufLen() {
       return dataLen_;
    }

    void setImageBufLen(int len) {
       dataLen_ = len;
    }

  private:
    int id_;
    std::string ip_;
    int num_;
    char data_[MAX_IMAGE_BUF_LEN];
    int dataLen_;
};

class Cameral {
  public:
    void addInfo(std::string ip, int id) {
      CameralInfo cameralInfo(ip, id);
      info_.insert(cameralInfo);
    }

    std::set<CameralInfo> getInfo() {
      return info_;
    }

    void clear() {
      info_.clear();
    }
    
  private:
   std::set<CameralInfo> info_;
};

class CameralManager {
  public:
    static Cameral& getCameral() {
       static Cameral cameral;
       return cameral;
    }
};

static void readConfig() {
    std::ifstream ifs;
    ifs.open("camera.json");
 
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(ifs, root, false)) {
        printf("camera file error\n");
        return;
    }

    for (int i =0; i < root.size(); i++) {
      Json::Value cameral = root[i];
      std::string ip = cameral["ip"].asString();
      printf("%s\n", ip.c_str());
      int index = ip.rfind(".");
      if (index != -1) {
        std::string ipString = ip.substr(index + 1);
        CameralManager::getCameral().addInfo(ip, atoi(ipString.c_str()));
      }
    }
 
    ifs.close();
}

std::string getSerial(const CameralInfo &info, std::string &data) {
  Json::Value root;
  root["cameralIp"] = info.getIp();
  root["data"] = data;
  return root.toStyledString();
}

void* captureThread(void *param) {
  srand(time(NULL));
  CameralInfo *info = (CameralInfo*)param;
   //login
  NET_DVR_DEVICEINFO_V30 struDeviceInfo;
  char name[30];
  strcpy(name, info->getIp().c_str());
  long lUserID = 0;
  NET_DVR_DEVICEINFO_V40 lpDeviceInfo;
  
  while (1) {

    NET_DVR_USER_LOGIN_INFO pLoginInfo;
    strcpy(pLoginInfo.sUserName,"admin");
    strcpy(pLoginInfo.sDeviceAddress, info->getIp().c_str());
    strcpy(pLoginInfo.sPassword, "ky221data");
    pLoginInfo.wPort = 8000;

    
    lUserID = NET_DVR_Login_V40(&pLoginInfo, &lpDeviceInfo);
    if (lUserID < 0){
      printf("pyd  %s---Login error, %d\n",info->getIp().c_str(), NET_DVR_GetLastError());
      sleep(5);
      continue;
    }

    printf("pyd  %s---Login success, %d\n",info->getIp().c_str(), NET_DVR_GetLastError());
    NET_DVR_JPEGPARA strPicPara = {0};
    strPicPara.wPicQuality = 0;
    strPicPara.wPicSize = 0;
    int iRet;
   // printf("channel %d\n", lpDeviceInfo struDeviceInfo.byStartChan);
     printf("channel %d\n", lpDeviceInfo.struDeviceV30.byStartChan);
    char buf[64];
    snprintf(buf, sizeof(buf), "./%d/%d.jpg", info->getId(), rand()%1000 + 1000); 
    unsigned int len;
    iRet = NET_DVR_CaptureJPEGPicture_NEW(lUserID, 
      lpDeviceInfo.struDeviceV30.byStartChan, &strPicPara, info->getImageBuf(), MAX_IMAGE_BUF_LEN, &len);
    if (!iRet){
        printf("pyd%s---NET_DVR_CaptureJPEGPicture error, %d\n", 
          info->getIp().c_str(), NET_DVR_GetLastError());
        info->setImageBufLen(0);
        NET_DVR_Logout(lUserID);
        sleep(5);
        continue;
    }
    info->setImageBufLen((int)len);
    LOG(INFO) << "recv data len" << len;
    if (len > 0) {
      std::vector<unsigned char> imageVec(info->getImageBuf(), info->getImageBuf() + len);
      int faceNumber = faceNum(imageVec);
      LOG(INFO) <<"The facenum is : " << faceNumber;
      if (faceNumber > 0) {
        std::string imageDataBase64;
	base64Encry(info->getImageBuf(), len, &imageDataBase64);
	std::string serial = getSerial(*info, imageDataBase64);
	LOG(INFO) << serial;
	KafkaService::getKafkaProducer()->Send((char*)serial.c_str(), serial.length());
      }
    }
    NET_DVR_Logout(lUserID);
    sleep(10);
  }

  return NULL;

}

/*******************************************************************
      Function:   Demo_Capture
   Description:   Capture picture.
     Parameter:   (IN)   none 
        Return:   0--success£¬-1--fail.   
**********************************************************************/
int Demo_Capture() {
    printf("begin login capture\n");
    readConfig();
    
    NET_DVR_Init();
    NET_DVR_SetConnectTime(6000, 3);
    NET_DVR_SetReconnect(30000, true);
    NET_DVR_SetRecvTimeOut(20000);
  
    std::set<CameralInfo> ips = CameralManager::getCameral().getInfo();
    std::set<CameralInfo>::iterator it = ips.begin();
    time_t now = time(NULL);
    while (it != ips.end()) {
      pthread_t pid_t;
      const CameralInfo &cameraInfo = *it;
      pthread_create(&pid_t, NULL, captureThread, (void*) (&cameraInfo));
      it++;
    }
    
   while (1) {
     sleep(1);
   }
   NET_DVR_Cleanup();
   return HPR_OK;
}
