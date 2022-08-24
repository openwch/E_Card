
#include "string.h"
#include "QR_Encode.h"
//��ά������ԭ����� http://coolshell.cn/articles/10590.html#jtss-tsina

// P28 ��9 QR����Ÿ��汾�ľ�����Ч
const QR_VERSIONINFO QR_VersonInfo[] = {
    {0}, // Ver.0
    {1U, // Ver.1
     26U,
     {19U, 16U, 13U, 9U},
     0U,
     {0U, 0U, 0U, 0U, 0U, 0U},
     {{1U, 26U, 19U},
      {1U, 26U, 16U},
      {1U, 26U, 13U},
      {1U, 26U, 9U}},
     {{0U, 0U, 0U},
      {0U, 0U, 0U},
      {0U, 0U, 0U},
      {0U, 0U, 0U}}}};

// struct<unnamed>
// {
//     (uint16_t)1U, (uint16_t)26U, 
//     uint16_t[4]{(uint16_t)19U, (uint16_t)16U, (uint16_t)13U, (uint16_t)9U},
//     (uint16_t)0U, 
//     uint16_t[6]{(uint16_t)0U, (uint16_t)0U, (uint16_t)0U, (uint16_t)0U, (uint16_t)0U, (uint16_t)0U}, 
//     RS_BLOCKINFO[4]{struct<unnamed>{(uint16_t)1U, (uint16_t)26U, (uint16_t)19U}}, 
//     RS_BLOCKINFO[4]
//     {
//         struct<unnamed>{(uint16_t)1U, (uint16_t)26U, (uint16_t)16U}, struct<unnamed>{(uint16_t)1U, (uint16_t)26U, (uint16_t)13U}, struct<unnamed>{(uint16_t)1U, (uint16_t)26U, (uint16_t)9U}, struct<unnamed>
//         {
//             (uint16_t)0U, (uint16_t)0U, (uint16_t)0U
//         }
//     }
// }

// GF(2^8) ָ��2ϵ��������Դ����ʽ
/////////////////////////////////////////////////////////////////////////////
const BYTE byExpToInt[] = {1, 2, 4, 8, 16, 32, 64, 128, 29, 58, 116, 232, 205, 135, 19, 38,
                           76, 152, 45, 90, 180, 117, 234, 201, 143, 3, 6, 12, 24, 48, 96, 192,
                           157, 39, 78, 156, 37, 74, 148, 53, 106, 212, 181, 119, 238, 193, 159, 35,
                           70, 140, 5, 10, 20, 40, 80, 160, 93, 186, 105, 210, 185, 111, 222, 161,
                           95, 190, 97, 194, 153, 47, 94, 188, 101, 202, 137, 15, 30, 60, 120, 240,
                           253, 231, 211, 187, 107, 214, 177, 127, 254, 225, 223, 163, 91, 182, 113, 226,
                           217, 175, 67, 134, 17, 34, 68, 136, 13, 26, 52, 104, 208, 189, 103, 206,
                           129, 31, 62, 124, 248, 237, 199, 147, 59, 118, 236, 197, 151, 51, 102, 204,
                           133, 23, 46, 92, 184, 109, 218, 169, 79, 158, 33, 66, 132, 21, 42, 84,
                           168, 77, 154, 41, 82, 164, 85, 170, 73, 146, 57, 114, 228, 213, 183, 115,
                           230, 209, 191, 99, 198, 145, 63, 126, 252, 229, 215, 179, 123, 246, 241, 255,
                           227, 219, 171, 75, 150, 49, 98, 196, 149, 55, 110, 220, 165, 87, 174, 65,
                           130, 25, 50, 100, 200, 141, 7, 14, 28, 56, 112, 224, 221, 167, 83, 166,
                           81, 162, 89, 178, 121, 242, 249, 239, 195, 155, 43, 86, 172, 69, 138, 9,
                           18, 36, 72, 144, 61, 122, 244, 245, 247, 243, 251, 235, 203, 139, 11, 22,
                           44, 88, 176, 125, 250, 233, 207, 131, 27, 54, 108, 216, 173, 71, 142, 1};
// GF(2^8) ϵ��2ָ��������Դ����ʽ
/////////////////////////////////////////////////////////////////////////////
const BYTE byIntToExp[] = {0, 0, 1, 25, 2, 50, 26, 198, 3, 223, 51, 238, 27, 104, 199, 75,
                           4, 100, 224, 14, 52, 141, 239, 129, 28, 193, 105, 248, 200, 8, 76, 113,
                           5, 138, 101, 47, 225, 36, 15, 33, 53, 147, 142, 218, 240, 18, 130, 69,
                           29, 181, 194, 125, 106, 39, 249, 185, 201, 154, 9, 120, 77, 228, 114, 166,
                           6, 191, 139, 98, 102, 221, 48, 253, 226, 152, 37, 179, 16, 145, 34, 136,
                           54, 208, 148, 206, 143, 150, 219, 189, 241, 210, 19, 92, 131, 56, 70, 64,
                           30, 66, 182, 163, 195, 72, 126, 110, 107, 58, 40, 84, 250, 133, 186, 61,
                           202, 94, 155, 159, 10, 21, 121, 43, 78, 212, 229, 172, 115, 243, 167, 87,
                           7, 112, 192, 247, 140, 128, 99, 13, 103, 74, 222, 237, 49, 197, 254, 24,
                           227, 165, 153, 119, 38, 184, 180, 124, 17, 68, 146, 217, 35, 32, 137, 46,
                           55, 63, 209, 91, 149, 188, 207, 205, 144, 135, 151, 178, 220, 252, 190, 97,
                           242, 86, 211, 171, 20, 42, 93, 158, 132, 60, 57, 83, 71, 109, 65, 162,
                           31, 45, 67, 216, 183, 123, 164, 118, 196, 23, 73, 236, 127, 12, 111, 246,
                           108, 161, 59, 82, 41, 157, 85, 170, 251, 96, 134, 177, 187, 204, 62, 90,
                           203, 89, 95, 176, 156, 169, 160, 81, 11, 245, 22, 235, 122, 117, 44, 215,
                           79, 174, 213, 233, 230, 231, 173, 232, 116, 214, 244, 234, 168, 80, 88, 175};

//�������ֵ����ɶ���ʽ //P53 ��¼A
//���Ǽ�������2^8��    100011101         ��ʾ��ģ�����ʽ��
// X^8+X^4+X^3+X^2+1
/////////////////////////////////////////////////////////////////////////////
const BYTE byRSExp7[]  = {87, 229, 146, 149, 238, 102, 21};
const BYTE byRSExp10[] = {251, 67, 46, 61, 118, 70, 64, 94, 32, 45};
const BYTE byRSExp13[] = {74, 152, 176, 100, 86, 100, 106, 104, 130, 218, 206, 140, 78};
const BYTE byRSExp15[] = {8, 183, 61, 91, 202, 37, 51, 58, 58, 237, 140, 124, 5, 99, 105};
const BYTE byRSExp16[] = {120, 104, 107, 109, 102, 161, 76, 3, 91, 191, 147, 169, 182, 194, 225, 120};
const BYTE byRSExp17[] = {43, 139, 206, 78, 43, 239, 123, 206, 214, 147, 24, 99, 150, 39, 243, 163, 136};
const BYTE byRSExp18[] = {215, 234, 158, 94, 184, 97, 118, 170, 79, 187, 152, 148, 252, 179, 5, 98, 96, 153};
const BYTE byRSExp20[] = {17, 60, 79, 50, 61, 163, 26, 187, 202, 180, 221, 225, 83, 239, 156, 164, 212, 212, 188, 190};
const BYTE byRSExp22[] = {210, 171, 247, 242, 93, 230, 14, 109, 221, 53, 200, 74, 8, 172, 98, 80, 219, 134, 160, 105,
                          165, 231};
const BYTE byRSExp24[] = {229, 121, 135, 48, 211, 117, 251, 126, 159, 180, 169, 152, 192, 226, 228, 218, 111, 0, 117, 232,
                          87, 96, 227, 21};
const BYTE byRSExp26[] = {173, 125, 158, 2, 103, 182, 118, 17, 145, 201, 111, 28, 165, 53, 161, 21, 245, 142, 13, 102,
                          48, 227, 153, 145, 218, 70};
const BYTE byRSExp28[] = {168, 223, 200, 104, 224, 234, 108, 180, 110, 190, 195, 147, 205, 27, 232, 201, 21, 43, 245, 87,
                          42, 195, 212, 119, 242, 37, 9, 123};
