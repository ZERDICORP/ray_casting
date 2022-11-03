// athm.h

#ifdef _WIN32
	#include <ws2tcpip.h>
#endif
#include <cstring>
#include <dirent.h>
#include <ctime>
#include <random>
#include <map>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <sys/types.h>
#include <sys/stat.h>

#define mPi 3.14159265359
#define mDoublePi mPi * 2
#define mG 6.674184 * pow(10, -11)

#ifndef ZER__ATHM_MODEL
	#define ZER__ATHM_MODEL
		namespace zer
		{
			namespace athm
			{
				inline void rand_init();
				inline void sort(auto start, auto end, auto callback);

				template<typename TKey, typename TValue>
				inline std::vector<TKey> keys(std::map<TKey, TValue> const& map);
				template<typename TKey, typename TValue>
				inline std::vector<TValue> values(std::map<TKey, TValue> const& map);
				inline std::vector<std::string> listDir(std::string sDir);
				inline std::vector<std::string> listFolders(std::string sDir);
				inline std::vector<std::string> listFiles(std::string sDir);
				inline std::vector<std::string> split(std::string sString, std::string sSeparator);

				inline char controlCharToChar(char cControlChar);

				inline std::string getCurrentDateTime(std::string format);
				inline std::string collapseReps(std::string s);
				inline std::string expandReps(std::string s);
				inline std::string replace(std::string sString, std::string sSubstring, std::string sReplacement);

				template <typename T>
				inline T rand_choice(std::initializer_list<T> list, int iLen);
				
				inline int rand_int(int iTo);
				inline int rand_int(int iFrom, int iTo);
				inline int index(std::string sString, std::string sSubstring);

				inline float rand_float() {return (float)rand_int(10000) / (float)10000;}
				inline float toRadians(float fDegree) {return fDegree * (mPi / 180);}
				inline float toDegrees(float fRadians) {return fRadians / (mPi / 180);}
				inline float getAngleBetweenPoints(float fY1, float fX1, float fY2, float fX2) {return toRadians(180 / mPi * atan2f(fX1 - fX2, fY1 - fY2)) + toRadians(90);}
				inline float getDistance2D(float fY1, float fX1, float fY2, float fX2) {return sqrt(pow(fabs(fY1 - fY2), 2) + pow(fabs(fX1 - fX2), 2));}
				inline float sign(float f) {return f / fabs(f);}

				inline double gravityForce(double dM1, double dM2, double dR) {return mG * ((dM1 * dM2) / pow(dR, 2));}

				template <typename T>
				inline bool vectorHas(std::vector<T>& vec, const T& item) {return std::find(vec.begin(), vec.end(), item) != vec.end();}
				inline bool exists(std::string sPath);
				inline bool isFolder(std::string sPath);
				inline bool isFile(std::string sPath) {return !isFolder(sPath);}
				inline bool isInt(char chr);
				inline bool isNumber(std::string str);
				inline bool inRange2D(int fMatrixHeight, int fMatrixWidth, float fY, float fX) {return ((fY >= 0 && fY < fMatrixHeight) && (fX >= 0 && fX < fMatrixWidth));}

				#include "athm.inl"
			};
		};
#endif
