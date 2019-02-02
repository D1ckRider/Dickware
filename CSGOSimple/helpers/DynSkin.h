#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <map>
#endif /* WIN32_LEAN_AND_MEAN */

#include <Windows.h>

#include <fstream>
#include <vector>
#include <unordered_map>
#include <iostream>

#pragma warning(disable : 4227)

namespace DynSkin 
{
	namespace File
	{
		typedef unsigned int szPos;
		typedef szPos szSize;
		typedef std::vector<szPos> vecPos;
		typedef std::vector<char> vecFile;

		class CFile
		{
		public:
			CFile(void);
			virtual                             ~CFile(void);
			bool                                Load(const std::string& path, bool isunicode = false);
			void                                Release(void);
			szPos                               FindFirstOf(const std::string& search, szPos start, szPos end);
			vecPos                              FindAllOf(const std::string& search, szPos start, szPos end, vecPos pos = vecPos());
			std::string                         GetStringAt(szPos start, szSize length);
			std::string                         GetStringBetween(szPos start, szPos end);
			inline const std::string& const     GetPath(void) { return _path; }
			inline const vecFile& const         GetFile(void) { return _file; }
			inline const szPos& const           GetFileSize(void) { return _file.size(); }
			inline const char& const            at(std::size_t p) { return _file.at(p); }
		private:
			virtual bool Parse(void) = 0;

			inline bool CompareBytes(char* pszData, const char* pszPattern)
			{
				for (auto i = 0; *pszPattern; ++pszPattern, ++i) {
					if (pszData[i] != *pszPattern) {
						return false;
					}
				}
				return true;
			}
		protected:
			std::string                         _path;          // filepath
			vecFile                             _file;          // file std::binary | std::ate
			CFile*                              _instance;      // instance
		};
	}

	namespace File 
	{
		namespace Valve
		{
			class CLevel;

			typedef std::multimap< std::string, CLevel* >        mapLevel;
			typedef std::unordered_map< std::string, std::string >    mapVariable;

			class CLevel
			{
			public:

				CLevel(szPos& start, CFile* file);
				~CLevel(void);

				inline const mapLevel& const        GetSubLevels(void) { return _sublevels; }
				inline const mapVariable& const     GetVariables(void) { return _variables; }
			protected:
				szPos                               _start = 0;            // abs. pos start
				szPos                               _end = 0;            // abs. pos end
				mapLevel                            _sublevels;            // Sublevel
				mapVariable                         _variables;            // Variables
			};

			class CConfig : public CFile
			{
			public:
				CConfig(void);
				~CConfig(void);
				inline CLevel*                      GetLevel(void) { return _level; }
			private:
				bool                                Parse(void);
			protected:
				CLevel*                             _level = 0;            // Level
			};
		}
	}

	namespace Skins
	{
		struct SkinInfo_t
		{
			unsigned int    _id;        // index u know for what

			std::string     _shortname; // shortname
			std::string     _name;      // full skin name
		};

		typedef std::vector<SkinInfo_t> vecSkinInfo;
		typedef std::unordered_map<std::string, vecSkinInfo> mapSkinInfo;

		class CSkins
		{
		public:
			CSkins(void);
			~CSkins(void);
			bool								DumpTable(File::Valve::mapLevel tab, File::Valve::mapVariable vt, File::Valve::mapLevel ai_wi);
			bool                                Load(const std::string& gamePath, const std::string& gameShortName);
			void                                Release(void);
			void                                Dump(std::ofstream& dump);

			inline const mapSkinInfo& const GetSkinInfo(void) { return _skininfo; }
			inline const vecSkinInfo& const GetSkinInfoByWeapon(const std::string& weapon)
			{
				auto& ret = _skininfo.find(weapon);
				if (ret != _skininfo.end())
					return ret->second;
				return vecSkinInfo();
			}


		protected:
			File::Valve::CConfig*               _items_game = nullptr; // config file by valve
			File::Valve::CConfig*               _csgo_english = nullptr; // config file by valve
			mapSkinInfo                         _skininfo; // Holds Skinsinformation
		};
	}
};

#pragma warning(default : 4227)