const BYTE byRSExp30[] = {41, 173, 145, 152, 216, 31, 179, 182, 50, 48, 110, 86, 239, 96, 222, 125, 42, 173, 226, 193,
                          224, 130, 156, 37, 251, 216, 238, 40, 192, 180};
const BYTE byRSExp32[] = {10, 6, 106, 190, 249, 167, 4, 67, 209, 138, 138, 32, 242, 123, 89, 27, 120, 185, 80, 156,
                          38, 69, 171, 60, 28, 222, 80, 52, 254, 185, 220, 241};
const BYTE byRSExp34[] = {111, 77, 146, 94, 26, 21, 108, 19, 105, 94, 113, 193, 86, 140, 163, 125, 58, 158, 229, 239,
                          218, 103, 56, 70, 114, 61, 183, 129, 167, 13, 98, 62, 129, 51};
const BYTE byRSExp36[] = {200, 183, 98, 16, 172, 31, 246, 234, 60, 152, 115, 0, 167, 152, 113, 248, 238, 107, 18, 63,
                          218, 37, 87, 210, 105, 177, 120, 74, 121, 196, 117, 251, 113, 233, 30, 120};
const BYTE byRSExp38[] = {159, 34, 38, 228, 230, 59, 243, 95, 49, 218, 176, 164, 20, 65, 45, 111, 39, 81, 49, 118,
                          113, 222, 193, 250, 242, 168, 217, 41, 164, 247, 177, 30, 238, 18, 120, 153, 60, 193};
const BYTE byRSExp40[] = {59, 116, 79, 161, 252, 98, 128, 205, 128, 161, 247, 57, 163, 56, 235, 106, 53, 26, 187, 174,
                          226, 104, 170, 7, 175, 35, 181, 114, 88, 41, 47, 163, 125, 134, 72, 20, 232, 53, 35, 15};
const BYTE byRSExp42[] = {250, 103, 221, 230, 25, 18, 137, 231, 0, 3, 58, 242, 221, 191, 110, 84, 230, 8, 188, 106,
                          96, 147, 15, 131, 139, 34, 101, 223, 39, 101, 213, 199, 237, 254, 201, 123, 171, 162, 194, 117,
                          50, 96};
const BYTE byRSExp44[] = {190, 7, 61, 121, 71, 246, 69, 55, 168, 188, 89, 243, 191, 25, 72, 123, 9, 145, 14, 247,
                          1, 238, 44, 78, 143, 62, 224, 126, 118, 114, 68, 163, 52, 194, 217, 147, 204, 169, 37, 130,
                          113, 102, 73, 181};
const BYTE byRSExp46[] = {112, 94, 88, 112, 253, 224, 202, 115, 187, 99, 89, 5, 54, 113, 129, 44, 58, 16, 135, 216,
                          169, 211, 36, 1, 4, 96, 60, 241, 73, 104, 234, 8, 249, 245, 119, 174, 52, 25, 157, 224,
                          43, 202, 223, 19, 82, 15};
const BYTE byRSExp48[] = {228, 25, 196, 130, 211, 146, 60, 24, 251, 90, 39, 102, 240, 61, 178, 63, 46, 123, 115, 18,
                          221, 111, 135, 160, 182, 205, 107, 206, 95, 150, 120, 184, 91, 21, 247, 156, 140, 238, 191, 11,
                          94, 227, 84, 50, 163, 39, 34, 108};
const BYTE byRSExp50[] = {232, 125, 157, 161, 164, 9, 118, 46, 209, 99, 203, 193, 35, 3, 209, 111, 195, 242, 203, 225,
                          46, 13, 32, 160, 126, 209, 130, 160, 242, 215, 242, 75, 77, 42, 189, 32, 113, 65, 124, 69,
                          228, 114, 235, 175, 124, 170, 215, 232, 133, 205};
const BYTE byRSExp52[] = {116, 50, 86, 186, 50, 220, 251, 89, 192, 46, 86, 127, 124, 19, 184, 233, 151, 215, 22, 14,
                          59, 145, 37, 242, 203, 134, 254, 89, 190, 94, 59, 65, 124, 113, 100, 233, 235, 121, 22, 76,
                          86, 97, 39, 242, 200, 220, 101, 33, 239, 254, 116, 51};
const BYTE byRSExp54[] = {183, 26, 201, 87, 210, 221, 113, 21, 46, 65, 45, 50, 238, 184, 249, 225, 102, 58, 209, 218,
                          109, 165, 26, 95, 184, 192, 52, 245, 35, 254, 238, 175, 172, 79, 123, 25, 122, 43, 120, 108,
                          215, 80, 128, 201, 235, 8, 153, 59, 101, 31, 198, 76, 31, 156};
const BYTE byRSExp56[] = {106, 120, 107, 157, 164, 216, 112, 116, 2, 91, 248, 163, 36, 201, 202, 229, 6, 144, 254, 155,
                          135, 208, 170, 209, 12, 139, 127, 142, 182, 249, 177, 174, 190, 28, 10, 85, 239, 184, 101, 124,
                          152, 206, 96, 23, 163, 61, 27, 196, 247, 151, 154, 202, 207, 20, 61, 10};
const BYTE byRSExp58[] = {82, 116, 26, 247, 66, 27, 62, 107, 252, 182, 200, 185, 235, 55, 251, 242, 210, 144, 154, 237,
                          176, 141, 192, 248, 152, 249, 206, 85, 253, 142, 65, 165, 125, 23, 24, 30, 122, 240, 214, 6,
                          129, 218, 29, 145, 127, 134, 206, 245, 117, 29, 41, 63, 159, 142, 233, 125, 148, 123};
const BYTE byRSExp60[] = {107, 140, 26, 12, 9, 141, 243, 197, 226, 197, 219, 45, 211, 101, 219, 120, 28, 181, 127, 6,
                          100, 247, 2, 205, 198, 57, 115, 219, 101, 109, 160, 82, 37, 38, 238, 49, 160, 209, 121, 86,
                          11, 124, 30, 181, 84, 25, 194, 87, 65, 102, 190, 220, 70, 27, 209, 16, 89, 7, 33, 240};
const BYTE byRSExp62[] = {65, 202, 113, 98, 71, 223, 248, 118, 214, 94, 0, 122, 37, 23, 2, 228, 58, 121, 7, 105,
                          135, 78, 243, 118, 70, 76, 223, 89, 72, 50, 70, 111, 194, 17, 212, 126, 181, 35, 221, 117,
                          235, 11, 229, 149, 147, 123, 213, 40, 115, 6, 200, 100, 26, 246, 182, 218, 127, 215, 36, 186,
                          110, 106};
const BYTE byRSExp64[] = {45, 51, 175, 9, 7, 158, 159, 49, 68, 119, 92, 123, 177, 204, 187, 254, 200, 78, 141, 149,
                          119, 26, 127, 53, 160, 93, 199, 212, 29, 24, 145, 156, 208, 150, 218, 209, 4, 216, 91, 47,
                          184, 146, 47, 140, 195, 195, 125, 242, 238, 63, 99, 108, 140, 230, 242, 31, 204, 11, 178, 243,
                          217, 156, 213, 231};
const BYTE byRSExp66[] = {5, 118, 222, 180, 136, 136, 162, 51, 46, 117, 13, 215, 81, 17, 139, 247, 197, 171, 95, 173,
                          65, 137, 178, 68, 111, 95, 101, 41, 72, 214, 169, 197, 95, 7, 44, 154, 77, 111, 236, 40,
                          121, 143, 63, 87, 80, 253, 240, 126, 217, 77, 34, 232, 106, 50, 168, 82, 76, 146, 67, 106,
                          171, 25, 132, 93, 45, 105};
const BYTE byRSExp68[] = {247, 159, 223, 33, 224, 93, 77, 70, 90, 160, 32, 254, 43, 150, 84, 101, 190, 205, 133, 52,
                          60, 202, 165, 220, 203, 151, 93, 84, 15, 84, 253, 173, 160, 89, 227, 52, 199, 97, 95, 231,
                          52, 177, 41, 125, 137, 241, 166, 225, 118, 2, 54, 32, 82, 215, 175, 198, 43, 238, 235, 27,
                          101, 184, 127, 3, 5, 8, 163, 238};

