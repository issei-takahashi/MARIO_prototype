// windowsスレッド関連
#include <windows.h>
#include <process.h>
#include <iostream>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#include "includes.hpp"

namespace io = boost::iostreams;

class MemMapFile
{
interface__:
	MemMapFile();
	virtual ~MemMapFile();

	// ファイルオープン
	virtual bool openFile( string _fileName, DWORD rwflag=GENERIC_READ | GENERIC_WRITE, DWORD openflag=OPEN_EXISTING);

	// ファイルストリーム取得
	iostream * getFileStream() const;

capcelled__:
	FILE* pPointer;   // ファイルポインタ
	DWORD dwFileSize; // ファイルサイズ
	sp< io::stream<io::file_descriptor> > pFileStream; // ファイルストリーム

inner__:
	HANDLE hFile;
	HANDLE hMap;
};