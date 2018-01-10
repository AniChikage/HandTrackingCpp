// ConsoleApplication2.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "stdio.h"
#include <Kinect.h>       //Kinect��ͷ�ļ�  
#include <iostream>  
#include <windows.h>  
#include <opencv2\highgui.hpp>            //opencvͷ�ļ�  
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#pragma comment ( lib, "kinect20.lib" )  

using   namespace   std;
using   namespace   cv;
int main(void)
{
	
	HRESULT hResult = S_OK;     //���ڼ������Ƿ�ɹ�
	IKinectSensor *kinect;           //����һ����Ӧ��
	GetDefaultKinectSensor(&kinect);
	kinect->Open();     //�򿪸�Ӧ��
	IColorFrameSource *colorsource;
	IColorFrameReader *colorreader;
	IFrameDescription *colorde;
	kinect->get_ColorFrameSource(&colorsource);
	colorsource->OpenReader(&colorreader);
	colorsource->get_FrameDescription(&colorde);

	int width = 0;      //���Ϳ�
	int height = 0;
	colorde->get_Height(&height);
	colorde->get_Width(&width);
	Mat a(height, width, CV_8UC4);      //ע�⣺�������Ϊ4ͨ����ͼ��Kinect������ֻ����Bgra��ʽ����
	namedWindow("main");

	cout << "�����С";
	cout << a.size();


	while (1)
	{
		IColorFrame*frame;
		hResult = colorreader->AcquireLatestFrame(&frame);
		if (SUCCEEDED(hResult))
		{
			frame->CopyConvertedFrameDataToArray(height*width * 4, reinterpret_cast<BYTE*>(a.data), ColorImageFormat::ColorImageFormat_Bgra);   //��������
		}
		if (frame != NULL)   //�ͷ�
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
				//ע�� Mat::at �����Ǹ�ģ�庯��, ��Ҫָ����������, ��Ϊ����ͼ�Ǿ��к�������ͨ����ͼ,
				//�������Ĳ������Ϳ��Դ���һ�� Vec3b, ����һ����� 3 �� uchar ���ݵ� Vec(����). ����
				//�ṩ����������, [2]��ʾ���Ƿ��ص�����ͨ��, �������� Red ͨ��, ��һ��ͨ��(Blue)��[0]���� BGR 
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
				//ע�� Mat::at �����Ǹ�ģ�庯��, ��Ҫָ����������, ��Ϊ����ͼ�Ǿ��к�������ͨ����ͼ,
				//�������Ĳ������Ϳ��Դ���һ�� Vec3b, ����һ����� 3 �� uchar ���ݵ� Vec(����). ����
				//�ṩ����������, [2]��ʾ���Ƿ��ص�����ͨ��, �������� Red ͨ��, ��һ��ͨ��(Blue)��[0]���� BGR 
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
	if (colorsource != NULL)    //ȫ���ͷŵ�
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
	GetDefaultKinectSensor(&mySensor);  //��ȡ��Ӧ��  
	mySensor->Open();           //�򿪸�Ӧ��  


	IDepthFrameSource   * mySource = nullptr;   //ȡ���������  
	mySensor->get_DepthFrameSource(&mySource);

	int height = 0, width = 0;
	IFrameDescription   * myDescription = nullptr;  //ȡ��������ݵķֱ���  
	mySource->get_FrameDescription(&myDescription);
	myDescription->get_Height(&height);
	myDescription->get_Width(&width);
	myDescription->Release();

	IDepthFrameReader   * myReader = nullptr;
	mySource->OpenReader(&myReader);    //��������ݵ�Reader  

	IDepthFrame * myFrame = nullptr;
	Mat temp(height, width, CV_16UC1);    //����ͼ�����  
	Mat img(height, width, CV_8UC1);
	while (1)
	{
		if (myReader->AcquireLatestFrame(&myFrame) == S_OK) //ͨ��Reader���Ի�ȡ���µ�һ֡������ݣ��������֡��,���ж��Ƿ�ɹ���ȡ  
		{
			myFrame->CopyFrameDataToArray(height * width, (UINT16 *)temp.data); //�Ȱ����ݴ���16λ��ͼ�������  
			temp.convertTo(img, CV_8UC1, 255.0 / 4500);   //�ٰ�16λת��Ϊ8λ  
			imshow("TEST", img);
			myFrame->Release();
		}
		if (waitKey(30) == VK_ESCAPE)
			break;
	}
	myReader->Release();        //�ͷŲ��õı������ҹرո�Ӧ��  
	mySource->Release();
	mySensor->Close();
	mySensor->Release();
	*/

	return  0;
}