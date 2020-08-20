// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_PL_ID_H__
#define __KK_PL_ID_H__

struct kkPluginID
{
	void * m_data     = nullptr;
	u32  m_dataSize = 0;
};

// ������������ � �������� ��� �������������
// ����������� ������� ���������� ���� ����� ��� ��������. 
// ��� �� ����������� �������� ������� ������ �������� ����� �� ������ �������� �������
class kkPluginIDData
{
public:
	kkPluginIDData(){}
	virtual ~kkPluginIDData(){}

	kkPluginCommonInterface* m_ci = nullptr;

	u32 m_uniqueNumber = 0;
	virtual u32 returnMagic(){ return KK_MAKEFOURCC('K','K','R','O');} 
};

#endif