const BYTE *byRSExp[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, byRSExp7, NULL, NULL,
                         byRSExp10, NULL, NULL, byRSExp13, NULL, byRSExp15, byRSExp16, byRSExp17, byRSExp18, NULL,
                         byRSExp20, NULL, byRSExp22, NULL, byRSExp24, NULL, byRSExp26, NULL, byRSExp28, NULL,
                         byRSExp30, NULL, byRSExp32, NULL, byRSExp34, NULL, byRSExp36, NULL, byRSExp38, NULL,
                         byRSExp40, NULL, byRSExp42, NULL, byRSExp44, NULL, byRSExp46, NULL, byRSExp48, NULL,
                         byRSExp50, NULL, byRSExp52, NULL, byRSExp54, NULL, byRSExp56, NULL, byRSExp58, NULL,
                         byRSExp60, NULL, byRSExp62, NULL, byRSExp64, NULL, byRSExp66, NULL, byRSExp68};

const BYTE nIndicatorLenNumeral[]  = {10, 12, 14};
const BYTE nIndicatorLenAlphabet[] = {9, 11, 13};
const BYTE nIndicatorLen8Bit[]     = {8, 16, 16};
const BYTE nIndicatorLenKanji[]    = {8, 10, 12};

int m_nSymbleSize;
BYTE m_byModuleData[MAX_MODULESIZE][MAX_MODULESIZE];

int m_ncDataCodeWordBit;
BYTE m_byDataCodeWord[MAX_DATACODEWORD];

int m_ncDataBlock;
BYTE m_byBlockMode[MAX_DATACODEWORD];
BYTE m_nBlockLength[MAX_DATACODEWORD];

int m_ncAllCodeWord;
BYTE m_byAllCodeWord[MAX_ALLCODEWORD];
BYTE m_byRSWork[MAX_CODEBLOCK];

int m_nLevel;
int m_nVersion;
char m_bAutoExtent;
int m_nMaskingNo;

