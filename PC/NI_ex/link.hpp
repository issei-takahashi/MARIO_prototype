#pragma once

/* OpenNI2関連 */
#pragma comment(lib,"C:\\OpenNI2\\Lib\\OpenNI2.lib")

#ifdef _DEBUG //Debugモードの場合
#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_core220d.lib")            // opencv_core
#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_imgproc220d.lib")        // opencv_imgproc
#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_highgui220d.lib")        // opencv_highgui
#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_objdetect220d.lib")    // opencv_objdetect
//以下、必要に応じて追加
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_ml220d.lib")            // opencv_ml
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_features2d220d.lib")    // opencv_features2d
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_video220d.lib")        // opencv_video
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_calib3d220d.lib")        // opencv_calib3d
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_flann220d.lib")        // opencv_flann
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_contrib220d.lib")        // opencv_contrib
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_legacy220d.lib")        // opencv_legacy
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_gpu220d.lib")            // opencv_gpu
#else //Releaseモードの場合
/* OpenCV2.2関連 */
#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_core220.lib")            // opencv_core
#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_imgproc220.lib")        // opencv_imgproc
#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_highgui220.lib")        // opencv_highgui
#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_objdetect220.lib")    // opencv_objdetect
//以下、必要に応じて追加
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_ml220.lib")            // opencv_ml
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_features2d220.lib")    // opencv_features2d
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_video220.lib")        // opencv_video
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_calib3d220.lib")        // opencv_calib3d
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_flann220.lib")        // opencv_flann
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_contrib220.lib")        // opencv_contrib
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_legacy220.lib")        // opencv_legacy
//#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_gpu220.lib")            // opencv_gpu
    
#endif