// ConsoleApplication2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include <Kinect.h>       //Kinect的头文件  
#include <iostream>  
#include <windows.h>  
#include <opencv2\highgui.hpp>            //opencv头文件  
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#pragma comment ( lib, "kinect20.lib" )  

using   namespace   std;
using   namespace   cv;
int main(void)
{
	
	HRESULT hResult = S_OK;     //用于检测操作是否成功
	IKinectSensor *kinect;           //创建一个感应器
	GetDefaultKinectSensor(&kinect);
	kinect->Open();     //打开感应器
	IColorFrameSource *colorsource;
	IColorFrameReader *colorreader;
	IFrameDescription *colorde;
	kinect->get_ColorFrameSource(&colorsource);
	colorsource->OpenReader(&colorreader);
	colorsource->get_FrameDescription(&colorde);

	int width = 0;      //长和宽
	int height = 0;
	colorde->get_Height(&height);
	colorde->get_Width(&width);
	Mat a(height, width, CV_8UC4);      //注意：这里必须为4通道的图，Kinect的数据只能以Bgra格式传出
	namedWindow("main");

	cout << "矩阵大小";
	cout << a.size();


	while (1)
	{
		IColorFrame*frame;
		hResult = colorreader->AcquireLatestFrame(&frame);
		if (SUCCEEDED(hResult))
		{
			frame->CopyConvertedFrameDataToArray(height*width * 4, reinterpret_cast<BYTE*>(a.data), ColorImageFormat::ColorImageFormat_Bgra);   //传出数据
		}
		if (frame != NULL)   //释放
		{
			frame->Release();
			frame = NULL;
		}
		if (waitKey(30) == VK_ESCAPE)
			break;

		Mat dstImage1;
		resize(a, dstImage1, Size(a.cols / 3, a.rows / 3), 0, 0, INTER_LINEAR);
		
		for (int row = 0; row < dstImage1.rows; row++)
		{
			for (int col = 0; col < dstImage1.cols; col++)
			{
				//注意 Mat::at 函数是个模板函数, 需要指明参数类型, 因为这张图是具有红蓝绿三通道的图,
				//所以它的参数类型可以传递一个 Vec3b, 这是一个存放 3 个 uchar 数据的 Vec(向量). 这里
				//提供了索引重载, [2]表示的是返回第三个通道, 在这里是 Red 通道, 第一个通道(Blue)用[0]返回 BGR 
				if (dstImage1.at<Vec4b>(row, col)[2] > 95 &&
					dstImage1.at<Vec4b>(row, col)[2] > 40 &&
					dstImage1.at<Vec4b>(row, col)[2] > 20 &&
					dstImage1.at<Vec4b>(row, col)[2] - dstImage1.at<Vec4b>(row, col)[0] > 15 &&
					dstImage1.at<Vec4b>(row, col)[2] - dstImage1.at<Vec4b>(row, col)[1] > 15)
				{
					dstImage1.at<Vec4b>(row, col) = Vec4b(36, 100, 254, 0);
				}
				else {
					dstImage1.at<Vec4b>(row, col) = Vec4b(0, 0, 0, 0);
				}

			}
		}

		/*
		for (int row = 0; row < a.rows; row++)
		{
			for (int col = 0; col < a.cols; col++)
			{
				//注意 Mat::at 函数是个模板函数, 需要指明参数类型, 因为这张图是具有红蓝绿三通道的图,
				//所以它的参数类型可以传递一个 Vec3b, 这是一个存放 3 个 uchar 数据的 Vec(向量). 这里
				//提供了索引重载, [2]表示的是返回第三个通道, 在这里是 Red 通道, 第一个通道(Blue)用[0]返回 BGR 
				if (a.at<Vec4b>(row, col)[2] > 95 &&
					a.at<Vec4b>(row, col)[2] > 40 &&
					a.at<Vec4b>(row, col)[2] > 20 &&
					a.at<Vec4b>(row, col)[2] - a.at<Vec4b>(row, col)[0] > 15 &&
					a.at<Vec4b>(row, col)[2] - a.at<Vec4b>(row, col)[1] > 15) 
				{
					a.at<Vec4b>(row, col) = Vec4b(36, 100, 254, 0);
				}
				else {
					a.at<Vec4b>(row, col) = Vec4b(0, 0, 0, 0);
				}
				
			}
		}
		*/

		/*
		for (int row = 0; row < quanter_a.rows; row++)
		{
			for (int col = 0; col < quanter_a.cols; col++)
			{
				quanter_a.at<Vec4d>(row, col) = Vec4b(255,0,0,0);
			}
		}
		*/

		cv::Mat thresholded;
		cv::threshold(dstImage1, thresholded, 90, 255, cv::THRESH_BINARY);

	    //imshow("aaa", a);
		//cv::imshow("Binary Image", thresholded);

		Mat kernel = getStructuringElement(MORPH_CROSS, Size(3, 3), Point(-1, -1));
		dilate(thresholded, thresholded, kernel);

		//Mat dstImage1;
		//resize(thresholded, dstImage1, Size(thresholded.cols / 2, thresholded.rows / 2), 0, 0, INTER_LINEAR);

		cv::imshow("main", thresholded);

	}
	if (colorsource != NULL)    //全部释放掉
	{
		colorsource->Release();
		colorsource = NULL;
	}
	if (colorreader != NULL)
	{
		colorreader->Release();
		colorreader = NULL;
	}
	if (colorde != NULL)
	{
		colorde->Release();
		colorde = NULL;
	}
	if (kinect)
	{
		kinect->Close();
	}
	if (kinect != NULL)
	{
		kinect->Release();
		kinect = NULL;
	}
	destroyAllWindows();

	/*

	IKinectSensor   * mySensor = nullptr;
	GetDefaultKinectSensor(&mySensor);  //获取感应器  
	mySensor->Open();           //打开感应器  


	IDepthFrameSource   * mySource = nullptr;   //取得深度数据  
	mySensor->get_DepthFrameSource(&mySource);

	int height = 0, width = 0;
	IFrameDescription   * myDescription = nullptr;  //取得深度数据的分辨率  
	mySource->get_FrameDescription(&myDescription);
	myDescription->get_Height(&height);
	myDescription->get_Width(&width);
	myDescription->Release();

	IDepthFrameReader   * myReader = nullptr;
	mySource->OpenReader(&myReader);    //打开深度数据的Reader  

	IDepthFrame * myFrame = nullptr;
	Mat temp(height, width, CV_16UC1);    //建立图像矩阵  
	Mat img(height, width, CV_8UC1);
	while (1)
	{
		if (myReader->AcquireLatestFrame(&myFrame) == S_OK) //通过Reader尝试获取最新的一帧深度数据，放入深度帧中,并判断是否成功获取  
		{
			myFrame->CopyFrameDataToArray(height * width, (UINT16 *)temp.data); //先把数据存入16位的图像矩阵中  
			temp.convertTo(img, CV_8UC1, 255.0 / 4500);   //再把16位转换为8位  
			imshow("TEST", img);
			myFrame->Release();
		}
		if (waitKey(30) == VK_ESCAPE)
			break;
	}
	myReader->Release();        //释放不用的变量并且关闭感应器  
	mySource->Release();
	mySensor->Close();
	mySensor->Release();
	*/

	return  0;
}