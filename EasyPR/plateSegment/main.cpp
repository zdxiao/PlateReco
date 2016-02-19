/*******************************************************************/
/*
				Author:		    Qi Gao
				Creat Date:	    2016/02/19
				Email:		    gaoqi@mail.bnu.edu.cn
				Last Modify:	2016/02/19
*/
/*********************************************************************/

#include <iostream>
#include <fstream>

#include "easypr/core/chars_segment.h"

using namespace std;
using namespace cv;
using namespace easypr;

void combineImg(Mat plate, vector<Mat> blocks, Mat& result)
{
	Mat plate_grey;
	cvtColor(plate, plate_grey, CV_BGR2GRAY);

	result.create(plate_grey.rows*2+2, plate_grey.cols+9,CV_8UC1);
	int startW = 1, startH = 1;

	Mat imgROI = result(Rect(startW, startH, plate_grey.cols, plate_grey.rows));
	plate_grey.copyTo(imgROI);

	startH += plate_grey.rows;
	size_t idx = blocks.size();
	int additionW = plate_grey.cols / idx;
	for (auto it = blocks.begin(); it != blocks.end(); it++)
	{
		imgROI = result(Rect(startW, startH, additionW, plate_grey.rows));
		resize(*it, imgROI, Size(additionW, plate_grey.rows));
		
		startW += additionW+1;
	}
}

bool judgeSegment(Mat img)
{
	vector<Mat> resultVec;
	CCharsSegment plate;

	plate.setDebug(1);

	int result = plate.charsSegment(img, resultVec);
	if (result == 0) {
		Mat res;
		combineImg(img, resultVec, res);
		
		imshow("chars_segment", res);
		
		char c = waitKey(0) & 255;
		destroyWindow("chars_segment");
		cout << c << endl;
		if (c == 'a')
		{
			cout << "success" << endl;
			return true;
		}
	}
	return false;
}

void testSeg()
{
	string rootDir("A:\\WorkPlace\\CPLUS\\TN_ALPR\\seg_test_160219");
	Mat img, imgH;
	ifstream infile(rootDir + "\\list.txt");
	string filename;
	if (!infile)
	{
		cout << "open list file error" << endl;
	}

	ofstream outfile(rootDir + "\\res.txt");
	if (!outfile)
	{
		cout << "open result file error" << endl;
	}

	int TotalCnt = 0, FailCnt = 0;
	while (getline(infile, filename))
	{
		string imgName = rootDir + "\\" + filename;
		img = imread(imgName.c_str());

		if (!judgeSegment(img))
		{
			outfile << filename << endl;
			FailCnt++;
		}
		TotalCnt++;
	}
	cout << "Ê§°ÜÂÊ£º" << FailCnt*1.0 / TotalCnt << endl;

	infile.close();
	outfile.close();
}

int main(int argc, char** argv)
{
	testSeg();
	cout << "Press any key..." << endl;
	int a;
	cin >> a;
	return 0;
}