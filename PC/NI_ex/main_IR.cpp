// IR画像を表示する
#include <OpenNI.h>
#include <opencv2\opencv.hpp>
#include <vector>

void main_IR()
{
    try {
        openni::OpenNI::initialize();

        openni::Device device;
        auto ret = device.open( openni::ANY_DEVICE );
        if ( ret != openni::STATUS_OK ) {
            throw std::runtime_error( "" );
        }

        openni::VideoStream colorStream;
        colorStream.create( device, openni::SensorType::SENSOR_IR );
        colorStream.start();

        std::vector<openni::VideoStream*> streams;
        streams.push_back( &colorStream );

        cv::Mat colorImage;

        while ( 1 ) {
            int changedIndex;
            openni::OpenNI::waitForAnyStream( &streams[0], streams.size(), &changedIndex );
            if ( changedIndex == 0 ) {
                openni::VideoFrameRef colorFrame;
                colorStream.readFrame( &colorFrame );
                if ( colorFrame.isValid() ) {
                    if ( colorFrame.getVideoMode().getPixelFormat() ==
                      openni::PixelFormat::PIXEL_FORMAT_GRAY16 ) {
                        // XitonのIRのフォーマットは16bitグレースケール
                        // 実際は255諧調らしく、CV_8Uに落とさないと見えない
                        colorImage = cv::Mat( colorStream.getVideoMode().getResolutionY(), 
                            colorStream.getVideoMode().getResolutionX(),
                            CV_16U, (char*)colorFrame.getData() );
                        colorImage.convertTo( colorImage, CV_8U );
                    }
                    else {
                        // KinectのIRのフォーマットは8bitグレースケール
                        // Kinect SDKは16bitグレースケール
                        colorImage = cv::Mat( colorStream.getVideoMode().getResolutionY(), 
                            colorStream.getVideoMode().getResolutionX(),
                            CV_8U, (char*)colorFrame.getData() );
                    }

                    cv::imshow( "IR Image", colorImage );
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