//��������
// lpsSource Դ����
char EncodeData(char *lpsSource)
{
    int i, j, nVersion = 1, bAutoExtent = 1, ncSource;
    int ncLength, nEncodeVersion, ncDataCodeWord, ncTerminater;
    BYTE byPaddingCode = 0xec;
    int nDataCwIndex   = 0, ncBlock1, ncBlock2, ncBlockSum;
    int nBlockNo       = 0, ncDataCw1, ncDataCw2;
    int ncRSCw1, ncRSCw2;

    ncSource     = 0;
    m_nLevel     = 0;
    m_nMaskingNo = 0;

    ncLength = ncSource > 0 ? ncSource : strlen(lpsSource); //����Ҫ��������ݵĳ���

    // uprintf("ncLength = %d\n",ncLength);
    if (ncLength == 0)
        return FALSE;

    nEncodeVersion = GetEncodeVersion(nVersion, lpsSource, ncLength); //�������������Ҫ�İ汾

    // uprintf("nEncodeVersion = %d\n",nEncodeVersion);
    if (nEncodeVersion == 0)
        return FALSE;

    if (nVersion == 0)
    {
        m_nVersion = nEncodeVersion;
    }
    else
    {
        if (nEncodeVersion <= nVersion)
        {
            m_nVersion = nVersion;
        }
        else
        {
            if (bAutoExtent)
                m_nVersion = nEncodeVersion;
            else
                return FALSE;
        }
    }

    // uprintf("m_nVersion = %d\n",m_nVersion);

    ncDataCodeWord = QR_VersonInfo[m_nVersion].ncDataCodeWord[m_nLevel];

    ncTerminater = min(4, (ncDataCodeWord * 8) - m_ncDataCodeWordBit);

    if (ncTerminater > 0)
        m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, 0, ncTerminater);

    for (i = (m_ncDataCodeWordBit + 7) / 8; i < ncDataCodeWord; ++i)
    {
        m_byDataCodeWord[i] = byPaddingCode;

        byPaddingCode = (BYTE)(byPaddingCode == 0xec ? 0x11 : 0xec);
    }

    m_ncAllCodeWord = QR_VersonInfo[m_nVersion].ncAllCodeWord;
    memset(m_byAllCodeWord, 0, m_ncAllCodeWord);

    ncBlock1   = QR_VersonInfo[m_nVersion].RS_BlockInfo1[m_nLevel].ncRSBlock;
    ncBlock2   = QR_VersonInfo[m_nVersion].RS_BlockInfo2[m_nLevel].ncRSBlock;
    ncBlockSum = ncBlock1 + ncBlock2;

    ncDataCw1 = QR_VersonInfo[m_nVersion].RS_BlockInfo1[m_nLevel].ncDataCodeWord;
    ncDataCw2 = QR_VersonInfo[m_nVersion].RS_BlockInfo2[m_nLevel].ncDataCodeWord;

    for (i = 0; i < ncBlock1; ++i)
    {
        for (j = 0; j < ncDataCw1; ++j)
        {
            m_byAllCodeWord[(ncBlockSum * j) + nBlockNo] = m_byDataCodeWord[nDataCwIndex++];
        }

        ++nBlockNo;
    }

    for (i = 0; i < ncBlock2; ++i)
    {
        for (j = 0; j < ncDataCw2; ++j)
        {
            if (j < ncDataCw1)
            {
                m_byAllCodeWord[(ncBlockSum * j) + nBlockNo] = m_byDataCodeWord[nDataCwIndex++];
            }
            else
            {
                m_byAllCodeWord[(ncBlockSum * ncDataCw1) + i] = m_byDataCodeWord[nDataCwIndex++];
            }
        }

        ++nBlockNo;
    }

    ncRSCw1 = QR_VersonInfo[m_nVersion].RS_BlockInfo1[m_nLevel].ncAllCodeWord - ncDataCw1;
    ncRSCw2 = QR_VersonInfo[m_nVersion].RS_BlockInfo2[m_nLevel].ncAllCodeWord - ncDataCw2;

    nDataCwIndex = 0;
    nBlockNo     = 0;

    for (i = 0; i < ncBlock1; ++i)
    {
        memset(m_byRSWork, 0, sizeof(m_byRSWork));

        memmove(m_byRSWork, m_byDataCodeWord + nDataCwIndex, ncDataCw1);

        GetRSCodeWord(m_byRSWork, ncDataCw1, ncRSCw1);

        for (j = 0; j < ncRSCw1; ++j)
        {
            m_byAllCodeWord[ncDataCodeWord + (ncBlockSum * j) + nBlockNo] = m_byRSWork[j];
        }

        nDataCwIndex += ncDataCw1;
        ++nBlockNo;
    }

    for (i = 0; i < ncBlock2; ++i)
    {
        memset(m_byRSWork, 0, sizeof(m_byRSWork));

        memmove(m_byRSWork, m_byDataCodeWord + nDataCwIndex, ncDataCw2);

        GetRSCodeWord(m_byRSWork, ncDataCw2, ncRSCw2);

        for (j = 0; j < ncRSCw2; ++j)
        {
            m_byAllCodeWord[ncDataCodeWord + (ncBlockSum * j) + nBlockNo] = m_byRSWork[j];
        }

        nDataCwIndex += ncDataCw2;
        ++nBlockNo;
    }

    m_nSymbleSize = m_nVersion * 4 + 17;

    FormatModule();

    return TRUE;
}
//ÿ���汾�����ֶε�λ��
// nVersion	�汾
// lpsSource	Դ����
// ncLength	����
/*
Number of bits per length field
Encoding		Ver.1�C9		10�C26		27�C40
Numeric			10			12			14
Alphanumeric	9			11			13
Byte			8			16			16
Kanji			8			10			12
*/
int GetEncodeVersion(int nVersion, char *lpsSource, int ncLength)
{
    int nVerGroup = nVersion >= 27 ? QR_VRESION_L : (nVersion >= 10 ? QR_VRESION_M : QR_VRESION_S);
    int i, j;

    for (i = nVerGroup; i <= QR_VRESION_L; ++i)
    {
        if (EncodeSourceData(lpsSource, ncLength, i))
        {
            // if (i == QR_VRESION_S)
            // {
                for (j = 1; j <= 9; ++j)
                {
                    if ((m_ncDataCodeWordBit + 7) / 8 <= QR_VersonInfo[j].ncDataCodeWord[m_nLevel])
                        return j;
                }
            // }
            // else if (i == QR_VRESION_M)
            // {
            //     for (j = 10; j <= 26; ++j)
            //     {
            //         if ((m_ncDataCodeWordBit + 7) / 8 <= QR_VersonInfo[j].ncDataCodeWord[m_nLevel])
            //             return j;
            //     }
            // }
            // else if (i == QR_VRESION_L)
            // {
            //     for (j = 27; j <= 40; ++j)
            //     {
            //         if ((m_ncDataCodeWordBit + 7) / 8 <= QR_VersonInfo[j].ncDataCodeWord[m_nLevel])
            //             return j;
            //     }
            // }
        }
    }

    return 0;
}
//ģʽָ�꣬����ģʽ
// lpsSource Դ����
// ncLength	���ݳ���
// nVerGroup	�汾��
// char EncodeSourceData(char *lpsSource, int ncLength, int nVerGroup)
int EncodeSourceData(char *lpsSource, int ncLength, int nVerGroup)
{
    int i, j;
    int ncSrcBits, ncDstBits;
    int nBlock     = 0;
    int ncComplete = 0;
    WORD wBinCode;

    memset(m_nBlockLength, 0, sizeof(m_nBlockLength));

    for (m_ncDataBlock = i = 0; i < ncLength; ++i)
    {
        BYTE byMode;

        if(i < ncLength - 1 && IsChineseData(lpsSource[i], lpsSource[i + 1]))
            byMode = QR_MODE_CHINESE;			//����
        else if (IsNumeralData(lpsSource[i]))
            byMode = QR_MODE_NUMERAL; //����
        else if (IsAlphabetData(lpsSource[i]))
            byMode = QR_MODE_ALPHABET; //�ַ�
        else
            byMode = QR_MODE_8BIT; //�ֽ�

        if (i == 0)
            m_byBlockMode[0] = byMode; //

        if (m_byBlockMode[m_ncDataBlock] != byMode) //���ǰ���������ݵ�ģʽ��һ��
            m_byBlockMode[++m_ncDataBlock] = byMode;

        ++m_nBlockLength[m_ncDataBlock];
    }

    ++m_ncDataBlock;

    while (nBlock < m_ncDataBlock - 1)
    {
        int ncJoinFront, ncJoinBehind;
        int nJoinPosition = 0;

        if ((m_byBlockMode[nBlock] == QR_MODE_NUMERAL && m_byBlockMode[nBlock + 1] == QR_MODE_ALPHABET) ||
            (m_byBlockMode[nBlock] == QR_MODE_ALPHABET && m_byBlockMode[nBlock + 1] == QR_MODE_NUMERAL))
        {
            //���ǰ��2�����ݿ�Ϊ��ĸ�����֣����Ժϲ�����ĸ����ģʽ(��ĸģʽ����������)  ��������λ��
            ncSrcBits = GetBitLength(m_byBlockMode[nBlock], m_nBlockLength[nBlock], nVerGroup) +
                        GetBitLength(m_byBlockMode[nBlock + 1], m_nBlockLength[nBlock + 1], nVerGroup);

            ncDstBits = GetBitLength(QR_MODE_ALPHABET, m_nBlockLength[nBlock] + m_nBlockLength[nBlock + 1], nVerGroup);

            if (ncSrcBits > ncDstBits)
            {
                //����ϲ�ǰ���ںϲ��󳤶�
                if (nBlock >= 1 && m_byBlockMode[nBlock - 1] == QR_MODE_8BIT)
                {
                    //�ж�֮ǰ�����ݿ��ǲ���8bitģʽ������ǵĻ��ϲ�����(����Ҫ��3�����ݿ������ж�)
                    ncJoinFront = GetBitLength(QR_MODE_8BIT, m_nBlockLength[nBlock - 1] + m_nBlockLength[nBlock], nVerGroup) +
                                  GetBitLength(m_byBlockMode[nBlock + 1], m_nBlockLength[nBlock + 1], nVerGroup);

                    if (ncJoinFront > ncDstBits + GetBitLength(QR_MODE_8BIT, m_nBlockLength[nBlock - 1], nVerGroup))
                        ncJoinFront = 0; //����ϲ�������ݳ������� �Ƿ�
                }
                else
                    ncJoinFront = 0; //������ϲ�������

                if (nBlock < m_ncDataBlock - 2 && m_byBlockMode[nBlock + 2] == QR_MODE_8BIT)
                {
                    //�����������ݿ�Ϊ8bitģʽ���ϲ�����(����Ҫ��3�����ݿ������ж�)
                    ncJoinBehind = GetBitLength(m_byBlockMode[nBlock], m_nBlockLength[nBlock], nVerGroup) +
                                   GetBitLength(QR_MODE_8BIT, m_nBlockLength[nBlock + 1] + m_nBlockLength[nBlock + 2], nVerGroup);

                    if (ncJoinBehind > ncDstBits + GetBitLength(QR_MODE_8BIT, m_nBlockLength[nBlock + 2], nVerGroup))
                        ncJoinBehind = 0; //����ϲ�������ݳ������� �Ƿ�
                }
                else
                    ncJoinBehind = 0; //������ϲ�������

                if (ncJoinFront != 0 && ncJoinBehind != 0) //ǰ������ݿ�ϲ��󶼲�Ϊ0
                {
                    nJoinPosition = (ncJoinFront < ncJoinBehind) ? -1 : 1; //λ�õı�ǣ����ǰ���С�ں����Ϊ-1�����ڵ���Ϊ1
                }
                else
                {
                    //�����һ�����߶�Ϊ0		���ǰ�治Ϊ0Ϊ-1�����治Ϊ0Ϊ1  ����Ϊ0
                    nJoinPosition = (ncJoinFront != 0) ? -1 : ((ncJoinBehind != 0) ? 1 : 0);
                }

                if (nJoinPosition != 0)
                {
                    //����ϲ�������ݲ�Ϊ0
                    if (nJoinPosition == -1) //ǰ��������
                    {
                        m_nBlockLength[nBlock - 1] += m_nBlockLength[nBlock];

                        for (i = nBlock; i < m_ncDataBlock - 1; ++i)
                        {
                            m_byBlockMode[i]  = m_byBlockMode[i + 1];
                            m_nBlockLength[i] = m_nBlockLength[i + 1];
                        }
                    }
                    else
                    {
                        //����������
                        m_byBlockMode[nBlock + 1] = QR_MODE_8BIT;                 //�������ݿ�Ϊ8BITģʽ�����µ����ݿ�ģʽ�����8BIT
                        m_nBlockLength[nBlock + 1] += m_nBlockLength[nBlock + 2]; //����

                        for (i = nBlock + 2; i < m_ncDataBlock - 1; ++i)
                        {
                            m_byBlockMode[i]  = m_byBlockMode[i + 1];
                            m_nBlockLength[i] = m_nBlockLength[i + 1];
                        }
                    }

                    --m_ncDataBlock;
                }
                else
                {
                    //�ϲ��������쳣�����߲�����ϲ�����
                    if (nBlock < m_ncDataBlock - 2 && m_byBlockMode[nBlock + 2] == QR_MODE_ALPHABET)
                    {
                        //�������һ������Ҳ����ĸ���֣��ϲ�����
                        m_nBlockLength[nBlock + 1] += m_nBlockLength[nBlock + 2];

                        for (i = nBlock + 2; i < m_ncDataBlock - 1; ++i)
                        {
                            m_byBlockMode[i]  = m_byBlockMode[i + 1];
                            m_nBlockLength[i] = m_nBlockLength[i + 1];
                        }

                        --m_ncDataBlock;
                    }

                    //���������ݿ�Ϊ��ĸ����
                    m_byBlockMode[nBlock] = QR_MODE_ALPHABET;
                    m_nBlockLength[nBlock] += m_nBlockLength[nBlock + 1];

                    for (i = nBlock + 1; i < m_ncDataBlock - 1; ++i)
                    {
                        m_byBlockMode[i]  = m_byBlockMode[i + 1];
                        m_nBlockLength[i] = m_nBlockLength[i + 1];
                    }

                    --m_ncDataBlock;

                    //���ǰ��һ������Ҳ����ĸ���֣��ϲ�����
                    if (nBlock >= 1 && m_byBlockMode[nBlock - 1] == QR_MODE_ALPHABET)
                    {
                        m_nBlockLength[nBlock - 1] += m_nBlockLength[nBlock];

                        for (i = nBlock; i < m_ncDataBlock - 1; ++i)
                        {
                            m_byBlockMode[i]  = m_byBlockMode[i + 1];
                            m_nBlockLength[i] = m_nBlockLength[i + 1];
                        }

                        --m_ncDataBlock;
                    }
                }

                continue;
            }
        }

        ++nBlock;
    }

    nBlock = 0;

    while (nBlock < m_ncDataBlock - 1)
    {
        //�ϲ�2����������
        ncSrcBits = GetBitLength(m_byBlockMode[nBlock], m_nBlockLength[nBlock], nVerGroup) + GetBitLength(m_byBlockMode[nBlock + 1], m_nBlockLength[nBlock + 1], nVerGroup);

        ncDstBits = GetBitLength(QR_MODE_8BIT, m_nBlockLength[nBlock] + m_nBlockLength[nBlock + 1], nVerGroup);

        if (nBlock >= 1 && m_byBlockMode[nBlock - 1] == QR_MODE_8BIT) //ǰһ���ݿ�ΪBITģʽ
            ncDstBits -= (4 + nIndicatorLen8Bit[nVerGroup]);

        if (nBlock < m_ncDataBlock - 2 && m_byBlockMode[nBlock + 2] == QR_MODE_8BIT)
            ncDstBits -= (4 + nIndicatorLen8Bit[nVerGroup]);

        if (ncSrcBits > ncDstBits)
        {
            //����ϲ����������ӣ�ǰһ��������8bit����
            if (nBlock >= 1 && m_byBlockMode[nBlock - 1] == QR_MODE_8BIT)
            {
                m_nBlockLength[nBlock - 1] += m_nBlockLength[nBlock];

                for (i = nBlock; i < m_ncDataBlock - 1; ++i)
                {
                    m_byBlockMode[i]  = m_byBlockMode[i + 1];
                    m_nBlockLength[i] = m_nBlockLength[i + 1];
                }

                --m_ncDataBlock;
                --nBlock;
            }

            if (nBlock < m_ncDataBlock - 2 && m_byBlockMode[nBlock + 2] == QR_MODE_8BIT)
            {
                //���ݿ�+2Ҳ��8bit
                m_nBlockLength[nBlock + 1] += m_nBlockLength[nBlock + 2];

                for (i = nBlock + 2; i < m_ncDataBlock - 1; ++i)
                {
                    m_byBlockMode[i]  = m_byBlockMode[i + 1];
                    m_nBlockLength[i] = m_nBlockLength[i + 1];
                }

                --m_ncDataBlock;
            }

            //�����µ����ݿ�Ϊ8BITģʽ
            m_byBlockMode[nBlock] = QR_MODE_8BIT;
            m_nBlockLength[nBlock] += m_nBlockLength[nBlock + 1];

            for (i = nBlock + 1; i < m_ncDataBlock - 1; ++i)
            {
                m_byBlockMode[i]  = m_byBlockMode[i + 1];
                m_nBlockLength[i] = m_nBlockLength[i + 1];
            }

            --m_ncDataBlock;

            if (nBlock >= 1)
                --nBlock;

            continue;
        }

        ++nBlock;
    }

    m_ncDataCodeWordBit = 0;

    memset(m_byDataCodeWord, 0, MAX_DATACODEWORD);

    for (i = 0; i < m_ncDataBlock && m_ncDataCodeWordBit != -1; ++i)
    {
        if (m_byBlockMode[i] == QR_MODE_NUMERAL)
        {
            //������ݿ�������ģʽ
            m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, 1, 4);

            m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, (WORD)m_nBlockLength[i], nIndicatorLenNumeral[nVerGroup]);

            for (j = 0; j < m_nBlockLength[i]; j += 3)
            {
                if (j < m_nBlockLength[i] - 2)
                {
                    wBinCode = (WORD)(((lpsSource[ncComplete + j] - '0') * 100) +
                                      ((lpsSource[ncComplete + j + 1] - '0') * 10) +
                                      (lpsSource[ncComplete + j + 2] - '0'));

                    m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, wBinCode, 10);
                }
                else if (j == m_nBlockLength[i] - 2)
                {
                    wBinCode = (WORD)(((lpsSource[ncComplete + j] - '0') * 10) +
                                      (lpsSource[ncComplete + j + 1] - '0'));

                    m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, wBinCode, 7);
                }
                else if (j == m_nBlockLength[i] - 1)
                {
                    wBinCode = (WORD)(lpsSource[ncComplete + j] - '0');

                    m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, wBinCode, 4);
                }
            }

            ncComplete += m_nBlockLength[i];
        }

        else if (m_byBlockMode[i] == QR_MODE_ALPHABET)
        {
            m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, 2, 4);

            m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, (WORD)m_nBlockLength[i], nIndicatorLenAlphabet[nVerGroup]);

            for (j = 0; j < m_nBlockLength[i]; j += 2)
            {
                if (j < m_nBlockLength[i] - 1)
                {
                    wBinCode = (WORD)((AlphabetToBinaly(lpsSource[ncComplete + j]) * 45) +
                                      AlphabetToBinaly(lpsSource[ncComplete + j + 1]));

                    m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, wBinCode, 11);
                }
                else
                {
                    wBinCode = (WORD)AlphabetToBinaly(lpsSource[ncComplete + j]);

                    m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, wBinCode, 6);
                }
            }

            ncComplete += m_nBlockLength[i];
        }

        else if (m_byBlockMode[i] == QR_MODE_8BIT)
        {
            m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, 4, 4);

            m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, (WORD)m_nBlockLength[i], nIndicatorLen8Bit[nVerGroup]);

            for (j = 0; j < m_nBlockLength[i]; ++j)
            {
                m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, (WORD)lpsSource[ncComplete + j], 8);
            }

            ncComplete += m_nBlockLength[i];
        }
        else // if(m_byBlockMode[i] == QR_MODE_KANJI)
        {
            //�ձ�ģʽ
            m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, 8, 4);

            m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, (WORD)(m_nBlockLength[i] / 2), nIndicatorLenKanji[nVerGroup]);

            for (j = 0; j < m_nBlockLength[i] / 2; ++j)
            {
                WORD wBinCode = KanjiToBinaly((WORD)(((BYTE)lpsSource[ncComplete + (j * 2)] << 8) + (BYTE)lpsSource[ncComplete +
                                                                                                                    (j * 2) + 1]));

                m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, wBinCode, 13);
            }

            ncComplete += m_nBlockLength[i];
        }

        //		else
        //		{//����ģʽ
        //			m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, 8, 4);

        //			m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, (WORD)(m_nBlockLength[i] / 2), nIndicatorLenKanji[nVerGroup]);

        //			for (j = 0; j < m_nBlockLength[i] / 2; ++j)
        //			{
        //				WORD wBinCode = ChineseToBinaly((WORD)(((BYTE)lpsSource[ncComplete + (j * 2)] << 8) + (BYTE)lpsSource[ncComplete + (j * 2) + 1]));

        //				m_ncDataCodeWordBit = SetBitStream(m_ncDataCodeWordBit, wBinCode, 13);
        //			}

        //			ncComplete += m_nBlockLength[i];
        //		}
    }

    return (m_ncDataCodeWordBit != -1);
    // return TRUE;
}
//��ȡbit����
int GetBitLength(BYTE nMode, int ncData, int nVerGroup)
{
    int ncBits = 0;

    switch (nMode)
    {
        case QR_MODE_NUMERAL:
            ncBits = 4 + nIndicatorLenNumeral[nVerGroup] + (10 * (ncData / 3));

            switch (ncData % 3)
            {
                case 1:
                    ncBits += 4;
                    break;

                case 2:
                    ncBits += 7;
                    break;

                default:
                    break;
            }

            break;

        case QR_MODE_ALPHABET:
            ncBits = 4 + nIndicatorLenAlphabet[nVerGroup] + (11 * (ncData / 2)) + (6 * (ncData % 2));
            break;

        case QR_MODE_8BIT:
            ncBits = 4 + nIndicatorLen8Bit[nVerGroup] + (8 * ncData);
            break;

        default:
            // ncBits = 4 + nIndicatorLenKanji[nVerGroup] + (13 * (ncData / 2));//������д����
            ncBits = 4 + nIndicatorLenKanji[nVerGroup] + (13 * ncData); //�ĵ��㷨
            break;
    }

    return ncBits;
}
//����bit��
int SetBitStream(int nIndex, WORD wData, int ncData)
{
    int i;

    if (nIndex == -1 || nIndex + ncData > MAX_DATACODEWORD * 8)
        return -1;

    for (i = 0; i < ncData; ++i)
    {
        if (wData & (1 << (ncData - i - 1)))
        {
            m_byDataCodeWord[(nIndex + i) / 8] |= 1 << (7 - ((nIndex + i) % 8));
        }
    }

    return nIndex + ncData;
}
//�ǲ������ֱ�����Ϣ
//�Ƿ���TRUE���񷵻�FALSE
char IsNumeralData(unsigned char c)
{
    if (c >= '0' && c <= '9')
        return TRUE;

    return FALSE;
}
//�Ƿ����ַ�������Ϣ
//�Ƿ���TRUE���񷵻�FALSE
char IsAlphabetData(unsigned char c)
{
    if (c >= '0' && c <= '9')
        return TRUE;

    if (c >= 'A' && c <= 'Z')
        return TRUE;

    if (c == ' ' || c == '$' || c == '%' || c == '*' || c == '+' || c == '-' || c == '.' || c == '/' || c == ':')
        return TRUE;

    return FALSE;
}
//�Ƿ����ĺ�����Ϣ
//�Ƿ���TRUE���񷵻�FALSE
char IsKanjiData(unsigned char c1, unsigned char c2)
{

    if (((c1 >= 0x81 && c1 <= 0x9f) || (c1 >= 0xe0 && c1 <= 0xeb)) && (c2 >= 0x40))
    {
        if ((c1 == 0x9f && c2 > 0xfc) || (c1 == 0xeb && c2 > 0xbf))
            return FALSE;

        return TRUE;
    }

    return FALSE;
}
char IsChineseData(unsigned char c1, unsigned char c2)
{
    //����ģʽ
    if ((c1 >= 0xa1 && c1 < 0xaa) || (c1 >= 0xb0 && c1 <= 0xfa))
    {
        if (c2 >= 0xa1 && c2 <= 0xfe) return TRUE;
    }

    return FALSE;
}

