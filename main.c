/**
  ******************************************************************************
  * @file    app.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides all the Application firmware functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

// Remarks
// Don't forget to select the right PLL settings for you Xtal
// system_stm32f4xx.c
/************************* PLL Parameters *************************************/
/* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
/ #define PLL_M      8
/ #define PLL_N      336
*/

// STM's VCP supports only 64 Byte packets
/* CDC Endpoints parameters: you can fine tune these values depending on the needed baudrates and performance.
#ifdef USE_USB_OTG_HS
 #define CDC_DATA_MAX_PACKET_SIZE       64  // Endpoint IN & OUT Packet size
 #define CDC_CMD_PACKET_SZE
*/


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"
#include <stm32f4xx_tim.h>
#include "stm32f4_discovery.h"

#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"

#include "util.h"

#include "stm32adc.h"
#include "stm32serial.h"
#include "gcode_parser.h"
#include "stm32F4xx_fft.h"
#include "PGA_Ctrl.h"
#include "defines.h"
#include "dac.h"
//#include "rpm_sig_gen.h"
#include "DOGM_LCD_LIB/driver_dogm128.h"
#include "DOGM_LCD_LIB/graphics.h"
#include "DOGM_LCD_LIB/aux_routines.h"
//#include "user_if.h"
#include "menu.h"
#include "BGT24_Ctrl.h"
#include "ff.h"
#include "diskio.h"

#include "Circle_fitting.h"

const int16_t testadc11[FFT_LENGTH]={
		945,849,757,667,561,429,267,95,-81,-260,-432,-593,-738,-859,-954,-1017,-1048,-1051,-1032,-995,-947,-896,-845,-795,-748,-700,-657,-613,-576,-542,-513,-485,-461,-435,-406,-375,
		-339,-301,-258,-206,-144,-65,32,146,273,406,529,631,723,809,901,987,1057,1108,1139,1152,1147,1125,1087,1029,953,861,771,681,579,451,297,126,-47,-220,-391,-551,-693,-814,-905,
		-965,-996,-997,-974,-935,-886,-833,-782,-732,-685,-639,-596,-556,-519,-486,-461,-437,-415,-392,-367,-338,-305,-268,-227,-176,-115,-35,63,178,306,437,558,656,746,835,933,1020,
		1083,1136,1164,1175,1170,1145,1103,1041,966,870,777,686,584,458,301,130,-47,-211,-371,-526,-671,-800,-897,-964,-1000,-1007,-990,-954,-911,-862,-814,-767,-721,-676,-633,-593,
		-556,-524,-496,-473,-449,-424,-396,-366,-331,-294,-249,-199,-135,-55,42,155,282,415,535,639,729,816,908,993,1060,1110,1141,1151,1148,1124,1082,1025,947,854,764,672,572,442,
		286,117,-59,-234,-403,-563,-708,-829,-922,-982,-1016,-1018,-997,-959,-910,-857,-804,-754,-705,-660,-615,-573,-538,-505,-477,-453,-430,-407,-381,-351,-319,-282,-241,-188,
		-128,-48,50,164,296,427,551,651,742,831,926,1015,1079,1128,1158,1172,1164,1139,1099,1037,962,867,773,686,583,457,300,128,-47,-223,-393,-558,-701,-824,-919,-982,-1017,-1021,-1004,
		-966,-920,-871,-821,-774,-725,-681,-637,-596,-558,-524,-495,-471,-446,-421,-393,-359,-308,-265,-223,-172,-112,-34,62,174,300,431,547,650,738,826,921,1005,1071,1119,1148,1159,1153,
		1129,1086,1028,950,855,765,675,573,443,286,118,-56,-233,-405,-565,-711,-833,-924,-989,-1020,-1024,-1002,-963,-914,-860,-809,-757,-707,-660,-616,-575,-537,-506,-477,-454,-429,-407,
		-381,-351,-319,-281,-241,-191,-130,-50,48,164,292,423,544,646,735,823,916,1005,1070,1121,1150,1164,1160,1135,1094,1033,958,865,772,684,583,459,302,131,-45,-223,-394,-554,-701,
		-824,-918,-981,-1016,-1020,-1003,-965,-919,-869,-819,-770,-723,-676,-633,-591,-554,-522,-494,-467,-444,-418,-390,-357,-323,-285,-241,-191,-127,-49,47,164,290,420,541,643,734,824,
		919,1003,1074,1124,1154,1167,1160,1138,1096,1039,961,870,783,700,606,479,320,150,-28,-206,-379,-543,-689,-812,-907,-970,-1004,-1009,-988,-951,-903,-850,-799,-750,-701,-655,-612,
		-571,-534,-503,-476,-453,-431,-410,-385,-357,-325,-289,-250,-200,-139,-60,37,152,282,414,539,639,730,817,911,996,1063,1115,1147,1158,1153,1129,1090,1030,953,860,771,682,579,453,
		296,126,-51,-224,-394,-558,-703,-827,-921,-985,-1021,-1023,-1005,-969,-923,-873,-823,-773,-725,-680,-636,-592,-555,-523,-492,-466,-442,-415,-387,-357,-320,-282,-239,-187,-125,
		-48,49,163,290,420,541,643,732,821,913,997,1067,1115,1148,1159,1155,1129,1088,1028,950,858,767,677,574,444,286,119,-58,-233,-403,-566,-709,-830,-921,-983,-1015,-1017,-997,-957,
		-908,-854,-803,-753,-704,-658,-614,-571,-526,-483,-453,-432,-413,-392,-368,-342,-311,-279,-239,-191,-131,-52,43,157,282,414,537,640,729,817,913,1000,1067,1120,1150,1163,1156,1135,
		1093,1035,959,866,773,683,581,454,297,125,-49,-224,-396,-556,-700,-824,-916,-982,-1016,-1020,-1002,-965,-918,-869,-818,-771,-723,-677,-633,-592,-554,-522,-495,-468,-443,-419,-390,
		-357,-323,-283,-240,-189,-126,-47,50,162,289,419,539,643,728,819,911,996,1065,1113,1145,1157,1152,1126,1087,1028,949,858,765,676,574,444,287,117,-57,-232,-401,-562,-705,-826,-920,
		-981,-1013,-1015,-994,-954,-906,-854,-800,-749,-700,-654,-610,-570,-532,-500,-472,-449,-427,-406,-379,-351,-322,-284,-245,-195,-136,-59,38,153,282,412,539,639,729,818,912,999,1068,
		1121,1160,1184,1186,1156,1116,1054,977,882,784,696,592,466,309,139,-35,-216,-386,-552,-697,-822,-918,-980,-1016,-1020,-1002,-964,-918,-868,-817,-769,-722,-676,-632,-590,-555,-522,
		-493,-469,-444,-418,-389,-358,-324,-286,-242,-191,-128,-49,47,162,286,420,542,645,734,823,917,1003,1073,1122,1153,1166,1158,1136,1092,1035,955,860,769,680,576,446,290,120,-58,-231,
		-403,-566,-710,-833,-926,-988,-1021,-1023,-1003,-964,-916,-862,-809,-759,-709,-661,-616,-575,-537,-505,-477,-454,-431,-409,-384,-356,-323,-289,-249,-202,-141,-65,31,144,272,406,531,
		634,725,811,907,993,1060,1114,1148,1161,1157,1135,1095,1037,961,869,776,688,588,464,308,138,-37,-214,-385,-547,-691,-816,-911,-974,-1007,-1012,-994,-958,-908,-843,-786,-740,-694,
		-651,-609,-570,-534,-504,-476,-451,-428,-403,-377,-346,-313,-276,-235,-185,-123,-47,49,162,288,419,542,643,733,821,915,999,1069,1115,1146,1160,1154,1129,1087,1025,948,858,763,673,
		572,442,286,118,-57,-234,-404,-566,-712,-833,-926,-990,-1021,-1024,-1003,-964,-915,-862,-809,-757,-709,-661,-617,-575,-538,-506,-479,-455,-433,-411,-386,-360,-328,-293,-254,-205,
		-145,-68,28,143,272,404,529,633,722,809,901,990,1057,1111,1143,1158,1153,1133,1093,1034,959,868,775,688,586,461,308,139,-37,-212,-384,-546,-689,-812,-908,-970,-1003,-1009,-989,-954,
		-908,-858,-808,-760,-711,-665,-620,-579,-541,-509,-480,-455,-430,-405,-378,-347,-313,-276,-234,-183,-122,-45,52,164,290,419,546,657,754,847,942,1027,1092,1138,1165,1175,1167,1143,
		1099,1037,961,864,771,681

};

