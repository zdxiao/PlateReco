#include "easypr/core/chars_segment.h"

using namespace std;

namespace easypr {

	const float DEFAULT_BLUEPERCEMT = 0.3f;
	const float DEFAULT_WHITEPERCEMT = 0.1f;

	CCharsSegment::CCharsSegment() {
		m_LiuDingSize = DEFAULT_LIUDING_SIZE;
		m_theMatWidth = DEFAULT_MAT_WIDTH;
		m_theMatHeight = DEFAULT_MAT_HEIGHT;

		//！车牌颜色判断参数

		m_ColorThreshold = DEFAULT_COLORTHRESHOLD;
		m_BluePercent = DEFAULT_BLUEPERCEMT;
		m_WhitePercent = DEFAULT_WHITEPERCEMT;

		m_debug = DEFAULT_DEBUG;
	}

	//! 字符尺寸验证

	bool CCharsSegment::verifyCharSizes(Mat r, int& rSize) {
		// Char sizes 45x90
		float aspect = 45.0f / 90.0f;
		float charAspect = (float)r.cols / (float)r.rows;
		float error = 0.7f;
		float minHeight = 10.f;
		float maxHeight = 35.f;
		// We have a different aspect ratio for number 1, and it can be ~0.2
		float minAspect = 0.05f;
		float maxAspect = aspect + aspect * error;
		// area of pixels
		int area = cv::countNonZero(r);
		// bb area
		int bbArea = r.cols * r.rows;
		//% of pixel in area
		int percPixels = area / bbArea;

		if (percPixels <= 1 && charAspect > minAspect && charAspect < maxAspect &&
			r.rows >= minHeight && r.rows < maxHeight)
			return true;
		else if (percPixels <= 1 && r.rows >= minHeight && r.rows < maxHeight &&
			charAspect > minAspect)
		{
			rSize = (int)(charAspect / (aspect));
			if (m_debug){
				cout << charAspect << "rSize:" << rSize << endl;
			}
			return true;
		}
		else
			return false;
	}

	//! 字符预处理

	Mat CCharsSegment::preprocessChar(Mat in) {
		// Remap image
		int h = in.rows;
		int w = in.cols;

		//统一每个字符的大小

		int charSize = CHAR_SIZE;

		Mat transformMat = Mat::eye(2, 3, CV_32F);
		int m = max(w, h);
		transformMat.at<float>(0, 2) = float(m / 2 - w / 2);
		transformMat.at<float>(1, 2) = float(m / 2 - h / 2);

		Mat warpImage(m, m, in.type());
		warpAffine(in, warpImage, transformMat, warpImage.size(), INTER_LINEAR,
			BORDER_CONSTANT, Scalar(0));

		//！ 将所有的字符调整成统一的尺寸

		Mat out;
		resize(warpImage, out, Size(charSize, charSize));

		return out;
	}

	//! 字符分割与排序

	int getParts(int big_num, int size_of_partion){
		if (big_num <= 0 || size_of_partion <= 0)
			return 1;
		double result = big_num / size_of_partion*1.0;

		if (result < 1) return 1;
		if (result < 2.5) return 2;
		if (result < 4.2) return 3;
		if (result < 6) return 4;
	}

