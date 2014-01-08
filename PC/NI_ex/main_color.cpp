#include <OpenNI.h>
#include <opencv2\opencv.hpp>
#include <vector>

void main_color()
{
    try {
        openni::OpenNI::initialize();

        openni::Device device;
        auto ret = device.open( openni::ANY_DEVICE );
        if ( ret != openni::STATUS_OK ) {
            throw std::runtime_error( "" );
        }

        openni::VideoStream colorStream;
        colorStream.create( device, openni::SensorType::SENSOR_COLOR );
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
                    colorImage = cv::Mat( colorStream.getVideoMode().getResolutionY(),
                                          colorStream.getVideoMode().getResolutionX(),
                                          CV_8UC3, (char*)colorFrame.getData() );
                }

                cv::cvtColor( colorImage, colorImage, CV_BGR2RGB );
                cv::imshow( "Color Camera", colorImage );

                int key = cv::waitKey( 10 );
                if ( key == 'q' ) {
                    break;
                }
            }
        }
    }
    catch ( std::exception& ) {
        std::cout << openni::OpenNI::getExtendedError() << std::endl;
    }
}