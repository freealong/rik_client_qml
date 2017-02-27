#include <iostream>
#include <sstream>
#include <fstream>
#include <cv.h>
#include <highgui.h>
#include "NFunc.h"

#include "cvRun.h"

using namespace cv;
using namespace std;
using namespace NFunc;

int cv_run(int argc, char* argv) {

  if(argc < 2) {
    help();
    return 0;
  }

  std::cout << "Starting..." << std::endl;
  const char* sysPara = argv;
  if( strcmp(sysPara, "-h") == 0 ) {
    help();
  } else if( strcmp(sysPara, "-v") == 0 ){      //-- Show video.

    VideoCapture video(0);
    //VideoCapture video(1);

    showVideo(video);

  } else if ( strcmp(sysPara, "-vc") == 0 ) {    //-- Show corners in video.

    VideoCapture video(0);
    //VideoCapture video(1);

    showCorners(video);

  } else if ( strcmp(sysPara, "-vp") == 0 ) {    //-- Show processed image and video.

    // VideoCapture video(0);
    VideoCapture video(1);

    showGrid(video);

  }
//  else if ( strcmp(sysPara, "-p") == 0) {    //-- Deal with image.

//    if( argc==2 ) {
//      cerr << "An image should be provided!" <<endl;
//      return 0;
//    } else if( argc > 3) {
//      cerr << "One image each time!" <<endl;
//      return 0;
//    }

//    const char* imageDir  = argv[2];
//    Mat src = imread(argv[2], 1);
//    vector<double> pos;
//    dealWithImage(src, pos, "src");

//  }
//  else if ( strcmp(sysPara, "-all") ==0 ) {    //-- Deal with images under the same dir.
//    if( argc==2 ) {
//      cerr << "A directory should be provided!" <<endl;
//      return 0;
//    } else if( argc > 3) {
//      cerr << "One directory each time!" <<endl;
//      return 0;
//    }

//    string imageDir(argv[2]);
//    dealWithBatches(imageDir);

//  } else if ( strcmp(sysPara, "-rot") == 0 ) {
//    if( argc==2 ) {
//      cerr << "An image should be provided!" <<endl;
//      return 0;
//    } else if( argc > 3) {
//      cerr << "One image each time!" <<endl;
//      return 0;
//    }
//    const char* imageDir  = argv[2];
//    Mat src = imread(argv[2], 1);

//    dealWithRotImage(src);


//  }
  else {
    cerr << "Unrecognized parameters!" <<endl;
    help();
  }

  return 0;
}