const int16_t testadc22[FFT_LENGTH]={
		490,475,466,458,454,446,433,410,375,321,246,147,23,-122,-282,-452,-625,-786,-938,-1068,-1173,-1254,-1308,-1342,-1350,-1342,-1312,-1259,-1190,-1095,-986,-859,-720,-575,-422,-271,-123,26,
		176,323,461,579,676,757,829,889,925,934,923,893,852,806,760,718,677,636,599,567,539,518,501,493,488,486,484,480,468,449,416,363,290,192,68,-79,-241,-412,-585,-750,-902,-1033,-1138,-1217,
		-1272,-1303,-1314,-1304,-1273,-1221,-1156,-1067,-959,-836,-695,-552,-403,-255,-105,41,189,339,474,591,685,765,840,900,934,942,929,898,856,809,765,721,681,641,605,572,544,518,500,488,481,
		477,475,470,460,439,407,367,304,204,93,-57,-220,-394,-566,-732,-888,-1020,-1130,-1211,-1272,-1307,-1320,-1315,-1285,-1235,-1166,-1076,-969,-842,-704,-558,-411,-259,-113,37,187,333,471,
		587,682,761,835,894,928,938,924,894,852,805,758,715,674,633,596,562,533,510,492,482,476,475,474,468,458,438,403,352,277,180,56,-90,-253,-421,-596,-761,-914,-1043,-1147,-1226,-1281,-1312,
		-1323,-1314,-1283,-1231,-1165,-1077,-967,-844,-703,-562,-411,-263,-116,35,182,332,465,584,677,756,830,887,922,932,917,886,845,799,755,713,672,633,597,564,533,510,492,480,475,470,469,464,
		454,434,399,349,276,180,58,-87,-247,-417,-590,-755,-905,-1037,-1145,-1225,-1283,-1318,-1330,-1322,-1290,-1242,-1173,-1082,-975,-844,-708,-561,-412,-251,-83,63,212,356,489,602,694,773,849,
		908,941,950,937,905,861,813,765,721,679,639,602,566,537,512,495,485,479,477,475,470,460,440,406,355,280,181,57,-89,-251,-424,-597,-762,-913,-1044,-1150,-1229,-1285,-1315,-1329,-1317,-1284,
		-1234,-1168,-1078,-970,-846,-706,-562,-411,-264,-117,33,179,328,469,586,680,761,834,891,924,933,918,888,847,799,756,712,672,631,595,561,532,508,490,479,474,471,469,465,455,437,403,353,279,
		184,61,-84,-243,-413,-585,-751,-904,-1034,-1141,-1224,-1281,-1317,-1330,-1321,-1292,-1245,-1173,-1084,-976,-847,-711,-565,-415,-263,-114,37,187,333,470,586,680,763,836,895,929,939,929,900,
		857,812,765,722,681,642,607,574,544,522,503,495,502,476,510,506,493,471,437,383,309,207,81,-68,-233,-407,-581,-753,-904,-1035,-1143,-1223,-1280,-1311,-1324,-1314,-1283,-1234,-1168,-1080,
		-972,-848,-709,-565,-414,-266,-117,33,181,332,469,586,680,762,836,894,927,935,919,889,846,799,754,712,669,630,596,561,530,507,488,476,472,467,468,463,452,433,399,350,277,180,59,-87,-247,
		-417,-590,-754,-908,-1038,-1147,-1227,-1283,-1319,-1332,-1325,-1297,-1246,-1178,-1087,-978,-850,-714,-568,-418,-269,-119,27,180,324,463,580,675,755,832,890,929,939,925,898,858,810,765,721,
		681,640,605,571,541,518,500,489,485,483,480,476,464,445,411,360,285,186,62,-87,-249,-423,-597,-763,-918,-1046,-1154,-1233,-1288,-1318,-1329,-1318,-1287,-1236,-1154,-1066,-952,-827,-691,
		-547,-401,-255,-105,40,189,338,475,592,685,767,841,898,933,941,926,895,853,806,760,717,675,635,598,564,533,509,489,479,472,469,466,463,452,432,398,348,274,179,56,-88,-248,-418,-590,-757,
		-910,-1040,-1148,-1228,-1286,-1320,-1335,-1326,-1298,-1253,-1180,-1091,-981,-857,-718,-571,-424,-270,-127,24,176,321,460,579,674,755,830,890,926,940,927,898,859,813,767,725,683,644,608,
		574,547,521,503,491,487,483,481,477,465,444,410,358,284,184,60,-86,-251,-423,-598,-764,-916,-1047,-1152,-1232,-1285,-1317,-1328,-1315,-1286,-1232,-1165,-1080,-970,-846,-704,-562,-410,
		-261,-113,40,184,334,472,590,684,767,841,899,936,944,930,898,856,807,762,719,684,650,618,582,552,526,507,495,487,481,479,473,461,441,405,353,279,181,58,-89,-249,-419,-591,-755,-909,
		-1040,-1149,-1232,-1287,-1325,-1337,-1330,-1305,-1253,-1185,-1093,-986,-860,-722,-578,-427,-278,-127,22,172,320,459,577,672,755,828,889,926,939,926,899,859,812,767,724,683,644,608,573,
		545,519,501,491,486,482,481,475,464,445,412,361,289,190,67,-80,-244,-416,-589,-756,-910,-1041,-1148,-1226,-1282,-1313,-1323,-1313,-1281,-1231,-1165,-1076,-968,-844,-703,-558,-407,-259,
		-111,40,186,333,473,591,686,766,840,899,934,942,927,898,855,806,761,717,676,636,597,563,533,509,492,481,476,471,471,465,456,437,402,352,279,183,61,-84,-245,-413,-584,-749,-903,-1036,
		-1134,-1202,-1261,-1299,-1314,-1307,-1282,-1238,-1169,-1082,-976,-850,-714,-571,-422,-274,-127,22,171,318,458,574,669,751,824,884,922,933,923,895,856,811,766,723,682,643,606,573,544,
		518,501,489,484,480,477,472,462,443,407,357,282,185,60,-85,-248,-422,-596,-763,-915,-1046,-1153,-1232,-1285,-1318,-1327,-1317,-1286,-1234,-1167,-1080,-971,-846,-707,-561,-410,-261,
		-113,38,183,335,473,590,686,766,844,902,939,947,933,902,858,810,762,719,677,635,600,566,534,511,492,483,476,474,471,467,456,438,404,353,279,183,61,-83,-245,-414,-585,-751,-905,-1037,
		-1143,-1227,-1285,-1320,-1333,-1325,-1299,-1248,-1180,-1090,-981,-858,-718,-575,-425,-277,-128,21,169,317,455,573,670,753,826,886,933,960,952,929,886,837,787,741,699,660,623,588,557,
		532,513,500,492,488

};
/*int16_t testadc1[FFT_LENGTH]={-740,-674,-621,-585,-564,-555,-552,-548,-537,-519,-496,-466,
		-430,-404,-370,-312,-208,-43,173,418,635,816,1033,1230,1381,
		1488,1552,1576,1569,1541,1492,1429,1355,1269,1173,1066,944,
		812,695,553,354,108,-166,-452,-732,-997,-1220,-1400,-1523,
		-1588,-1592,-1540,-1447,-1327,-1205,-1087,-981,-888,-807,
		-737,-677,-632,-610,-604,-617,-639,-661,-678,-678,-657,-614,
		-551,-480,-406,-321,-220,-82,105,328,561,745,951,1166,1350,
		1494,1602,1675,1712,1722,1709,1673,1623,1558,1485,1404,1317,
		1222,1113,988,844,710,551,325,57,-217,-484,-731,-951,-1136,
		-1279,-1374,-1415,-1407,-1363,-1291,-1204,-1106,-1005,-903,
		-808,-720,-648,-594,-555,-536,-525,-521,-517,-507,-492,-471,
		-450,-436,-421,-393,-337,-229,-65,153,398,624,805,1021,1223,
		1378,1492,1558,1584,1583,1556,1511,1450,1377,1294,1199,1092,
		966,842,731,598,406,158,-115,-404,-686,-952,-1179,-1359,-1485,
		-1550,-1556,-1505,-1413,-1297,-1177,-1064,-961,-874,-798,-731,
		-676,-637,-617,-612,-629,-653,-677,-696,-699,-677,-634,-572,
		-499,-423,-340,-237,-103,81,300,536,725,918,1136,1320,1464,
		1575,1646,1686,1697,1685,1651,1603,1540,1469,1388,1301,1206,
		1098,971,830,697,536,306,40,-235,-500,-748,-967,-1150,-1293,
		-1383,-1422,-1413,-1365,-1293,-1203,-1104,-1000,-900,-803,
		-716,-642,-588,-552,-534,-524,-520,-516,-504,-485,-463,-442,
		-426,-411,-384,-329,-224,-63,150,398,623,807,1022,1224,1382,
		1491,1559,1589,1584,1561,1514,1453,1378,1294,1197,1089,964,
		830,707,567,367,119,-156,-441,-723,-986,-1217,-1407,-1544,
		-1613,-1605,-1528,-1410,-1275,-1137,-1012,-898,-797,-710,
		-632,-563,-522,-501,-506,-518,-542,-588,-630,-655,-653,-621,
		-566,-501,-431,-367,-291,-185,-37,163,396,615,788,995,1192,
		1351,1477,1562,1615,1639,1638,1616,1573,1518,1452,1375,1292,
		1194,1086,956,815,681,512,274,8,-261,-522,-759,-970,-1142,
		-1270,-1346,-1367,-1343,-1287,-1211,-1118,-1017,-910,-804,
		-703,-612,-538,-483,-449,-434,-434,-441,-450,-454,-451,-444,
		-437,-436,-436,-429,-395,-313,-171,32,272,523,718,911,1125,
		1298,1429,1512,1558,1566,1546,1508,1448,1379,1295,1198,1086,
		961,823,699,554,352,106,-164,-442,-715,-967,-1177,-1340,
		-1449,-1498,-1487,-1422,-1321,-1201,-1079,-967,-870,-784,
		-710,-645,-593,-560,-544,-549,-572,-606,-644,-677,-692,
		-680,-641,-581,-506,-429,-347,-251,-120,58,276,511,704,
		894,1112,1295,1443,1554,1629,1670,1687,1679,1662,1630,1580,
		1510,1427,1338,1240,1128,999,851,712,547,317,47,-230,-496,
		-742,-960,-1143,-1283,-1375,-1412,-1402,-1357,-1287,-1200,
		-1101,-999,-892,-794,-702,-629,-569,-531,-509,-499,-499,
		-497,-490,-476,-458,-442,-432,-420,-400,-350,-254,-101,111,
		353,588,768,974,1176,1337,1453,1523,1559,1560,1536,1494,
		1432,1362,1279,1183,1072,947,813,693,549,347,103,-165,-447,
		-722,-977,-1191,-1359,-1474,-1530,-1530,-1473,-1379,-1264,
		-1146,-1038,-940,-857,-781,-716,-660,-622,-601,-601,-615,
		-643,-673,-696,-702,-685,-641,-577,-501,-421,-333,-228,
		-94,87,306,536,725,922,1141,1324,1473,1584,1659,1701,
		1715,1704,1670,1621,1559,1485,1404,1312,1216,1103,974,
		828,693,527,291,21,-252,-520,-763,-981,-1164,-1304,-1393,
		-1430,-1418,-1370,-1295,-1202,-1093,-970,-861,-769,-685,
		-617,-566,-533,-515,-511,-512,-510,-503,-488,-470,-452,
		-440,-426,-403,-348,-248,-91,120,361,593,773,981,1179,
		1340,1452,1522,1556,1560,1537,1496,1437,1368,1286,1192,
		1084,962,828,706,566,369,125,-145,-428,-706,-964,-1182,
		-1357,-1475,-1535,-1536,-1482,-1387,-1273,-1155,-1045,
		-945,-860,-786,-722,-666,-630,-610,-608,-619,-646,-674,
		-696,-703,-685,-643,-581,-507,-428,-343,-238,-104,76,293,
		525,716,908,1128,1314,1464,1575,1651,1694,1710,1700,1667,
		1622,1560,1488,1411,1323,1225,1116,991,846,709,548,318,53,
		-220,-482,-726,-945,-1129,-1271,-1366,-1408,-1402,-1357,
		-1290,-1205,-1109,-1008,-908,-817,-729,-660,-606,-569,-550,
		-538,-534,-526,-514,-492,-465,-441,-422,-405,-376,-319,-213,
		-54,159,402,626,804,1030,1245,1400,1511,1574,1602,1598,1570,
		1523,1458,1385,1301,1204,1093,967,831,707,564,366,121,-151,
		-437,-719,-982,-1205,-1381,-1502,-1566,-1567,-1513,-1418,
		-1301,-1181,-1067,-965,-878,-800,-733,-676,-637,-617,-614,
		-628,-651,-680,-699,-703,-684,-640,-576,-501,-420,-336,-233,
		-99,81,299,530,722,915,1133,1315,1466,1576,1651,1693,1706,1695,
		1663,1614,1552,1480,1400,1312,1218,1105,979,837,702,542,315,47,
		-226,-489,-733,-951,-1135,-1278,-1373,-1413,-1405,-1361,-1292,
		-1205,-1108,-1006,-903,-807,-719,-648,-592,-557,-538,-532,-530,
		-528,-516,-498,-475,-453,-436,-421,-394,-339,-234,-74,138,387,
		613,795,1008,1206,1364,1474,1544,1576,1576,1551,1507,1447,1376,
		1292,1198,1090,965,833,710,569,375,130,-141,-423,-707,-969,
		-1191,-1371,-1492,-1537,-1529,-1475,-1383,-1267,-1146,-1033,
		-933,-847,-772,-705,-651,-613,-594,-594,-610,-636,-668,-690,
		-698,-681,-641,-581,-512,-434,-352,-252,-119,62,282,516,710,
		902,1121,1309,1458,1570,1647,1690,1703,1692,1659,1611,1549,
		1477,1399,1312,1217,1110,985,844,711,555,332,69,-201,-466,-710,
		-929,-1116,-1264,-1362,-1407,-1404,-1364,-1299,-1216,-1122,-1023,
		-924,-831,-745,-673,-618,-580,-559,-547,-542,-535,-521,-500,-473,
		-449,-429,-411,-383,-326,-220,-58,158,404,625,805,1019,1221,1375,
		1486,1549,1579,1575,1550,1503,1443,1370,1286,1190,1084,959,829,
		706,565,369,126,-145,-432,-715,-979,-1204,-1382,-1507,-1570,-1576,
		-1524,-1429,-1310,-1185,-1067,-963,-869,-788,-717,-655,-614,-590,-588
};
int16_t testadc2[FFT_LENGTH]={-1745,-1673,-1567,-1430,-1273,-1088,-892,-688,-486,-295,-109,87,266,435,626,778,
		978,1176,1326,1413,1433,1393,1298,1165,1006,840,716,607,502,403,324,269,242,240,
		262,308,375,458,542,625,695,756,792,794,752,666,529,311,31,-278,-603,-912,-1184,
		-1400,-1564,-1678,-1752,-1792,-1807,-1803,-1774,-1716,-1616,-1489,-1333,-1149,-946,
		-713,-474,-227,6,227,427,585,726,884,1086,1266,1399,1474,1482,1430,1331,1195,1037,
		873,740,627,515,403,304,230,187,170,183,223,285,367,457,550,628,693,735,750,727,669,
		567,400,168,-101,-392,-683,-957,-1198,-1395,-1554,-1672,-1751,-1783,-1779,-1748,-1671,
		-1560,-1423,-1256,-1075,-875,-668,-468,-271,-93,88,260,445,612,769,973,1179,1338,1431,
		1461,1427,1339,1210,1055,888,752,644,543,450,372,319,291,287,305,349,412,494,592,664,
		732,789,821,820,772,684,547,331,51,-261,-588,-898,-1174,-1393,-1558,-1679,-1756,-1800,
		-1817,-1815,-1793,-1737,-1643,-1517,-1360,-1176,-974,-742,-502,-256,-19,200,399,565,
		704,853,1052,1234,1369,1445,1457,1408,1313,1182,1025,864,732,622,508,397,299,225,183,
		167,181,221,282,363,453,546,624,689,729,740,719,658,551,382,147,-125,-414,-708,-981,
		-1217,-1413,-1569,-1684,-1759,-1789,-1784,-1745,-1674,-1559,-1421,-1255,-1070,-876,-668,
		-468,-271,-93,83,256,432,601,757,956,1162,1321,1420,1450,1419,1334,1204,1049,884,750,642,
		540,447,367,313,286,282,303,346,409,487,567,646,712,769,803,801,755,668,526,290,-20,-372,
		-731,-1059,-1330,-1542,-1697,-1795,-1841,-1864,-1870,-1863,-1844,-1789,-1690,-1545,-1356,
		-1181,-959,-732,-480,-232,10,233,420,577,700,841,1031,1222,1379,1484,1525,1500,1424,1309,
		1165,1006,849,728,628,528,435,361,309,289,292,324,378,446,521,596,661,712,742,742,706,634,
		513,321,73,-208,-506,-799,-1069,-1296,-1485,-1633,-1741,-1804,-1825,-1819,-1785,-1708,-1595,
		-1455,-1291,-1110,-909,-707,-504,-309,-129,49,219,402,574,735,932,1154,1338,1458,1512,1502,
		1434,1321,1176,1014,857,736,639,552,476,418,383,373,383,414,467,530,598,664,720,764,785,
		772,717,621,466,231,-52,-364,-685,-987,-1250,-1459,-1617,-1734,-1805,-1841,-1855,-1856,
		-1838,-1791,-1700,-1569,-1408,-1223,-1010,-772,-521,-264,-15,218,419,584,724,888,1094,
		1285,1428,1513,1529,1491,1399,1269,1117,953,799,688,585,484,405,330,303,284,288,318,370,
		440,514,589,655,710,744,749,720,657,548,372,134,-138,-433,-726,-999,-1238,-1431,-1587,
		-1703,-1772,-1799,-1790,-1753,-1668,-1558,-1414,-1248,-1067,-864,-666,-463,-275,-94,79,
		246,421,589,745,934,1144,1309,1409,1444,1416,1337,1213,1063,901,764,657,563,471,395,342,
		310,307,322,362,419,491,566,636,701,749,776,769,721,632,485,258,-22,-333,-658,-961,-1226,
		-1439,-1601,-1718,-1794,-1833,-1848,-1851,-1834,-1789,-1700,-1570,-1416,-1229,-1022,-789,
		-540,-291,-43,183,390,561,700,852,1052,1239,1382,1466,1487,1446,1359,1235,1083,920,777,667,
		565,460,369,295,251,237,249,285,346,421,503,584,656,710,745,749,722,656,546,369,130,-145,
		-442,-740,-1021,-1264,-1464,-1623,-1718,-1778,-1800,-1793,-1758,-1680,-1567,-1425,-1256,
		-1073,-871,-672,-471,-281,-100,76,249,428,597,759,962,1174,1343,1446,1484,1456,1378,1254,
		1103,935,790,681,585,493,416,360,328,323,336,371,427,496,570,640,705,755,783,779,732,646,
		504,280,3,-308,-630,-938,-1206,-1420,-1582,-1699,-1777,-1818,-1835,-1836,-1819,-1769,-1681,
		-1553,-1402,-1217,-1013,-784,-540,-294,-49,172,374,546,689,836,1030,1215,1356,1440,1463,1424,
		1339,1214,1064,901,763,652,546,439,342,268,222,204,216,252,312,389,475,562,637,696,735,743,719,
		658,553,384,153,-115,-406,-699,-973,-1216,-1414,-1572,-1691,-1764,-1790,-1785,-1748,-1667,
		-1555,-1413,-1243,-1060,-858,-654,-453,-262,-86,87,253,431,597,754,953,1162,1324,1422,1454,
		1421,1349,1241,1083,920,776,666,569,477,397,342,311,305,324,363,422,492,571,643,709,762,794,
		791,747,660,522,303,22,-290,-615,-922,-1194,-1412,-1574,-1692,-1767,-1807,-1824,-1825,-1804,
		-1751,-1658,-1532,-1376,-1198,-989,-763,-518,-275,-36,187,384,553,690,834,1029,1211,1350,1432,
		1450,1408,1317,1190,1038,874,743,633,523,413,316,244,195,180,192,231,292,372,463,550,631,693,
		734,746,723,662,559,392,160,-110,-400,-692,-968,-1208,-1408,-1567,-1688,-1766,-1798,-1800,-1764,
		-1689,-1581,-1440,-1272,-1085,-885,-676,-473,-278,-96,81,252,431,600,757,962,1171,1333,1433,
		1465,1435,1351,1227,1074,907,767,659,562,470,394,338,309,301,321,362,423,498,575,647,715,770,
		804,803,758,672,537,319,48,-247,-582,-884,-1161,-1384,-1554,-1674,-1756,-1800,-1819,-1821,-1802,
		-1745,-1656,-1531,-1379,-1198,-989,-764,-519,-276,-34,189,389,561,699,850,1046,1231,1370,1451,1471,
		1428,1337,1210,1054,889,752,639,526,414,313,238,188,169,181,219,277,357,448,535,618,682,726,740,720,
		664,567,408,184,-79,-365,-655,-931,-1169,-1369,-1528,-1650,-1732,-1771,-1771,-1739,-1670,-1562,-1425,
		-1259,-1077,-875,-669,-469,-275,-98,79,248,426,595,748,941,1148,1304,1400,1426,1392,1306,1178,1025,
		861,732,628,526,432,356,305,275,273,294,337,402,481,563,639,709,767,803,804,760,673,538,323,43,-269,
		-597,-907,-1180,-1402,-1568,-1687,-1762,-1803,-1817,-1815,-1793,-1732,-1640,-1512
};*/