//��ĸ����Ϣת��
BYTE AlphabetToBinaly(unsigned char c)
{
    if (c >= '0' && c <= '9') return (unsigned char)(c - '0');

    if (c >= 'A' && c <= 'Z') return (unsigned char)(c - 'A' + 10);

    if (c == ' ') return 36;

    if (c == '$') return 37;

    if (c == '%') return 38;

    if (c == '*') return 39;

    if (c == '+') return 40;

    if (c == '-') return 41;

    if (c == '.') return 42;

    if (c == '/') return 43;

    return 44;
}
//����������Ϣת��
//���ĺͺ��ֵı�����ȥһ��ֵ��
//�磺��0X8140 to 0X9FFC�е��ַ����ȥ8140��
//��0XE040��0XEBBF�е��ַ�Ҫ��ȥ0XC140��
//Ȼ��ѽ��ǰ����16����λ�ó�������0XC0��
//Ȼ���ټ��Ϻ�����16����λ�����ת��13bit�ı��롣
WORD KanjiToBinaly(WORD wc)
{
    if (wc >= 0x8140 && wc <= 0x9ffc)
        wc -= 0x8140;
    else
        wc -= 0xc140;

    return (WORD)(((wc >> 8) * 0xc0) + (wc & 0x00ff));
}
/*
�������ĺ��ֵ������ĺ���ת���������ƣ�
1�����ڵ�һ�ֽ�Ϊ0xA1~0xAA֮��,�ڶ��ֽ���0xA1~0xFE֮���ַ���
a)��һ�ֽڼ�ȥ0xA1��
b)��һ���������0x60;
c���ڶ��ֽڼ�ȥ0xA1;
d)��b)����Ľ������c����Ľ��;
e)�����ת��Ϊ13λ�����ƴ���
1�����ڵ�һ�ֽ�Ϊ0xB0~0xFA֮��,�ڶ��ֽ���0xA1~0xFE֮���ַ���
a)��һ�ֽڼ�ȥ0xA6��
b)��һ���������0x60;
c���ڶ��ֽڼ�ȥ0xA1;
d)��b)����Ľ������c����Ľ��;
e)�����ת��Ϊ13λ�����ƴ�
*/
WORD ChineseToBinaly(WORD wc)
{
    if (wc >= 0xa1a1 && wc <= 0xa9fe)
    {
        return (WORD)((((wc >> 8) - 0xa1) * 0x60) + ((wc & 0x00ff) - 0xa1));
    }

    if (wc >= 0xb0a1 && wc <= 0xf9fe)
    {
        return (WORD)((((wc >> 8) - 0xa6) * 0x60) + ((wc & 0x00ff) - 0xa1));
    }

    return (WORD)((((wc >> 8) - 0xa6) * 0x60) + ((wc & 0x00ff) - 0xa1));
}
//������
void GetRSCodeWord(BYTE *lpbyRSWork, int ncDataCodeWord, int ncRSCodeWord)
{
    int i, j;

    for (i = 0; i < ncDataCodeWord; ++i)
    {
        if (lpbyRSWork[0] != 0)
        {
            BYTE nExpFirst = byIntToExp[lpbyRSWork[0]];

            for (j = 0; j < ncRSCodeWord; ++j)
            {
                BYTE nExpElement = (BYTE)(((int)(byRSExp[ncRSCodeWord][j] + nExpFirst)) % 255);

                lpbyRSWork[j] = (BYTE)(lpbyRSWork[j + 1] ^ byExpToInt[nExpElement]);
            }

            for (j = ncRSCodeWord; j < ncDataCodeWord + ncRSCodeWord - 1; ++j)
                lpbyRSWork[j] = lpbyRSWork[j + 1];
        }
        else
        {
            for (j = 0; j < ncDataCodeWord + ncRSCodeWord - 1; ++j)
                lpbyRSWork[j] = lpbyRSWork[j + 1];
        }
    }
}
//��ʽ�����ݴ�ţ����������еĳߴ��У�
//���http://coolshell.cn/articles/10590.html#jtss-tsina
void FormatModule(void)
{
    int i, j;
    int nMinPenalty, nPenalty;

    memset(m_byModuleData, 0, sizeof(m_byModuleData));

    SetFunctionModule(); //���Ȱѻ���ͼ��������������

    SetCodeWordPattern(); //

    if (m_nMaskingNo == -1)
    {
        m_nMaskingNo = 0;

        SetMaskingPattern(m_nMaskingNo);
        SetFormatInfoPattern(m_nMaskingNo);

        nMinPenalty = CountPenalty();

        for (i = 1; i <= 7; ++i)
        {
            SetMaskingPattern(i);
            SetFormatInfoPattern(i);

            nPenalty = CountPenalty();

            if (nPenalty < nMinPenalty)
            {
                nMinPenalty  = nPenalty;
                m_nMaskingNo = i;
            }
        }
    }

    SetMaskingPattern(m_nMaskingNo);
    SetFormatInfoPattern(m_nMaskingNo);

    for (i = 0; i < m_nSymbleSize; ++i)
    {
        for (j = 0; j < m_nSymbleSize; ++j)
        {
            m_byModuleData[i][j] = (BYTE)((m_byModuleData[i][j] & 0x11) != 0);
        }
    }
}
//���ù�����ģ����Ϣ
// Position Detection Pattern	���ڱ�Ƕ�ά����д�С
// Separators for Postion Detection Patterns	�����Ϳ��Ա��һ������
// Timing PatternsҲ�����ڶ�λ�ġ�ԭ���Ƕ�ά����40�ֳߴ磬�ߴ�����˺���Ҫ�и���׼�ߣ���Ȼɨ���ʱ����ܻ�ɨ����
// Alignment Patterns ֻ��Version 2���ϣ�����Version2���Ķ�ά����Ҫ���������ͬ����Ϊ�˶�λ�õġ�
void SetFunctionModule(void)
{
    int i, j;

    SetFinderPattern(0, 0); //������������
    SetFinderPattern(m_nSymbleSize - 7, 0);
    SetFinderPattern(0, m_nSymbleSize - 7);

    for (i = 0; i < 8; ++i)
    {
        m_byModuleData[i][7] = m_byModuleData[7][i] = '\x20';
        m_byModuleData[m_nSymbleSize - 8][i] = m_byModuleData[m_nSymbleSize - 8 + i][7] = '\x20';
        m_byModuleData[i][m_nSymbleSize - 8] = m_byModuleData[7][m_nSymbleSize - 8 + i] = '\x20';
    }

    for (i = 0; i < 9; ++i)
    {
        m_byModuleData[i][8] = m_byModuleData[8][i] = '\x20';
    }

    for (i = 0; i < 8; ++i)
    {
        m_byModuleData[m_nSymbleSize - 8 + i][8] = m_byModuleData[8][m_nSymbleSize - 8 + i] = '\x20';
    }

    SetVersionPattern(); //�汾��Ϣ

    for (i = 0; i < QR_VersonInfo[m_nVersion].ncAlignPoint; ++i)
    {
        SetAlignmentPattern(QR_VersonInfo[m_nVersion].nAlignPoint[i], 6); //С����
        SetAlignmentPattern(6, QR_VersonInfo[m_nVersion].nAlignPoint[i]);

        for (j = 0; j < QR_VersonInfo[m_nVersion].ncAlignPoint; ++j)
        {
            SetAlignmentPattern(QR_VersonInfo[m_nVersion].nAlignPoint[i], QR_VersonInfo[m_nVersion].nAlignPoint[j]);
        }
    }

    for (i = 8; i <= m_nSymbleSize - 9; ++i)
    {
        m_byModuleData[i][6] = (i % 2) == 0 ? '\x30' : '\x20';
        m_byModuleData[6][i] = (i % 2) == 0 ? '\x30' : '\x20';
    }
}
//���ô������Ϣ
// O X X X X X X X
// O X O O O O O X
// O X O X X X O X
// O X O X X X O X
// O X O X X X O X
// O X O O O O O X
// O X X X X X X X
void SetFinderPattern(int x, int y)
{
    static BYTE byPattern[] = {0x7f,
                               0x41,
                               0x5d,
                               0x5d,
                               0x5d,
                               0x41,
                               0x7f};
    int i, j;

    for (i = 0; i < 7; ++i)
    {
        for (j = 0; j < 7; ++j)
        {
            m_byModuleData[x + j][y + i] = (byPattern[i] & (1 << (6 - j))) ? '\x30' : '\x20';
        }
    }
}
//����С������Ϣ
// Alignment Patterns ֻ��Version 2���ϣ�����Version2���Ķ�ά����Ҫ���������ͬ����Ϊ�˶�λ�õġ�
// Alignment Patterns�ǳ���3����Ļ��֣���С�Ļ���
// O O O X X X X X
// O O O X O O O X
// O O O X O X O X
// O O O X O O O X
// O O O X X X X X
void SetAlignmentPattern(int x, int y)
{
    static BYTE byPattern[] = {0x1f,
                               0x11,
                               0x15,
                               0x11,
                               0x1f};
    int i, j;

    if (m_byModuleData[x][y] & 0x20)
        return;

    x -= 2;
    y -= 2;

    for (i = 0; i < 5; ++i)
    {
        for (j = 0; j < 5; ++j)
        {
            m_byModuleData[x + j][y + i] = (byPattern[i] & (1 << (4 - j))) ? '\x30' : '\x20';
        }
    }
}
//���ð汾���� >= Version 7���ϣ���ҪԤ������3 x 6��������һЩ�汾��Ϣ��
void SetVersionPattern(void)
{
    int i, j;
    int nVerData;

    if (m_nVersion <= 6) //�� >= Version 7���ϣ���ҪԤ������3 x 6��������һЩ�汾��Ϣ��
        return;

    nVerData = m_nVersion << 12;

    for (i = 0; i < 6; ++i)
    {
        if (nVerData & (1 << (17 - i)))
        {
            nVerData ^= (0x1f25 << (5 - i));
        }
    }

    nVerData += m_nVersion << 12;

    for (i = 0; i < 6; ++i)
    {
        for (j = 0; j < 3; ++j)
        {
            m_byModuleData[m_nSymbleSize - 11 + j][i] = m_byModuleData[i][m_nSymbleSize - 11 + j] =
                (nVerData & (1 << (i * 3 + j))) ? '\x30' : '\x20';
        }
    }
}
//�������Ϊ���ǿ��Կ�ʼ��ͼ����ʹ��ˡ�
//��ά��Ļ��Ҽ�����û�����꣬����Ҫ��������;�����ĸ���codewords�������һ��
//��ν����أ��������£�
//���������룺��ÿ����ĵ�һ��codewords���ó�����˳�����кã�
//Ȼ����ȡ��һ��ĵڶ�����������ơ��磺����ʾ���е�Data Codewords����
void SetCodeWordPattern(void)
{
    int x = m_nSymbleSize;
    int y = m_nSymbleSize - 1;

    int nCoef_x = 1;
    int nCoef_y = 1;

    int i, j;

    for (i = 0; i < m_ncAllCodeWord; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            do
            {
                x += nCoef_x;
                nCoef_x *= -1;

                if (nCoef_x < 0)
                {
                    y += nCoef_y;

                    if (y < 0 || y == m_nSymbleSize)
                    {
                        y = (y < 0) ? 0 : m_nSymbleSize - 1;
                        nCoef_y *= -1;

                        x -= 2;

                        if (x == 6)
                            --x;
                    }
                }
            } while (m_byModuleData[x][y] & 0x20);

            m_byModuleData[x][y] = (m_byAllCodeWord[i] & (1 << (7 - j))) ? '\x02' : '\x00';
        }
    }
}
//�����ڱ���Ϣ
void SetMaskingPattern(int nPatternNo)
{
    int i, j;
    char bMask;

    for (i = 0; i < m_nSymbleSize; ++i)
    {
        for (j = 0; j < m_nSymbleSize; ++j)
        {
            if (!(m_byModuleData[j][i] & 0x20))
            {
                switch (nPatternNo)
                {
                    case 0:
                        bMask = ((i + j) % 2 == 0) ? TRUE : FALSE;
                        break;

                    case 1:
                        bMask = (i % 2 == 0) ? TRUE : FALSE;
                        break;

                    case 2:
                        bMask = (j % 3 == 0) ? TRUE : FALSE;
                        break;

                    case 3:
                        bMask = ((i + j) % 3 == 0) ? TRUE : FALSE;
                        break;

                    case 4:
                        bMask = (((i / 2) + (j / 3)) % 2 == 0) ? TRUE : FALSE;
                        break;

                    case 5:
                        bMask = (((i * j) % 2) + ((i * j) % 3) == 0) ? TRUE : FALSE;
                        break;

                    case 6:
                        bMask = ((((i * j) % 2) + ((i * j) % 3)) % 2 == 0) ? TRUE : FALSE;
                        break;

                    default:
                        bMask = ((((i * j) % 3) + ((i + j) % 2)) % 2 == 0) ? TRUE : FALSE;
                        break;
                }

                m_byModuleData[j][i] = (BYTE)((m_byModuleData[j][i] & 0xfe) | (((m_byModuleData[j][i] & 0x02) > 1) ^ bMask));
            }
        }
    }
}
//���ø�ʽ����Ϣ
void SetFormatInfoPattern(int nPatternNo)
{
    int nFormatInfo;
    int i;
    int nFormatData;

    switch (m_nLevel)
    {
        case QR_LEVEL_M:
            nFormatInfo = 0x00;
            break;

        case QR_LEVEL_L:
            nFormatInfo = 0x08;
            break;

        case QR_LEVEL_Q:
            nFormatInfo = 0x18;
            break;

        default:
            nFormatInfo = 0x10;
            break;
    }

    nFormatInfo += nPatternNo;

    nFormatData = nFormatInfo << 10;

    for (i = 0; i < 5; ++i)
    {
        if (nFormatData & (1 << (14 - i)))
        {
            nFormatData ^= (0x0537 << (4 - i));
        }
    }

    nFormatData += nFormatInfo << 10;

    nFormatData ^= 0x5412;

    for (i = 0; i <= 5; ++i)
        m_byModuleData[8][i] = (nFormatData & (1 << i)) ? '\x30' : '\x20';

    m_byModuleData[8][7] = (nFormatData & (1 << 6)) ? '\x30' : '\x20';
    m_byModuleData[8][8] = (nFormatData & (1 << 7)) ? '\x30' : '\x20';
    m_byModuleData[7][8] = (nFormatData & (1 << 8)) ? '\x30' : '\x20';

    for (i = 9; i <= 14; ++i)
        m_byModuleData[14 - i][8] = (nFormatData & (1 << i)) ? '\x30' : '\x20';

    for (i = 0; i <= 7; ++i)
        m_byModuleData[m_nSymbleSize - 1 - i][8] = (nFormatData & (1 << i)) ? '\x30' : '\x20';

    m_byModuleData[8][m_nSymbleSize - 8] = '\x30';

    for (i = 8; i <= 14; ++i)
        m_byModuleData[8][m_nSymbleSize - 15 + i] = (nFormatData & (1 << i)) ? '\x30' : '\x20';
}

