#include <OpenNI.h>
#include <opencv2\opencv.hpp>
#include <vector>
#include "Main.hpp"

void main_depth()
{
    try {
        openni::OpenNI::initialize();

        openni::Device device;
        auto ret = device.open( openni::ANY_DEVICE );
        if ( ret != openni::STATUS_OK ) {
            throw std::runtime_error( "" );
        }

        openni::VideoStream depthStream;
        depthStream.create( device, openni::SensorType::SENSOR_DEPTH );
        depthStream.start();

        std::vector<openni::VideoStream*> streams;
        streams.push_back( &depthStream );

        cv::Mat depthImage;

        while ( 1 ) {
            int changedIndex;
            openni::OpenNI::waitForAnyStream( &streams[0], streams.size(), &changedIndex );
            if ( changedIndex == 0 ) {
                openni::VideoFrameRef depthFrame;
                depthStream.readFrame( &depthFrame );
                if ( depthFrame.isValid() ) {
                    depthImage = cv::Mat( depthStream.getVideoMode().getResolutionY(),
                                          depthStream.getVideoMode().getResolutionX(),
                                          CV_16U, (char*)depthFrame.getData() );

                    // 0-10000mmまでのデータを0-255にする
					depthImage.convertTo( depthImage, CV_8U, 255.0 / 10000 );
					
					cv::Mat colImage( depthImage.rows, depthImage.cols, CV_8UC3 );

					// RGB画像（24bit）画像
					//for(int y = 0 ; y < depthImage.rows; y++){
					//	for(int x = 0 ; x < depthImage.cols; x++){
					//		uchar gvar = depthImage.at<uchar>(y, x);
					//		colImage.at<cv::Vec3b>(y, x) = cv::Vec3b(gvar, gvar, gvar);
					//		//colImage.data[y * depthImage.cols + x * depthImage.step + 0] = 0;
					//		//colImage.data[y * depthImage.cols + x * depthImage.step + 1] = 255;
					//		//colImage.data[y * depthImage.cols + x * depthImage.step + 2] = 255;
					//	}
					//}

                    cv::imshow( "Depth Camera", depthImage );
                }
            }

            int key = cv::waitKey( 10 );
            if ( key == 'q' ) {
                break;
            }
        }
    }
    catch ( std::exception& ) {
        std::cout << openni::OpenNI::getExtendedError() << std::endl;
    }
}