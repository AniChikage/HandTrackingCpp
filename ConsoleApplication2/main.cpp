// ConsoleApplication2.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "stdio.h"
#include "Kinect.h"       //Kinect��ͷ�ļ�  
#include <iostream>  
#include <windows.h>  
#include <opencv2\highgui.hpp>            //opencvͷ�ļ�  
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "vector"
#include "algorithm"
#include "math.h"
#include "map"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>    
#pragma comment ( lib, "kinect20.lib" )  

using namespace std;
using namespace cv;

#define outline_b 0
#define outline_g 0
#define outline_r 255
#define outline_border 2
#define cube_b 0
#define cube_g 255
#define cube_r 0

bool isOnHand(Vec4b a) {
	return a[0] == 255 && a[1] == 255 && a[2] == 255;
}

bool isOnOutline(Vec4b a) {
	return a[0] == outline_b && a[1] == outline_g && a[2] == outline_r;
}

double cmp(const pair<string, double>& x, const pair<string, double>& y)
{
	return x.second > y.second;
}

void sortMapByValue(map<string, double>& tMap, vector<pair<string, double> >& tVector)
{
	for (map<string, double>::iterator curr = tMap.begin(); curr != tMap.end(); curr++)
		tVector.push_back(make_pair(curr->first, curr->second));

	sort(tVector.begin(), tVector.end(), cmp);
}

vector<string> split(const string &s, const string &seperator) {
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		//�ҵ��ַ������׸������ڷָ�������ĸ��
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		//�ҵ���һ���ָ������������ָ���֮����ַ���ȡ����
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}

