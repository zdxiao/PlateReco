/*******************************************************************/
/*
				Author:		    Qi Gao
				Creat Date:	    2016/02/19
				Email:		    gaoqi@mail.bnu.edu.cn
				Last Modify:	2016/02/21
*/
/*********************************************************************/

#include <iostream>
#include <fstream>

#include "easypr/core/chars_segment.h"

using namespace std;
using namespace cv;
using namespace easypr;

int dbg = 1; // 1-¿ªÆôdebug 0-¹Ø±Õdebug

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

bool judgeSegment(Mat img, int debug_mode, string filename)
{
	vector<Mat> resultVec;
	CCharsSegment plate;

	plate.setDebug(debug_mode);

	int result = plate.charsSegment(img, resultVec);
	Mat res;
	if (result == 0) {
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
	filename = "A:\\WorkPlace\\CPLUS\\TN_ALPR\\plateData0220\\has\\result" + filename;
	imwrite(filename, res);
	return false;
}

void testSeg(int debug_mode)
{
	string rootDir("A:\\WorkPlace\\CPLUS\\TN_ALPR\\plateData0220\\has");
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
	while (getline(infile, filename)&&TotalCnt<100)
	{
		string imgName = rootDir + "\\" + filename;
		img = imread(imgName.c_str());

		if (!judgeSegment(img,debug_mode,filename))
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
	cout << "Input debug mode:" << endl;
	cout << "    0 - test " << endl;
	cout << "    1 - debug " << endl;
	cin >> dbg;
	testSeg(dbg);
	cout << "Press any key..." << endl;
	int a;
	cin >> a;
	return 0;
}