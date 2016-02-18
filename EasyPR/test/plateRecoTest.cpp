#include "easypr/core/plate_recognize.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace cv;
using namespace std;
using namespace easypr;

int main(int argc, char**argv)
{
	cout << "hello world!" << endl;
	ifstream listFile("/home/xiao/Project/PlateReco/EasyPR/list3.txt");
	while(1)
	{
		string line;
		getline(listFile, line);
		cout << line << endl;
		line = "/home/xiao/Project/PlateReco/2015_12_09/" + line;
		//line = "./resources/image/general_test/" + line;
		Mat img = imread(line);
		namedWindow("test", 0);
		imshow("test", img);
//		CPlateDetect cpd;
        CPlateRecognize cpr;
//		cpd.setPDDebug(true);
        cpr.setDebug(false);
		vector<CPlate> resultVec;
		vector<string> licenseVec;
//		cpd.plateDetect(img, resultVec);
        cpr.plateRecognize(img, licenseVec);
		//waitKey();
	}
	listFile.close();
	return 0;
}

