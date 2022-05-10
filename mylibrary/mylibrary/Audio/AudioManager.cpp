#include "AudioManager.h"

#include <fstream>
#include <cassert>

#pragma comment(lib,"xaudio2.lib")


#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

HRESULT FindChunk(HANDLE hFile, DWORD forucc, DWORD* dwChunkSize, DWORD* dwChunkDataPosition)
{
	HRESULT hr;

	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	hr = S_OK;

	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return HRESULT_FROM_WIN32(GetLastError());

	while (hr == S_OK) {
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
			hr = HRESULT_FROM_WIN32(GetLastError());
		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL)
			== 0)
			hr = HRESULT_FROM_WIN32(GetLastError());
		switch (dwChunkType) {
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile,
				&dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
				hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		default:
			if (SetFilePointer(hFile,
				dwChunkDataSize, NULL, FILE_CURRENT)
				== INVALID_SET_FILE_POINTER)
				return HRESULT_FROM_WIN32(GetLastError());
		}
		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == forucc) {
			*dwChunkSize = dwChunkDataSize;
			*dwChunkDataPosition = dwOffset;
			return S_OK;
		}
		dwOffset += dwChunkDataSize;
		if (bytesRead >= dwRIFFDataSize) return S_FALSE;
	}

	return S_OK;
}

HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
	DWORD dwRead;

	if (SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN)
		== INVALID_SET_FILE_POINTER)
		return HRESULT_FROM_WIN32(GetLastError());

	if (ReadFile(hFile, buffer, buffersize, &dwRead, NULL) == 0)
		return HRESULT_FROM_WIN32(GetLastError());

	return S_OK;
}



bool AudioManager::LoadFile(const wchar_t* filename)
{

	HRESULT hr;
	hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		return HRESULT_FROM_WIN32(GetLastError());
	}
	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		return HRESULT_FROM_WIN32(GetLastError());
	}

	// Check Wave File.
	hr = FindChunk(hFile, fourccRIFF, &dwChunkSize, &dwChunkPosition);
	if (FAILED(hr)) return S_FALSE;
	hr = ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
	if (FAILED(hr)) return S_FALSE;
	if (filetype != fourccWAVE) return S_FALSE;

	// Get Format.
	hr = FindChunk(hFile, fourccFMT, &dwChunkSize, &dwChunkPosition);
	if (FAILED(hr)) return S_FALSE;
	hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	if (FAILED(hr)) return S_FALSE;

	// Load Sound.
	hr = FindChunk(hFile, fourccDATA, &dwChunkSize, &dwChunkPosition);
	if (FAILED(hr)) return S_FALSE;
	pDataBuffer = new BYTE[dwChunkSize];
	hr = ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);
	if (FAILED(hr)) return S_FALSE;

	// バッファの情報を設定
	buffer.AudioBytes = dwChunkSize;
	buffer.pAudioData = pDataBuffer;

}


AudioManager::~AudioManager()
{
	for (auto x : pSourceVoice)
	{
		x->DestroyVoice();
	}
	pSourceVoice.clear();
	pSourceVoice.shrink_to_fit();

	masterVoice->DestroyVoice();
	delete  pDataBuffer;
	/*CoUninitialize();*/
}


bool AudioManager::Initialize()
{
	HRESULT result;


	//result =CoInitializeEx(NULL, COINIT_MULTITHREADED);
	//if (FAILED(result))
	//{
	//	assert(0);
	//	return false;
	//}


	// XAudioエンジンのインスタンスを生成
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if FAILED(result) {
		assert(0);
		return false;
	}

	// マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	if FAILED(result) {
		assert(0);
		return false;
	}

	return true;
}

void AudioManager::PlayWave(const wchar_t* filename)
{
	HRESULT result;

	result = LoadFile(filename);
	if (FAILED(result))
	{
		assert(0);
		return;
	}

	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = 0;

	IXAudio2SourceVoice* sourceVoice = nullptr;

	// SourceVoiceの生成
	result = xAudio2->CreateSourceVoice(&sourceVoice, &(wfx.Format));
	if FAILED(result) {
		assert(0);
		return;
	}

	// 波形データの再生
	result = sourceVoice->SubmitSourceBuffer(&buffer);
	if FAILED(result) {
		assert(0);
		return;
	}

	result = sourceVoice->Start();
	if FAILED(result) {
		assert(0);
		return;
	}

	pSourceVoice.push_back(sourceVoice);

}

void AudioManager::PlayLoop(const wchar_t* filename)
{
	HRESULT result;


	result = LoadFile(filename);
	if (FAILED(result))
	{
		assert(0);
		return;
	}

	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	IXAudio2SourceVoice* sourceVoice = nullptr;


	// SourceVoiceの生成
	result = xAudio2->CreateSourceVoice(&sourceVoice, &(wfx.Format));
	if FAILED(result) {
		assert(0);
		return;
	}

	// 波形データの再生
	result = sourceVoice->SubmitSourceBuffer(&buffer);
	if FAILED(result) {
		assert(0);
		return;
	}

	result = sourceVoice->Start();
	if FAILED(result) {
		assert(0);
		return;
	}

	pSourceVoice.push_back(sourceVoice);
}


void AudioManager::Stop()
{
	for (auto x : pSourceVoice)
	{
		x->DestroyVoice();
	}
	pSourceVoice.clear();
	pSourceVoice.shrink_to_fit();
}