int16_t Ampli,maxAmpli,minAmpli;

#define PI 3.1415926
#define PI2 6.283185

void Setup_USB_Status_LED(void);
void SysTick_Handler(void);
void DMA1_Channel1_IRQHandler(void);

volatile uint32_t packet_receive=1;
uint16_t pulsecnt=0;

float Unwrap_Phase[FFT_LENGTH];
u32 temp_TresholdVek[FFT_LENGTH2];

extern uint32_t Freq_Value[8];
extern uint32_t Freq_index;

float Phasearray[FFT_LENGTH];
int32_t Vibration_A[FFT_LENGTH], Vibration_A_w[FFT_LENGTH];


uint16_t  bladecnt = BLADECNT;
uint16_t sample_interval = LAB_MODE;
uint32_t samplerate = SAMPLERATE_NORM;

uint16_t dma_enabled = 1;

uint16_t flag=0;

uint16_t ADC_max_value, ADC_min_value;

int16_t no_of_peaks;

uint16_t debug;

uint32_t wheelfreq;
//uint32_t I2Q2[200];

uint16_t DMA_suspended = 0;

uint16_t PGAgainIDX[7] = {0,1,2,5,10,20,50};
uint16_t PGAConfig_Index[7] = {0,0x11,0x22,0x33,0x44,0x55,0x66};

