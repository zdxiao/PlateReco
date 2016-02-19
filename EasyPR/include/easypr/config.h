#ifndef EASYPR_CONFIG_H_
#define EASYPR_CONFIG_H_

namespace easypr {

static const char* kDefaultSvmPath = "resources/model/svm.xml";
static const char* kDefaultAnnPath = "resources/model/ann.xml";

typedef enum {
  kForward = 1, // correspond to "has plate"
  kInverse = 0  // correspond to "no plate"
} SvmLabel;

static const float kSvmPercentage   = 0.7f;
static const int   kPredictSize     = 10;
static const int   kNeurons         = 40;

static const char *kChars[] = {
  "U", "T", "R",
  "6", "A", "D",
  "9", "3", "Y",
  "Z",
  /*  10  */
  "L", "8", "G",
  "5", "N", "M",
  "V", "Q", /* {"I", "I"} */
  "X", "F", "7",
  "G", "W", /* {"O", "O"} */
  "S", "K", "2",
  "B", "E", "C",
  "1", "0", "P",
  "H", "4",
  /*  24  */

/*
0 zh_gui1
1 zh_yu1
2 z
3 zh_zang
4 zh_cuan
5 zh_liao
6 zh_e
7 zh_meng
8 zh_jl
9 zh_jin
10 zh_min
11 zh_gan1
12 zh_gan
13 zh_xin
14 zh_shan
15 zh_qiong
16 zh_wan
17 zh_zhe
18 zh_sx
19 zh_su
20 zh_yu
21 zh_ning
22 zh_yue
23 zh_ji
24 zh_jing
25 zh_lu
26 zh_qing
27 zh_yun
28 zh_xiang
29 zh_gui
30 zh_hei
*/
  "zh_gui1" , "zh_yu1"    , "zh_hu"  ,
  "zh_zang" , "zh_cuan"  , "zh_liao" ,
  "zh_e"  , "zh_meng"   , "zh_jl"   ,
  "zh_jin"  , "zh_min" , "zh_gan1"   ,
  "zh_gan" , "zh_xin"   , "zh_shan" ,
  "zh_qiong"  , "zh_wan" , "zh_zhe" ,
  "zh_sx", "zh_su" , "zh_yu"   ,
  "zh_ning"   , "zh_yue"  , "zh_ji",
  "zh_jing"  , "zh_lu"   , "zh_qing"  ,
  "zh_yun"  , "zh_xiang"  , "zh_gui" ,
  "zh_hei","zh_error"
  /*  31  */
};

static const int kCharsTotalNumber = 65;

static bool kDebug = false;

}

#endif // EASYPR_CONFIG_H_
