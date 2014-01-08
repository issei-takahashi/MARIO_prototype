#pragma once

#include "includes.hpp"

/*------------------- Class Timer ---------------------------

�ړI�F���ݎ����i�~���b�j�̌v��
�����F���m�X�e�[�g

--------------------------------------------------------*/

class Timer
{
	friend class GameMain;
open__:
	class LocalTime
	{
		friend class boost::serialization::access;
	public:
		string getLocalTimeString() const;
		int year;  // �N
		int month; // ��
		int day;   // ��
		int hour;  // ��
		int min;   // ��
		int sec;   // �b
	private:
		//�V���A���C�Y�p
		template<class Archive>
		void serialize( Archive& ar, unsigned int ver )
		{
			ar & boost::serialization::make_nvp("year", this->year);
			ar & boost::serialization::make_nvp("month",this->month );
			ar & boost::serialization::make_nvp("day", this->day );
			ar & boost::serialization::make_nvp("hour",this->hour );
			ar & boost::serialization::make_nvp("min",this->min );
			ar & boost::serialization::make_nvp("sec",this->sec );
		}
	} localTime ;
interface__:
	// ��Ύ��Ԍnget
	static Timer::LocalTime getLocalTime();

	// �t���[���nget
	static double GetLastTime(){return lastTime;}
	static double GetFrameTime(){return frameTime;}
	static int    GetFrameNum(){ return frameNum; }

inner__:
	static double lastTime; //���O�̌v������(s)
	static double frameTime; //1�t���[���ɂ�����������(s)
	static int frameNum; //�@�Q�[���N��������̃t���[����

inner__:  //GameMain�݂̂���Ăяo��
	static void setLastTime(); //�ŏI�v�����Ԃ̑���
	static void setFrameTime(); //1�t���[���ɂ����������Ԃ̑���
	static void setStartTime(); //�v���J�n���Ԃ̑���
};