uint16_t autogain = AUTOGAIN;
//uint16_t logging = LOGGING;

uint16_t rpm_updaterate = RPM_UPDATERATE;

uint8_t LCDGain;
//uint8_t LCDBackLight = BACKLIGHTINITSTATE;
//uint8_t LCDBackTimeout = LCDBACKLTIMEOUT;
//uint16_t LCDBackTimer = LCDBACKLTIMEOUT;



uint8_t PGA_gain = PGAGAIN;
uint16_t update_rate = UPDATERATE;


volatile unsigned long Timing_Delay = 0;

int16_t ADCBuffer[FFT_LENGTH2];
int16_t ADCBuffer1[FFT_LENGTH];
int16_t ADCBuffer2[FFT_LENGTH];

int16_t Excel1[FFT_LENGTH];
int16_t Excel2[FFT_LENGTH];

int32_t Freq1,Freq2,Freq3;
int32_t Maxpeak1,Maxpeak2,Maxpeak3, Maxpeakk, Maxpeak_d;
float Freq_Send;
#ifdef USE32BITFFT
int32_t HannBuf1[FFT_LENGTH2];
int32_t HannBuf2[FFT_LENGTH2];
int32_t HannBuf3[FFT_LENGTH2];
uint32_t pk_treshold = PK_TRSH;
#else
int16_t HannBuf1[FFT_LENGTH2];
int16_t HannBuf2[FFT_LENGTH2];
int16_t HannBuf3[FFT_LENGTH2];
uint16_t pk_treshold = PK_TRSH;
#endif

