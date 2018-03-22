#include"opencv2/objdetect/objdetect.hpp"
#include"opencv2/highgui/highgui.hpp"  
#include"opencv2/imgproc/imgproc.hpp" 
#include<vector>
using namespace std;
  
using namespace cv;  
  
//人脸检测的类  
  
int faceNum(std::vector<unsigned char> &data)  
{  
    CascadeClassifier faceCascade;  
    faceCascade.load("/home/panghao/opencv-3.4.0/data/haarcascades/haarcascade_frontalface_alt2.xml");   
    //加载分类器，注意文件路径  
  
    //Mat img = imread("/home/panghao/timg.jpg");  
    if (data.size() == 0) {
      return -1;
    }
    Mat img = imdecode(data, 1);
    Mat imgGray;  
    vector<Rect> faces;  
  
    if(img.empty())  
    {  
      return -1;  
    }  
  
    if(img.channels() ==3)  
    {  
       cvtColor(img, imgGray, CV_RGB2GRAY);  
    }  
    else  
    {  
       imgGray = img;  
    }  
  
    faceCascade.detectMultiScale(imgGray, faces, 1.2, 6, 0, Size(0, 0));   //检测人脸  
    int num = 0;
    if((num = faces.size())>0)  
    {  
       printf("human face detected %d\n", num);
#if 0
       for(int i =0; i<faces.size(); i++)  
       {  
           rectangle(img, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height),   
                           Scalar(0, 255, 0), 1, 8);    //框出人脸位置  
       }  

#endif  
    }
    return num;  
}  

