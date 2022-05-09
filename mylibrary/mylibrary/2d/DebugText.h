#pragma once

#include<d3d12.h>
#include<string>
#include<windows.h>

#include "Sprite.h"

//�f�o�b�O������N���X�̒�`
class DebugText
{
public://�萔
	static const int maxCharCount = 256;//�ő啶����
	static const int fontWidth = 9;  //�t�H���g�摜���P�������̉���
	static const int fontHeight = 18; //�t�H���g�摜��1�������̏c��
	static const int fontLineCount = 14; //�t�H���g
	static const int bufferSize = 256;	// �����t��������W�J�p�o�b�t�@�T�C�Y

public:// �ÓI�����o�֐�
	static DebugText* GetInstance();

public://�����o�֐�
	
	~DebugText();

	void Initialize(UINT texnumber);

	void Print(const std::string& text, float x, float y, float scale);
	void DrawAll(ID3D12GraphicsCommandList* cmdList);

private://�����o�ϐ�
	//�X�v���C�g�f�[�^�̔z��
	Sprite* sprites[maxCharCount];
	//�X�v���C�g�f�[�^�z��̓Y�����ԍ�
	int spriteIndex = 0;
};