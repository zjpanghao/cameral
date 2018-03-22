link_directories(/home/panghao/opencv-3.4.0/build/lib)
add_executable(face face.cpp)
include_directories(/home/panghao/opencv-3.4.0/modules/highgui/include 
                    /home/panghao/opencv-3.4.0/modules/core/include 
                    /home/panghao/opencv-3.4.0/modules/objdetect/include 
                    /home/panghao/opencv-3.4.0/modules/imgcodecs/include 
                    /home/panghao/opencv-3.4.0/modules/videoio/include 
                    /home/panghao/opencv-3.4.0/modules/imgproc/include 
                    /home/panghao/opencv-3.4.0/build)
target_link_libraries(face opencv_imgcodecs opencv_imgproc opencv_core opencv_objdetect)