	int CCharsSegment::charsSegment(Mat input, vector<Mat>& resultVec) {
		if (!input.data) return 0x01;

		int w = input.cols;
		int h = input.rows;

		Mat tmpMat = input(Rect_<double>(w * 0.1, h * 0.1, w * 0.8, h * 0.8));

		// 判断车牌颜色以此确认threshold方法

		Color plateType = getPlateType(tmpMat, true, 60);

		Mat input_grey;
		cvtColor(input, input_grey, CV_BGR2GRAY);

		Mat img_threshold;

		// 二值化
		// 根据车牌的不同颜色使用不同的阈值判断方法
		// TODO：使用MSER来提取这些轮廓

		if (BLUE == plateType) {
			//cout << "BLUE" << endl;
			img_threshold = input_grey.clone();

			int w = input_grey.cols;
			int h = input_grey.rows;
			Mat tmp = input_grey(Rect_<double>(w * 0.1, h * 0.1, w * 0.8, h * 0.8));
			int threadHoldV = ThresholdOtsu(tmp);

			threshold(input_grey, img_threshold, threadHoldV, 255, CV_THRESH_BINARY);

		}
		else if (YELLOW == plateType) {
			//cout << "YELLOW" << endl;
			img_threshold = input_grey.clone();
			int w = input_grey.cols;
			int h = input_grey.rows;
			Mat tmp = input_grey(Rect_<double>(w * 0.1, h * 0.1, w * 0.8, h * 0.8));
			int threadHoldV = ThresholdOtsu(tmp);
			// utils::imwrite("resources/image/tmp/inputgray2.jpg", input_grey);

			threshold(input_grey, img_threshold, threadHoldV, 255,
				CV_THRESH_BINARY_INV);

		}
		else if (WHITE == plateType) {
			//cout << "WHITE" << endl;

			threshold(input_grey, img_threshold, 10, 255,
				CV_THRESH_OTSU + CV_THRESH_BINARY_INV);
		}
		else {
			// cout << "UNKNOWN" << endl;
			threshold(input_grey, img_threshold, 10, 255,
				CV_THRESH_OTSU + CV_THRESH_BINARY);
		}

		if (m_debug)
		{
			cout << "Binary stage" << endl;
			imshow("origin plate", input_grey);
			imshow("binary plate", img_threshold);
			waitKey(0);
		}

		//{//Erode test
		// Mat dst;
		// int erosion_type;
		// for (int erosion_elem = 0; erosion_elem < 3; erosion_elem++)
		// {
		//  if (erosion_elem == 0){ erosion_type = MORPH_RECT; }
		//  else if (erosion_elem == 1){ erosion_type = MORPH_CROSS; }
		//  else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

		//  for (int erosion_size = 0; erosion_size < 3; erosion_size++)
		//  {
		//	  Mat element = getStructuringElement(erosion_type,
		//						Size(erosion_size + 1, erosion_size + 1),
		//						Point(erosion_size, erosion_size));
		//	  /// 腐蚀操作
		//	  erode(img_threshold, dst, element);
		//	  stringstream windowName;
		//	  windowName << erosion_elem;
		//	  windowName << erosion_size;
		//	  cout << windowName.str() << endl;
		//	  imshow(windowName.str(), dst);
		//	  waitKey();
		//  }
		// }
		//}

		//腐蚀操作一次
		Mat element = getStructuringElement(MORPH_RECT,
			Size(2, 2),
			Point(1, 1));
		/// 腐蚀操作
		//erode(img_threshold, img_threshold, element);

		// 去除车牌上方的柳钉以及下方的横线等干扰
		// 并且也判断了是否是车牌
		// 并且在此对字符的跳变次数以及字符颜色所占的比重做了是否是车牌的判别条件
		// 如果不是车牌，返回ErrorCode=0x02

		if (!clearLiuDing(img_threshold))
		{
			if (m_debug)
			{
				cout << "Fail at clear Maoding...press any key to continue" << endl;
				waitKey(0);
			}
			return 0x02;
		}

		//膨胀
		element = getStructuringElement(MORPH_ELLIPSE,
			Size(2, 2),
			Point(1, 1));
		Rect middleZone(0, m_theMatHeight / 4, m_theMatWidth, m_theMatHeight - m_theMatHeight / 4);
		dilate(img_threshold(middleZone), img_threshold(middleZone), element);
		//element = getStructuringElement(MORPH_RECT,
		//	Size(2, 2),
		//	Point(1, 1));
		//dilate(img_threshold, img_threshold, element);


		if (m_debug)
		{
			cout << "Clear maoding" << endl;
			imshow("cleared", img_threshold);
			waitKey(0);
		}

		// 在二值化图像中提取轮廓

		Mat img_contours;
		img_threshold.copyTo(img_contours);

		vector<vector<Point> > contours;
		findContours(img_contours,
			contours,               // a vector of contours
			CV_RETR_EXTERNAL,       // retrieve the external contours
			CV_CHAIN_APPROX_NONE);  // all pixels of each contours

		vector<vector<Point> >::iterator itc = contours.begin();
		vector<Rect> vecRect;
		vector<Rect> vecRectBig;

		// 将不符合特定尺寸的字符块排除出去
		int meanWidth = 0;
		int meanHeight = 0;
		int meanY = 0;
		int meanWidthcount = 0;
		while (itc != contours.end()) {
			Rect mr = boundingRect(Mat(*itc));
			Mat auxRoi(img_threshold, mr);

			int rSize = 0;
			if (verifyCharSizes(auxRoi, rSize))
			{
				if (rSize >= 1){
					vecRectBig.push_back(mr);
					rSize = 0;
				}
				else{
					meanWidth += mr.width;
					meanY += mr.y;
					meanHeight += mr.height;
					meanWidthcount++;
					//if (m_debug)
					//{
					//	cout << "--> check contour" << endl;
					//	imshow("ctr", auxRoi);
					//	waitKey(0);
					//	destroyWindow("ctr");
					//}
					vecRect.push_back(mr);
				}
			}
			++itc;
		}

		// 切分大块， 加入vecRect
		if (meanWidthcount > 0 && vecRectBig.size() > 0)
		{
			meanWidth /= meanWidthcount;
			meanY /= meanWidthcount;
			meanHeight /= meanWidthcount;
			for (auto it = vecRectBig.begin(); it != vecRectBig.end(); it++)
			{
				Rect thisRect = *it;
				int thisRectEnd = thisRect.x + thisRect.width;
				int thisStartX = thisRect.x;

				int parts = getParts(thisRect.width, meanWidth);

				int partionWidth = thisRect.width / parts;

				for (int partIdx = 0; partIdx < parts && thisStartX + partionWidth < 136; partIdx++)
				{
					Rect smallerRect(thisStartX, thisRect.y, partionWidth, thisRect.height);

					int saveFlag = 1;
					for (auto itRect = vecRect.begin(); itRect != vecRect.end(); itRect++)
					{
						Rect goodRect = *itRect;
						// good rect 在左面
						if (goodRect.x - smallerRect.x < 0 && smallerRect.x - goodRect.x < goodRect.width / 2)
						{
							saveFlag = 0;
							break;
						}
						// good rect 在右侧
						if (goodRect.x - smallerRect.x >= 0 && goodRect.x - smallerRect.x < smallerRect.width / 2)
						{
							saveFlag = 0;
							break;
						}

					}
					if (saveFlag == 1)
					{ 
						vecRect.push_back(smallerRect);
					}
					thisStartX += partionWidth;
					if (m_debug && saveFlag == 1)
					{
						cout << "--> check contour" << endl;
						Mat roi(img_threshold, smallerRect);
						imshow("ctr", roi);
						waitKey(0);
						destroyWindow("ctr");
					}
				}
			}
		}


		// 如果找不到任何字符块，则返回ErrorCode=0x03

		if (vecRect.size() == 0){
			if (m_debug)
			{
				cout << "Fail at contour check...press any key to continue" << endl;
				waitKey(0);
			}
			return 0x03;
		}

		// 对符合尺寸的图块按照从左到右进行排序;
		// 直接使用stl的sort方法，更有效率

		vector<Rect> sortedRect(vecRect);
		std::sort(sortedRect.begin(), sortedRect.end(),
			[](const Rect& r1, const Rect& r2) { return r1.x < r2.x; });

		// eliminate split rect from sortedRect vertor, if it is unnecessary
		for (auto idx = 0; idx < sortedRect.size(); idx++)
		{
			Rect currRect = *(sortedRect.begin() + idx);
			if (m_debug)
			{
				cout << "--> checking contour" << endl;
				Mat Roi(img_threshold, currRect);
				imshow("ctr", Roi);
				waitKey(0);
				destroyWindow("ctr");
			}
			int interval = 0;
			int removeId = -1;
			int hitflag = 0;
			for (auto idy = idx + 1; idy < sortedRect.size(); idy++)
			{
				Rect compareRect = *(sortedRect.begin() + idy);

				interval = compareRect.x - currRect.x;
				if (interval < currRect.width / 3 * 2){

					if (m_debug)
					{
						cout << "----> compare" << endl;
						Mat Roi_x(img_threshold, currRect);
						Mat Roi_y(img_threshold, compareRect);
						imshow("X", Roi_x);
						imshow("Y", Roi_y);
						waitKey(0);
					}

					hitflag = 1;
					int connectome_x = 0, connectome_y = 0;

					Mat img_rect_x, img_rect_y;
					img_threshold(currRect).copyTo(img_rect_x);
					img_threshold(compareRect).copyTo(img_rect_y);

					vector<vector<Point> > ctr_x;
					vector<vector<Point> > ctr_y;

					findContours(img_rect_x,
						ctr_x,               // a vector of contours
						CV_RETR_EXTERNAL,       // retrieve the external contours
						CV_CHAIN_APPROX_NONE);  // all pixels of each contours			  
					findContours(img_rect_y,
						ctr_y,               // a vector of contours
						CV_RETR_EXTERNAL,       // retrieve the external contours
						CV_CHAIN_APPROX_NONE);  // all pixels of each contours
					connectome_x = ctr_x.size();
					connectome_y = ctr_y.size();

					removeId = connectome_x < connectome_y ? idy : idx;
					if (m_debug)
					{
						if (removeId == idy)
						{
							destroyWindow("Y");
						}
						else{
							destroyWindow("X");
						}
					}
					break;
				}
			}
			if (hitflag == 1){
				sortedRect.erase(sortedRect.begin() + removeId);
				idx--;
			}
		}

		size_t specIndex = 0;

		// 获得特殊字符对应的Rectt,如苏A的"A"

		specIndex = GetSpecificRect(sortedRect);

		if (m_debug)
		{
			cout << "Special char" << endl;
			Rect mr = sortedRect[specIndex];
			Mat roi = img_threshold(mr);
			imshow("special", roi);
			waitKey(0);
			destroyWindow("special");
		}

		// 根据特定Rect向左反推出中文字符
		// 这样做的主要原因是根据findContours方法很难捕捉到中文字符的准确Rect，因此仅能
		// 退过特定算法来指定

		Rect chineseRect;
		if (specIndex < sortedRect.size())
			chineseRect = GetChineseRect(sortedRect[specIndex], sortedRect, specIndex);
		else{
			if (m_debug){
				cout << "fail at get chinese rect...press any key to continue." << endl;
				waitKey(0);
			}
			return 0x04;
		}

		//新建一个全新的排序Rect
		//将中文字符Rect第一个加进来，因为它肯定是最左边的
		//其余的Rect只按照顺序去6个，车牌只可能是7个字符！这样可以避免阴影导致的“1”字符

		vector<Rect> newSortedRect;
		newSortedRect.push_back(chineseRect);
		RebuildRect(sortedRect, newSortedRect, specIndex);

		if (newSortedRect.size() == 0) {
			if (m_debug){
				cout << "fail, number of rect is not 7...press any key to continue." << endl;
				waitKey(0);
			}
			return 0x05;
		}

		if (newSortedRect.size() == 7){
			stretchingLastChar(newSortedRect);
			stretchingLastSeven(img_threshold, newSortedRect);
		}
		// 开始截取每个字符

		for (size_t i = 0; i < newSortedRect.size(); i++) {
			Rect mr = newSortedRect[i];

			// Mat auxRoi(img_threshold, mr);

			// 使用灰度图来截取图块，然后依次对每个图块进行大津阈值来二值化

			Mat auxRoi(input_grey, mr);
			Mat newRoi;

			if (BLUE == plateType) {
				/* img_threshold = auxRoi.clone();
				 int w = input_grey.cols;
				 int h = input_grey.rows;
				 Mat tmp = input_grey(Rect_<double>(w * 0.1, h * 0.1, w * 0.8, h * 0.8));
				 int threadHoldV = ThresholdOtsu(tmp);*/

				threshold(auxRoi, newRoi, 5, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
			}
			else if (YELLOW == plateType) {
				threshold(auxRoi, newRoi, 5, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU);

			}
			else if (WHITE == plateType) {
				threshold(auxRoi, newRoi, 5, 255, CV_THRESH_OTSU + CV_THRESH_BINARY_INV);
			}
			else {
				threshold(auxRoi, newRoi, 5, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
			}

			// 归一化大小

			newRoi = preprocessChar(newRoi);

			//腐蚀膨胀一次
			element = getStructuringElement(MORPH_RECT,
				Size(2, 2),
				Point(1, 1));
			erode(newRoi, newRoi, element);
			dilate(newRoi, newRoi, element);

			// 每个字符图块输入到下面的步骤进行处理

			resultVec.push_back(newRoi);
		}

		return 0;
	}

	//! 根据特殊车牌来构造猜测中文字符的位置和大小

	Rect CCharsSegment::GetChineseRect(const Rect rectSpe, std::vector<Rect>& sortedRect, int index) {
		int height = rectSpe.height;
		float newwidth = rectSpe.width * 1.15f;
		int x = rectSpe.x;
		int y = rectSpe.y;

		int newx = x - int(newwidth * 1.15);
		newx = newx > 0 ? newx : 0;

		if (newx == 0) newwidth = x;

		if (newwidth < rectSpe.width*0.6)
		{
			Rect a(rectSpe.x, rectSpe.y, rectSpe.width, rectSpe.height);
			sortedRect.erase(sortedRect.begin() + index);
			return a;
		}
		else{
			Rect a(newx, y, int(newwidth), height);
			return a;
		}
	}

	//! 找出指示城市的字符的Rect，例如苏A7003X，就是"A"的位置

	int CCharsSegment::GetSpecificRect(const vector<Rect>& vecRect) {
		vector<int> xpositions;
		vector<int> indexGate;
		vector<int> indexValue;
		int maxHeight = 0;
		int maxWidth = 0;
		double maxRatio = 0.0;

		for (size_t i = 0; i < vecRect.size(); i++) {
			xpositions.push_back(vecRect[i].x);
			indexValue.push_back(0);
			indexGate.push_back(-1);

			if (vecRect[i].height > maxHeight) {
				maxHeight = vecRect[i].height;
			}
			if (vecRect[i].width > maxWidth) {
				maxWidth = vecRect[i].width;
			}
			if (1.0*vecRect[i].width / vecRect[i].height > maxRatio){
				maxRatio = 1.0*vecRect[i].width / vecRect[i].height;
			}

		}

		int specIndex = 0;

		for (size_t i = 0; i < vecRect.size(); i++) {
			Rect mr = vecRect[i];
			int midx = mr.x + mr.width / 2;

			//如果一个字符有一定的大小，并且在整个车牌的1/7到2/7之间，则是我们要找的特殊字符
			//当前字符和下个字符的距离在一定的范围内

			if ((mr.width > maxWidth * 0.8 || mr.height > maxHeight * 0.8) &&
				((1.0*mr.width / mr.height) > 0.25) &&
				(vecRect.size() - i >= 6) &&
				(midx < int(m_theMatWidth / 7) * 2 &&
				midx > int(m_theMatWidth / 7) * 1)) {
				if (m_debug){
					cout << "spec ratio" << 1.0*mr.width / mr.height << endl;
				}
				//满足以上条件，该字符可能为特殊字符，做以下操作
				// 1，Gate打开
				// 2，计算当前index的分数

				indexGate[i] = 1;	// gate打开
				//index分数——当前Rect中心和特殊字符指定位置的距离
				indexValue[i] += int(fabs(midx - (m_theMatWidth / 14.0 * 3))); 
				indexValue[i] += 2 * (vecRect.size() - i - 6);
			}
		}
		int maxValue = 0;
		for (size_t i = 0; i < vecRect.size(); i++){
			if (indexGate[i] > 0 && indexValue[i] >= maxValue){
				maxValue = indexValue[i];
				specIndex = i;
			}
		}

		return specIndex;
	}

	//! 这个函数做两个事情
	//  1.把特殊字符Rect左边的全部Rect去掉，后面再重建中文字符的位置。
	//  2.从特殊字符Rect开始，依次选择6个Rect，多余的舍去。

	int CCharsSegment::RebuildRect(const vector<Rect>& vecRect,
		vector<Rect>& outRect, int specIndex) {
		int count = 6;
		for (size_t i = specIndex; i < vecRect.size() && count; ++i, --count) {
			outRect.push_back(vecRect[i]);
		}

		return 0;
	}

	void CCharsSegment::stretchingLastChar(vector<Rect>& vecSortedRect){
		//提高最后一个字符字符高度

		vector<int> heights;
		for (int i = 0; i < vecSortedRect.size(); i++){
			heights.push_back(vecSortedRect.at(i).height);
		}

		sort(heights.begin(), heights.end());
		int averageHeight = 0;
		for (int i = 2; i < vecSortedRect.size() - 2; i++){
			averageHeight = averageHeight + heights.at(i);
		}
		averageHeight = averageHeight / (vecSortedRect.size() - 2);

		if (vecSortedRect.back().height < averageHeight){
			vecSortedRect.back().y = vecSortedRect.back().y - averageHeight + vecSortedRect.back().height;
			if (vecSortedRect.back().y < 0){ vecSortedRect.back().y = 0; }
		}
	}

	void CCharsSegment::stretchingLastSeven(Mat img_threshold, vector<Rect>& vecSortedRect){
		int areanonezero = cv::countNonZero(img_threshold(vecSortedRect.back()));
		//int allarea = (vecSortedRect.back().height*vecSortedRect.back().width);
		//cout << "allarea" << allarea << "nonezero" << areanonezero << endl;
		if (areanonezero < 200){
			cout << "stretch last character" << endl;
			vecSortedRect.back().height += vecSortedRect.back().y;
			vecSortedRect.back().y = 0;

		}
	}


}

