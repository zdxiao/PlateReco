
/*!
*  Copyright (c) 2016 by Heiyu-tech
* \brief C++  chars recognition.
*/

//
//  modify  by Jingping Xu on 2016/2/17
//  WeChat: sea942127
//  E-mail: vincent.0812@qq.com
//

#include "easypr/core/chars_recognise.h"
#include "easypr/util/util.h"

namespace easypr {

CCharsRecognise::CCharsRecognise() { m_charsSegment = new CCharsSegment(); }

CCharsRecognise::~CCharsRecognise() { SAFE_RELEASE(m_charsSegment); }

int CCharsRecognise::charsRecognise(Mat plate, std::string& plateLicense) {
  std::vector<Mat> matChars;

  int result = m_charsSegment->charsSegment(plate, matChars);
  auto character = CharsIdentify::instance();
  int flag = 0;
  if (result == 0) {
    for (auto block : matChars) {
		if (flag == 0){ auto chars = character->identify2(block);  plateLicense.append(chars.second); }
                else if(flag == 1){ auto chars = character->identify3(block);  plateLicense.append(chars.second); }
		else { auto chars = character->identify(block);  plateLicense.append(chars.second); }
                 flag++;
     
    }
  }
  if (plateLicense.size() < 7) {
    return -1;
  }

  return result;

}
}
