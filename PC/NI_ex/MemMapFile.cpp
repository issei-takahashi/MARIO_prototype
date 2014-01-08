#include "MemMapFile.hpp"

#include <stdio.h>

// メモリマップドファイル実装部

MemMapFile::MemMapFile()
	:hFile(INVALID_HANDLE_VALUE),hMap(0),pPointer(NULL),dwFileSize(0)
{

}


MemMapFile::~MemMapFile()
{
	UnmapViewOfFile( this->pPointer );
	if( this->hMap != 0 ){
		CloseHandle( this->hMap );
	}
	if( this->hFile != INVALID_HANDLE_VALUE ){
		CloseHandle( this->hFile );
	}
}

// ファイルオープン
bool MemMapFile::openFile( string _fileName, DWORD rwflag, DWORD openflag)
{
	/* ファイルオープン */
	this->hFile = CreateFile( _fileName.c_str(), rwflag, 0, 0, openflag, 0, 0 );
	if( this->hFile == INVALID_HANDLE_VALUE ){
		return false;
	}

	/* ファイルマッピングオブジェクトを作成 */
	DWORD mapflag = PAGE_READWRITE;
	if( rwflag == GENERIC_READ ){
		mapflag = PAGE_READONLY; // 読み込み専用に設定
	}
	this->hMap = CreateFileMapping( this->hFile, 0, mapflag, 0, 0, _fileName.c_str() );
	if( this->hMap <= 0 ){
		CloseHandle( this->hFile );
		this->hFile = INVALID_HANDLE_VALUE;
		return false;
	}

	/* ポインタを取得 */

	DWORD mapviewflag = FILE_MAP_WRITE;
	if( mapflag == PAGE_READONLY ){
		mapviewflag = FILE_MAP_READ;
	}
	this->pPointer = (FILE*)MapViewOfFile( this->hMap, mapviewflag, 0, 0, 0);
	if( this->pPointer == NULL){
		CloseHandle( this->hMap );
		CloseHandle( this->hFile );
		this->hMap = 0;
		this->hFile = INVALID_HANDLE_VALUE;
		return false;
	}

	/* ファイルサイズを取得 */

	DWORD high;
	this->dwFileSize = ::GetFileSize( this->hFile, &high );

	/* ファイルストリーム変換 */

	// ファイルポインタ→ファイルディスクリプタ
	int fd = fileno( this->pPointer );
	// ファイルディスクリプタ → boostファイルストリーム
	this->pFileStream = ( sp< io::stream<io::file_descriptor> > )( new io::stream<io::file_descriptor>( fd, io::never_close_handle) );

	//io::stream_buffer<io::file_descriptor_source> sb( io::file_descriptor_source( fd, io::never_close_handle) );

	return true;
}


// ファイルポインタ取得
iostream * MemMapFile::getFileStream() const
{
	return this->pFileStream.get();
}