int CountPenalty(void)
{
    int nPenalty = 0;
    int i, j, k;
    int nCount = 0, s_nCount;

    for (i = 0; i < m_nSymbleSize; ++i)
    {
        for (j = 0; j < m_nSymbleSize - 4; ++j)
        {
            int nCount = 1;

            for (k = j + 1; k < m_nSymbleSize; k++)
            {
                if (((m_byModuleData[i][j] & 0x11) == 0) == ((m_byModuleData[i][k] & 0x11) == 0))
                    ++nCount;
                else
                    break;
            }

            if (nCount >= 5)
            {
                nPenalty += 3 + (nCount - 5);
            }

            j = k - 1;
        }
    }

    for (i = 0; i < m_nSymbleSize; ++i)
    {
        for (j = 0; j < m_nSymbleSize - 4; ++j)
        {
            int nCount = 1;

            for (k = j + 1; k < m_nSymbleSize; k++)
            {
                if (((m_byModuleData[j][i] & 0x11) == 0) == ((m_byModuleData[k][i] & 0x11) == 0))
                    ++nCount;
                else
                    break;
            }

            if (nCount >= 5)
            {
                nPenalty += 3 + (nCount - 5);
            }

            j = k - 1;
        }
    }

    for (i = 0; i < m_nSymbleSize - 1; ++i)
    {
        for (j = 0; j < m_nSymbleSize - 1; ++j)
        {
            if ((((m_byModuleData[i][j] & 0x11) == 0) == ((m_byModuleData[i + 1][j] & 0x11) == 0)) &&
                (((m_byModuleData[i][j] & 0x11) == 0) == ((m_byModuleData[i][j + 1] & 0x11) == 0)) &&
                (((m_byModuleData[i][j] & 0x11) == 0) == ((m_byModuleData[i + 1][j + 1] & 0x11) == 0)))
            {
                nPenalty += 3;
            }
        }
    }

    for (i = 0; i < m_nSymbleSize; ++i)
    {
        for (j = 0; j < m_nSymbleSize - 6; ++j)
        {
            if (((j == 0) || (!(m_byModuleData[i][j - 1] & 0x11))) &&               // �� �܂��� �V���{���O
                (m_byModuleData[i][j] & 0x11) &&                                    // �� - 1
                (!(m_byModuleData[i][j + 1] & 0x11)) &&                             // �� - 1
                (m_byModuleData[i][j + 2] & 0x11) &&                                // �� ��
                (m_byModuleData[i][j + 3] & 0x11) &&                                // �� ��3
                (m_byModuleData[i][j + 4] & 0x11) &&                                // �� ��
                (!(m_byModuleData[i][j + 5] & 0x11)) &&                             // �� - 1
                (m_byModuleData[i][j + 6] & 0x11) &&                                // �� - 1
                ((j == m_nSymbleSize - 7) || (!(m_byModuleData[i][j + 7] & 0x11)))) // �� �܂��� �V���{���O
            {
                if (((j < 2 || !(m_byModuleData[i][j - 2] & 0x11)) &&
                     (j < 3 || !(m_byModuleData[i][j - 3] & 0x11)) &&
                     (j < 4 || !(m_byModuleData[i][j - 4] & 0x11))) ||
                    ((j >= m_nSymbleSize - 8 || !(m_byModuleData[i][j + 8] & 0x11)) &&
                     (j >= m_nSymbleSize - 9 || !(m_byModuleData[i][j + 9] & 0x11)) &&
                     (j >= m_nSymbleSize - 10 || !(m_byModuleData[i][j + 10] & 0x11))))
                {
                    nPenalty += 40;
                }
            }
        }
    }

    for (i = 0; i < m_nSymbleSize; ++i)
    {
        for (j = 0; j < m_nSymbleSize - 6; ++j)
        {
            if (((j == 0) || (!(m_byModuleData[j - 1][i] & 0x11))) &&               // �� �܂��� �V���{���O
                (m_byModuleData[j][i] & 0x11) &&                                    // �� - 1
                (!(m_byModuleData[j + 1][i] & 0x11)) &&                             // �� - 1
                (m_byModuleData[j + 2][i] & 0x11) &&                                // �� ��
                (m_byModuleData[j + 3][i] & 0x11) &&                                // �� ��3
                (m_byModuleData[j + 4][i] & 0x11) &&                                // �� ��
                (!(m_byModuleData[j + 5][i] & 0x11)) &&                             // �� - 1
                (m_byModuleData[j + 6][i] & 0x11) &&                                // �� - 1
                ((j == m_nSymbleSize - 7) || (!(m_byModuleData[j + 7][i] & 0x11)))) // �� �܂��� �V���{���O
            {
                if (((j < 2 || !(m_byModuleData[j - 2][i] & 0x11)) &&
                     (j < 3 || !(m_byModuleData[j - 3][i] & 0x11)) &&
                     (j < 4 || !(m_byModuleData[j - 4][i] & 0x11))) ||
                    ((j >= m_nSymbleSize - 8 || !(m_byModuleData[j + 8][i] & 0x11)) &&
                     (j >= m_nSymbleSize - 9 || !(m_byModuleData[j + 9][i] & 0x11)) &&
                     (j >= m_nSymbleSize - 10 || !(m_byModuleData[j + 10][i] & 0x11))))
                {
                    nPenalty += 40;
                }
            }
        }
    }

    for (i = 0; i < m_nSymbleSize; ++i)
    {
        for (j = 0; j < m_nSymbleSize; ++j)
        {
            if (!(m_byModuleData[i][j] & 0x11))
            {
                ++nCount;
            }
        }
    }

    if ((50 - ((nCount * 100) / (m_nSymbleSize * m_nSymbleSize))) > 0)
        s_nCount = 50 - ((nCount * 100) / (m_nSymbleSize * m_nSymbleSize));
    else
        s_nCount = 0 - (50 - ((nCount * 100) / (m_nSymbleSize * m_nSymbleSize)));

    nPenalty += (s_nCount / 5) * 10;

    return nPenalty;
}