int main(void)
{

	//begin ���峣���ͱ���
	int minX = 0;
	int minY = 0;
	int maxX = 0;
	int maxY = 0;

	int temp_minX = 0;
	int temp_minY = 0;
	int temp_maxX = 0;
	int temp_maxY = 0;

	map<string, double> singlePixelDistance;
	map<string, double> globalPixelDistance;
	vector<pair<int, int>> outline_vec;
	//end
	
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

		//cout << a.channels() << endl;

		Mat dstImage1;
		resize(a, dstImage1, Size(a.cols / 3, a.rows / 3), 0, 0, INTER_LINEAR);
		//a.release();
		
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

		cv::Mat thresholded1;
		cv::threshold(dstImage1, thresholded1, 0, 255, cv::THRESH_BINARY);
		//dstImage1.release();

	    //imshow("aaa", a);
		//cv::imshow("Binary Image", thresholded);

		cv::Mat thresholded;
		Mat kernel = getStructuringElement(MORPH_CROSS, Size(3, 3), Point(-1, -1));
		dilate(thresholded1, thresholded, kernel);



		#pragma region ��ȡ�ֵ��������ұ߽� 

	    maxX = 0;
		maxY = 0;
		minX = thresholded.rows - 1;
		minY = thresholded.cols - 1;

		try {
			for (int row = thresholded.rows / 4; row < thresholded.rows *3 /4; ++row){
				for (int col = thresholded.cols / 4; col < thresholded.cols * 3 /4; ++col){
					if (isOnHand(thresholded.at<Vec4b>(row, col))) {
						minX = minX > row ? row : minX;
						maxX = maxX < row ? row : maxX;
						minY = minY > col ? col : minY;
						maxY = maxY < col ? col : maxY;
					}
				}
			}
		}
		catch (Exception ex) {
			cout << ex.what() << endl;
		}

		cout << minX << "," << maxX << "," << minY << "," << maxY << endl;
		minY = (minY + maxY) / 2;

		temp_minX = minX;
		temp_minY = minY;
		temp_maxX = maxX;
		temp_maxY = maxY;
		maxX = 0;
		maxY = 0;
		minX = thresholded.rows - 1;
		minY = thresholded.cols - 1;

		try {
			for (int row = temp_minX; row < temp_maxX; ++row){
				for (int col = temp_minY; col < temp_maxY; ++col){
					if (isOnHand(thresholded.at<Vec4b>(row, col))) {
						minX = minX > row ? row : minX;
						maxX = maxX < row ? row : maxX;
						minY = minY > col ? col : minY;
						maxY = maxY < col ? col : maxY;
					}
				}
			}
		}
		catch (Exception ex) {
			cout << ex.what() << endl;
		}
		#pragma endregion

		#pragma region ��ȡ����

		int cc = 0;

		//singlePixelDistance = new vector<int>();
		globalPixelDistance.clear();
		outline_vec.clear();

		#pragma omp parallel for
		for (int row = minX; row < maxX; ++row) {
			for (int col = minY; col < maxY; ++col) {
				if (!isOnHand(thresholded.at<Vec4b>(row, col)) && (
					isOnHand(thresholded.at<Vec4b>(row - 1, col)) ||
					isOnHand(thresholded.at<Vec4b>(row + 1, col)) ||
					isOnHand(thresholded.at<Vec4b>(row, col - 1)) ||
					isOnHand(thresholded.at<Vec4b>(row, col + 1))
					)) {
					outline_vec.push_back(pair<int ,int>(row, col));
				}
			}
		}

		#pragma omp parallel for
		for (int row = minX; row < maxX; ++row) {
			for (int col = minY; col < maxY; ++col) {
				if (isOnHand(thresholded.at<Vec4b>(row, col))) {
					singlePixelDistance.clear();
					int cur_min = 2 * (maxX + maxY);
					int cur_x = 0;
					int cur_y = 0;
					if (cc == 0) {
						cout << "cur_min: " << cur_min << endl;
						cc++;
					}
					
					vector<pair<int, int>>::iterator iter;
					iter = outline_vec.begin();
					while (iter != outline_vec.end()) {
						double realDis = sqrt(pow((row - iter->first), 2) + pow((col - iter->second), 2));
						if (cur_min > realDis) {
							cur_min = realDis;
						}
						iter++;
					}

					globalPixelDistance.insert(pair<string, double>(to_string(row)+","+to_string(col), cur_min));
				}
			}
		}
		//vector<pair<string, double>> tVector;
		//sortMapByValue(globalPixelDistance, tVector);

		map<string, double>::iterator iter;
		iter = globalPixelDistance.begin();
		double maxDis = 0;
		string maxStr = "0";
		int maxCorX = 0;
		int maxCorY = 0;
		while (iter != globalPixelDistance.end()) {
			double curDis = iter->second;
			string curStr = iter->first;
			if (curDis > maxDis) {
				maxDis = curDis;
				maxStr = curStr;
			}
			iter++;
		}

		cout << "maxstr: " << maxStr << ",maxdis: " << maxDis << endl;
		vector<string> destination;
		//boost::split(destination, maxStr, boost::is_any_of(","), boost::token_compress_off);

		destination = split(maxStr, ",");
		if (destination.size() == 2) {
			maxCorX = boost::lexical_cast<int>(destination[0]);
			maxCorY = boost::lexical_cast<int>(destination[1]);
		}
		//maxCorX = boost::lexical_cast<int>(destination[0]);
		//maxCorY = boost::lexical_cast<int>(destination[1]);

		#pragma endregion

		#pragma region ����

		for (int loopx = minX; loopx < maxX; ++loopx) {
			thresholded.at<Vec4b>(loopx, minY)[0] = 0;
			thresholded.at<Vec4b>(loopx, minY)[1] = 255;
			thresholded.at<Vec4b>(loopx, minY)[2] = 0;
			thresholded.at<Vec4b>(loopx, maxY)[0] = 0;
			thresholded.at<Vec4b>(loopx, maxY)[1] = 255;
			thresholded.at<Vec4b>(loopx, maxY)[2] = 0;
		}
		
		for (int loopy = minY; loopy < maxY; ++loopy) {
			thresholded.at<Vec4b>(minX, loopy)[0] = 0;
			thresholded.at<Vec4b>(minX, loopy)[1] = 255;
			thresholded.at<Vec4b>(minX, loopy)[2] = 0;
			thresholded.at<Vec4b>(maxX, loopy)[0] = 0;
			thresholded.at<Vec4b>(maxX, loopy)[1] = 255;
			thresholded.at<Vec4b>(maxX, loopy)[2] = 0;
		}

		for (int loopx = minX; loopx < maxX; ++loopx) {
			for (int loopy = minY; loopy < maxY; ++loopy) {
				if (
					isOnHand(thresholded.at<Vec4b>(loopx, loopy)) &&
					!isOnHand(thresholded.at<Vec4b>(loopx, loopy - outline_border)) &&
					!isOnOutline(thresholded.at<Vec4b>(loopx, loopy - outline_border)) ||
					isOnHand(thresholded.at<Vec4b>(loopx, loopy)) &&
					!isOnHand(thresholded.at<Vec4b>(loopx, loopy + outline_border))
					) {
					thresholded.at<Vec4b>(loopx, loopy)[0] = outline_b;
					thresholded.at<Vec4b>(loopx, loopy)[1] = outline_g;
					thresholded.at<Vec4b>(loopx, loopy)[2] = outline_r;
				}
			}
		}

		for (int theta = 0; theta < 360; ++theta) {
			if (destination.size() == 2) {
				thresholded.at<Vec4b>(maxCorX + maxDis*cos(theta), maxCorY + maxDis*sin(theta))[0] = outline_b;
				thresholded.at<Vec4b>(maxCorX + maxDis*cos(theta), maxCorY + maxDis*sin(theta))[1] = outline_g;
				thresholded.at<Vec4b>(maxCorX + maxDis*cos(theta), maxCorY + maxDis*sin(theta))[2] = outline_r;

				thresholded.at<Vec4b>(maxCorX , maxCorY)[0] = outline_b;
				thresholded.at<Vec4b>(maxCorX , maxCorY)[1] = outline_g;
				thresholded.at<Vec4b>(maxCorX , maxCorY)[2] = outline_r;
			}
		}

		#pragma endregion
		

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
	������ݵĴ���
	

	

	IKinectSensor   * mySensor = nullptr;
	GetDefaultKinectSensor(&mySensor);  //��ȡ��Ӧ��  
	mySensor->Open();           //�򿪸�Ӧ��  


	IDepthFrameSource   * mySource = nullptr;   //ȡ���������  
	mySensor->get_DepthFrameSource(&mySource);

    height = 0, width = 0;
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