#ifdef MATLAB_OUTPUT
int32_t DeBuf1[FFT_LENGTH2], DeBuf2[FFT_LENGTH2];
int32_t DeHannBuf1[FFT_LENGTH2],DeHannBuf2[FFT_LENGTH2];
#endif

#ifdef USE32BITFFT
uint32_t peakValList[MAXPEAK];
#else
uint16_t peakValList[MAXPEAK];
#endif

float peakIdcsList[MAXPEAK];

extern int Fir_Filter_Toggle;

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup APP_VCP
  * @brief Mass storage application module
  * @{
  */

/** @defgroup APP_VCP_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup APP_VCP_Private_Defines
  * @{
  */

/**
  * @}
  */


/** @defgroup APP_VCP_Private_Macros
  * @{
  */
/**
  * @}
  */


/** @defgroup APP_VCP_Private_Variables
  * @{
  */

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

__ALIGN_BEGIN USB_OTG_CORE_HANDLE     USB_OTG_dev  __ALIGN_END ;

/**
  * @}
  */


/** @defgroup APP_VCP_Private_FunctionPrototypes
  * @{
  */
/**
  * @}
  */


/** @defgroup APP_VCP_Private_Functions
  * @{
  */

/**
  * @brief  Program entry point
  * @param  None
  * @retval None
  */

int main(void)
{
  __IO uint16_t aadd1 = 0;
  __IO uint16_t aadd2 = 0;
  SystemInit();
  Setup_USB_Status_LED();
  //stm32_serial_init();

  SoftSPI_Config();
  BLE_Init();
//  setupUART_RPM_out();
  SPI_Config(SPI_BaudRatePrescaler_8);   // ********************
  //ConfPGA(PGA_gain);

  ConfPGA(PGAConfig_Index[PGA_gain]);   //Felher !!: vorher ConfPGA(PGA_gain); richtig sollte ConfPGA(PGAConf_Index[PGA_gain])


//  initDAC_1();
//  setupRPMdivider();
//  Init_LCD();
//  init_Clickwheel();
  initBGT24();
  ConfBGT24(0x00, 0x00, 0x00, 0x00);

  //deinitadc();
  initadc(2);
  //Init menu.
//    extern const menudata mMenuData[];
//    init_menu(mMenuData, MENU_ENTRIES);
//    Callback1( 6 ); // Info Menü
//    ClickWheelState();
//    key_state = 0;
//    key_press = 0;
//    key_rpt = 0;
//  	BacklightON();
//  	delay_ms(500);
//  	Callback1( 1 ); // FFT Menü


  // Enable FPU
  //*((volatile unsigned long*)0xE000ED88) = 0xF << 20;

  /*!< At this stage the microcontroller clock setting is already configured,
  this is done through SystemInit() function which is called from startup
  file (startup_stm32fxxx_xx.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32fxxx.c file
  */

  USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS
            USB_OTG_HS_CORE_ID,
#else
            USB_OTG_FS_CORE_ID,
#endif
            &USR_desc,
            &USBD_CDC_cb,
            &USR_cb);

  gcode_init(usb_printf);

  // Configure Systick
  SysTick_Config((168000000 / 1000));

  // FATFS FatFs;   /* Work area (file system object) for logical drive */
  // FIL fil;       /* File object */
  char line[82]; /* Line buffer */
  // FRESULT fr;    /* FatFs return code */

  uint8_t flag = 0;

  /* Main loop */
  while(1)
  {
	/* Do infinite */
	//gcode_update();

  }
}