void Print_2DCode(void)
{
    int i, j, k, x, y, z, size, mod;
    BYTE bit_h, bit_v, print_num; // print_buff[1024];
    BYTE print_buff[256];

    //--	SendCmd(3, 0x1b, 0x4a, 25);

    bit_h = 4;
    bit_v = 2;

    size = m_nSymbleSize / bit_v;
    mod  = m_nSymbleSize % bit_v;

    print_num = bit_h * m_nSymbleSize;

    for (i = 0; i < size; i++)
    {
        print_buff[0] = 0x1b;
        print_buff[1] = 0x2a;
        print_buff[2] = print_num;
        print_buff[3] = 0x00;
        x             = 4;

        for (j = 0; j < m_nSymbleSize; j++)
        {
            k = i * bit_v;

            if (m_byModuleData[j][k] == 1 && m_byModuleData[j][k + 1] == 1)
            {
                print_buff[x] = 0xFF;
                x++;
            }
            else if (m_byModuleData[j][k] == 1 && m_byModuleData[j][k + 1] == 0)
            {
                print_buff[x] = 0xF0;
                x++;
            }
            else if (m_byModuleData[j][k] == 0 && m_byModuleData[j][k + 1] == 1)
            {
                print_buff[x] = 0x0F;
                x++;
            }
            else
            {
                print_buff[x] = 0x00;
                x++;
            }

            k = k + 2;

            for (y = 0; y < (bit_h - 1); y++)
            {
                print_buff[x] = print_buff[x - 1];
                x++;
            }
        }

        for (z = 0; z < (print_num + 4); z++)
            ;

        //--		ser_send_char(1,print_buff[z]);

        //--		SendCmd(3, 0x1b, 0x4a, 0);
    }

    if (mod != 0)
    {
        print_buff[0] = 0x1b;
        print_buff[1] = 0x2a;
        print_buff[2] = print_num;
        print_buff[3] = 0x00;
        x             = 4;

        for (j = 0; j < m_nSymbleSize; j++)
        {
            i = size * bit_v;

            if (m_byModuleData[j][i] == 1)
            {
                print_buff[x] = 0xF0;
                x++;
            }
            else
            {
                print_buff[x] = 0x00;
                x++;
            }

            for (y = 0; y < (bit_h - 1); y++)
            {
                print_buff[x] = print_buff[x - 1];
                x++;
            }
        }

        for (z = 0; z < (print_num + 4); z++)
            ;

        //--		ser_send_char(1,print_buff[z]);

        //--		SendCmd(3, 0x1b, 0x4a, 0);
    }

    //--	SendCmd(3, 0x1b, 0x4a, 25);
}
