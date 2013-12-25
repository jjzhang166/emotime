/*
 * agui.hpp
 * Copyright (C) 2013 stk <stk@101337>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef AGUI_HPP
#define AGUI_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <opencv2/opencv.hpp>
#include "capture.hpp"
#include "emo_detector.hpp"
#include "preprocessor.hpp"

using namespace std;
using namespace cv;
using namespace emotime;

namespace emotime{

  template <class D>
  class ADebugGui{
  
    protected:
      FacePreProcessor * preprocessor;
      EmoDetector<D> * detector;
      ACapture * capture;
      string mainWinTitle;
      string faceWinTitle;
      string featsWinTitle;
      int fps;
    public:
      ADebugGui(ACapture * capt, FacePreProcessor * fp, EmoDetector<D> * detect, int fps){
        capture=capt;
        preprocessor= fp;
        detector=detect;
        mainWinTitle=string("AGui: Main Emotime Debug GUI");
        faceWinTitle=string("AGui: Face");
        featsWinTitle=string("AGui: Features");
        this->fps=fps;
      }
      bool init(){
	       namedWindow(mainWinTitle.c_str(), WINDOW_NORMAL);
	       //namedWindow(faceWinTitle.c_str(), CV_WINDOW_AUTOSIZE);
	       namedWindow(featsWinTitle.c_str(), CV_WINDOW_AUTOSIZE);
      }
      bool nextFrame(){
        Mat frame,copy;
        Mat featvector;
        if (capture->nextFrame(frame)){
          frame.copyTo(copy);
          if (preprocessor->preprocess(frame,featvector)){
            pair<Emotion,float> prediction=detector->predict(featvector);
            stringstream ss;
            ss<<"Emotion="<<emotionStrings(prediction.first)<<", Score="<<prediction.second;
            string osd = ss.str();
            putText(frame, osd.c_str(), Point(80,60), FONT_HERSHEY_SIMPLEX, 0.7, Scalar::all(0));
            // QT only
            //displayOverlay(mainWinTitle.c_str(), osd.c_str(), 2000);
            imshow(mainWinTitle.c_str(), frame);
            Mat face;
            if(preprocessor->extractFace(copy,face)){
              Mat gabor;
              if (preprocessor->filterImage(face,gabor)){
                double min;
                double max;
                cv::minMaxIdx(gabor, &min, &max);
                cv::Mat adjMap;
                cv::convertScaleAbs(gabor, adjMap, 255 / max);
                Mat bigger;
                resize(adjMap,bigger,Size(adjMap.size().width*3,adjMap.size().height*3), 0, 0, CV_INTER_LINEAR);
                imshow(featsWinTitle.c_str(), bigger);
              }
            
            }

          }
          return true;
       } else { 
         return false;
       }

      }
      bool run(){
        init();
        while (nextFrame()){
          if (fps<=0){
            waitKey(0);
          } else {
            waitKey((int) (1.0/fps));
          }
        }
      } 
  };
}


#endif /* !AGUI_HPP */