#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File: pointer to the source file name
* @param  Line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */


void Setup_USB_Status_LED(void){
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_DeInit(GPIOD);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}

int countt = 1, Amplicount = 0;
float sum = 0;
float Ampliarray[11];

int Send_Ok=0;

void SysTick_Handler(void)
{
	static int timestamp = 0;
	int16_t i,j;
	u32 avg_sum, debug;

	char char_buf[20]; // for itoa etc. function

	Timing_Delay--;
	timestamp++;




	if (timestamp % rpm_updaterate == 0) {
		//DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, DISABLE);

		// GPIO_SetBits(GPIOC,GPIO_Pin_0);
		while(DMA_suspended == 0);	// Wait that ADC Buffer is filled



		           //ADCBuffer);	// Remove DC and find ADC max. level



//#ifdef MATLAB_OUTPUT
				Send_Ok = 0;
				for(i=0; i<FFT_LENGTH;i++){
					DeBuf1[i] = (int32_t)ADCBuffer1[i<<1];
					DeBuf1[i+FFT_LENGTH] = (int32_t)ADCBuffer2[i<<1];
					/*DeBuf2[i] = (int32_t)ADCBuffer2[i];
					if(i % 2==0){
						Excel1[i >> 1] = ADCBuffer1[i];
						Excel2[i >> 1] = ADCBuffer2[i];
					}*/
				}
				Send_Ok = 1;
//#endif

				//Hanning_Window(ADCBuffer1, HannBuf1, FFT_LENGTH);   //2 Hanning_Window = 600us
				//Hanning_Window(ADCBuffer2, HannBuf2, FFT_LENGTH);


#ifdef MATLAB_OUTPUT
				for(i=0; i<FFT_LENGTH2;i++){
					DeHannBuf1[i] = HannBuf1[i];
					DeHannBuf2[i] = HannBuf2[i];
				}
#endif


			    #ifdef USE32BITFFT
					//GPIO_SetBits(GPIOC,GPIO_Pin_6);
					//fft_q31(HannBuf1, fft_output3, (uint32_t)FFT_LENGTH, (uint32_t) 0, (uint32_t) 1);
		    		//fft_q31(HannBuf2, fft_output3, (uint32_t)FFT_LENGTH, (uint32_t) 0, (uint32_t) 1);   // 2 FFT Calc 18.4 ms
		    		//GPIO_ResetBits(GPIOC,GPIO_Pin_6);
			    #else
		    		//fft_q15(HannBuf1, fft_output1, (uint32_t)FFT_LENGTH, (uint32_t) 0, (uint32_t) 1);
		    		//fft_q15(HannBuf2, fft_output2, (uint32_t)FFT_LENGTH, (uint32_t) 0, (uint32_t) 1);
			    #endif


		    	/*Freq1 = DetectFreq(fft_output3,(uint32_t)FFT_LENGTH);
		    	Maxpeak1 = Maxpeakk;
		    	Freq2 = DetectFreq(fft_output3,(uint32_t)FFT_LENGTH);   //2 DetectFreq 174 us
		    	Maxpeak2 = Maxpeakk;

		    	if(Freq1!=0) remove_DC_Vib(ADCBuffer1,1024/(int)((float)SAMPLERATE_NORM/Freq1));
		    	else remove_DC_Vib(ADCBuffer1,1024);
		    	if(Freq2!=0) remove_DC_Vib(ADCBuffer2,1024/(int)((float)SAMPLERATE_NORM/Freq2));
		    	else remove_DC_Vib(ADCBuffer2,1024);
		    	*/
		    	//if((Freq1<=Freq2)&&(Freq1!=0)) Maxpeak_d=Maxpeak1;
		    		//	                 else Maxpeak_d=Maxpeak2;

		    	//int min_Freq;
		    	//if((Freq1<=Freq2)&&(Freq1!=0)) min_Freq = Freq1;
		    	  //         	   	   	   	 else min_Freq = Freq2;

		    	//noch mal remove DC nach Anzahl von Perioden
				/*for(i=0;i<FFT_LENGTH;i++){
						ADCBuffer1[i] = testadc11[i];
						ADCBuffer2[i] = testadc22[i];
					}
*/
		    	//removeDC(number_of_Periode);

		    	/*for(i=0;i<FFT_LENGTH;i++){
				    ADCBuffer1[i]=ADCBuffer1[i<<1];
				    ADCBuffer2[i]=ADCBuffer2[i<<1];
				  }
*/
				remove_DC_Vib(ADCBuffer1,1024);
				remove_DC_Vib(ADCBuffer2,1024);

				//remove_DC_Vib(ADCBuffer1,2048);
			    //remove_DC_Vib(ADCBuffer2,2048);

/*
		    	for(i=0;i<FFT_LENGTH;i++){
		    		ADCBuffer1[i]=ADCBuffer1[i<<1];
		    		ADCBuffer2[i]=ADCBuffer2[i<<1];
		    	}
*/

				if(Fir_Filter_Toggle==1) {
					if(Freq_index==3){
						Fir_Filter(1, ADCBuffer1);
						Fir_Filter(1, ADCBuffer2);
					}
					else if(Freq_index==5){
						Fir_Filter(2, ADCBuffer1);
						Fir_Filter(2, ADCBuffer2);
					}
					else if(Freq_index==6){
						Fir_Filter(3, ADCBuffer1);
						Fir_Filter(3, ADCBuffer2);
					}

				}

				double DCx=0,DCy=0,DCr=0;
		    	double LMx=0,LMy=0,LMr=0;

		    	CircleFitByTaubin(FFT_LENGTH,ADCBuffer1,ADCBuffer2,&DCx,&DCy,&DCr);

		    	CircleFitByLevenbergMarquardtFull(FFT_LENGTH,ADCBuffer1,ADCBuffer2,DCx,DCy,DCr,0.01,&LMx,&LMy,&LMr);

		    	float Basis = 0;																									// A1


		    	for(i=0;i<FFT_LENGTH;i++){

		    		Phasearray[i] = atan2f(ADCBuffer2[i]-LMy,ADCBuffer1[i]-LMx);
		    		//I2Q2[i]=sqrt((ADCBuffer1[i<<1]*ADCBuffer1[i<<1])+(ADCBuffer2[i<<1]*ADCBuffer2[i<<1]));
		    		if(i>0){
		    			if(Phasearray[i] - Phasearray[i-1] >= PI)
		    				Basis = Basis - 2*PI;
		    			else if(Phasearray[i] - Phasearray[i-1] <= -PI)
		    				Basis = Basis + 2*PI;//Phasearray[i] - Phasearray[i-1];

		    		}
		    	    Unwrap_Phase[i] = Phasearray[i] + Basis;
		    		//if(Phasearray[i]<0) Phasearray[i] += 180;
		    		Vibration_A[i] = (int)(Unwrap_Phase[i]/4/PI*1250);															// A2 -- A2 19.6ms
		    		Vibration_A_w[i] = (int)(Phasearray[i]/4/PI*1250);

		    		//		    		if(Vibration_A[i]>maxAmpli)
//		    			maxAmpli = Vibration_A[i];
//		    		if(Vibration_A[i]<minAmpli)
//		    			minAmpli = Vibration_A[i];
		    	}

		    	//unwrap(Unwrap_Phase,1024);

		    	//for(i=0;i<FFT_LENGTH;i++){
		    	//	Vibration_A[i] = (int)(Unwrap_Phase[i]/4/PI*1250);															// A2 -- A2 19.6ms
		    	//	Vibration_A_w[i] = (int)(Phasearray[i]/4/PI*1250);
		    	//}

		    	remove_DC_Vib1(Vibration_A,1024);

		    	Hanning_Window_neu(Vibration_A, HannBuf3, FFT_LENGTH);
		    	fft_q31(HannBuf3, fft_output3, (uint32_t)FFT_LENGTH, (uint32_t) 0, (uint32_t) 1);
		    	Freq3 = DetectFreq(fft_output3,(uint32_t)FFT_LENGTH);
		    	Maxpeak3 = Maxpeakk;
		    	Maxpeak_d = Maxpeak3;
		    	no_of_peaks = Spectrum_Peakdetect(fft_output3, FFT_LENGTH, pk_treshold);



		        int Periodendauer = 1024/Freq3;///Freq_Value[Freq_index];
		    	int Anzahl_von_Periode=FFT_LENGTH/Periodendauer;
		        sum=0;
		    	if(Anzahl_von_Periode!=0){
					for(i=0;i<Anzahl_von_Periode;i++){
						maxAmpli = -10000;
						minAmpli = 10000;
						for(j=0;j<Periodendauer;j++){
							if(Vibration_A[i*Periodendauer+j]>maxAmpli)
							maxAmpli = Vibration_A[i*Periodendauer+j];
							if(Vibration_A[i*Periodendauer+j]<minAmpli)
							minAmpli = Vibration_A[i*Periodendauer+j];
						}
						sum = sum+(maxAmpli-minAmpli);
					}
					Ampliarray[1]=Ampliarray[2];
					Ampliarray[2]=Ampliarray[3];
					Ampliarray[3]=Ampliarray[4];
					Ampliarray[4]=sum/Anzahl_von_Periode;

					Ampli = (int)((Ampliarray[1]+Ampliarray[2]+Ampliarray[3]+Ampliarray[4])/4);//(int)(sum/number_of_Periode);//(maxAmpli - minAmpli) >> 1;
		    	}
		    	else Ampli=0;
		    	//sum += Ampli - Ampliarray[Amplicount];
		    	//Ampliarray[Amplicount++] = Ampli;
		    	//if(Amplicount>=10) Amplicount = 0;

		    	//Ampli = sum / 10;


		    			   //else BLE_Send(Freq2,(u32)Maxpeak2,Ampli);																				//A2


		    	//if((Freq1<=Freq2)&&(Freq1!=0)) no_of_peaks = Spectrum_Peakdetect(fft_output1, FFT_LENGTH, pk_treshold);
		    	//		                 else no_of_peaks = Spectrum_Peakdetect(fft_output2, FFT_LENGTH, pk_treshold);



		    		if(no_of_peaks>0) BLE_Send((int16_t)Freq_Send,(u32)Maxpeak3,Ampli);
		    		else BLE_Send(10000,1,1);


		    	Auto_Gain_AD(Freq3*1024/Freq_Value[Freq_index]);
		    	//debug = rotSpeed(0);
		    	//rpm2DAC( rotSpeedFundamental());//rotSpeed(0) );

		    	//wheelfreq = rotSpeed(0); // Speed rpm information for rectangular speed signal
		    	//wheelfreq = rotSpeedFundamental();
				//GPIO_ResetBits(GPIOC,GPIO_Pin_0);

				ADC_Cmd(ADC1, ENABLE);
				ADC_Cmd(ADC2, ENABLE);
				DMA_suspended = 0;

	}



//	if ((timestamp % update_rate == 0) && (update_rate > 0)) {
//
//		if (samplerate == SAMPLERATE_LAB) {
//			usb_printf(&USB_OTG_dev,"\n!!   Caution Lab. Mode Samplerate only 20kHz  !!\n");
//			usb_printf(&USB_OTG_dev,"\n!! NO alias Filter, be aware of folding Noise !!\n");
//		}
//		usb_printf(&USB_OTG_dev,"ADC max. value: %u\n", ADC_max_value);
//		usb_printf(&USB_OTG_dev,"ADC min. value: %u\n", ADC_min_value);
//		usb_printf(&USB_OTG_dev,"DC value: %u\n", mean_IIR);
//		usb_printf(&USB_OTG_dev,"PGA Gain: %u\n", PGA_gain);
//		usb_printf(&USB_OTG_dev,"\nPeak indices, rpm speed\n");
//		usb_printf(&USB_OTG_dev,"----------------------------------------\n");
//
//		for (i=0;i<no_of_peaks;i++){
//
//			usb_printf(&USB_OTG_dev,"Peak  [Hz] %u: %u\n",i,rotfreq(i));
//			usb_printf(&USB_OTG_dev,"Speed [rpm]%u: %u\n\n",i,rotSpeed(i));
//		}
//	}

}

//void DMA2_Stream0_IRQHandler(void){
//	DMA_InitTypeDef DMA_InitStructure;
//	int i;
//    // Interrupt service handler for full transfer
//    if( DMA_GetITStatus( DMA2_Stream0, DMA_IT_TCIF0 ) != RESET )
//    {
//    	ADC_DMACmd(ADC1, DISABLE);	// Disable DMA otherwise DMA data destroys array during FFT computation
//    	ADC_DMACmd(ADC2, DISABLE);	// Disable DMA otherwise DMA data destroys array during FFT computation
//    	DMA_suspended = 1;
//        pulsecnt = 0;
//    }
//
//    for(i=0;i<=500;i++){
//    	ADCBuffer1[i]=ADCBuffer[i*2];
//    	ADCBuffer2[i]=ADCBuffer[i*2+1];
//    }
//}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
