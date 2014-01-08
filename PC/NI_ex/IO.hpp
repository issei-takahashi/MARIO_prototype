#pragma once
#include "includes.hpp"
#include "Point.hpp"
#include "Rect.hpp"
#include "Mutex.hpp"
#include "Timer.hpp"
#include "Point3D_mm.h"

class IO
{
open__:
	static Mutex mutex;

interface__:
	// �S�Ẵf�[�^��ǂݍ���
	static void loadAllData();
	// �萔�̒l���擾
	static double getConst( string const & _name );
	// �R���t�B�O�f�[�^(xml)��������
	static void saveConfigData();
	// ����f�[�^�̏�������
	static void writeInputData( Point3D_mm _point );

	// configData��get
	static double get_zoom(){ return configData.zoom; }
	static Point<int> get_cameraPos() { return configData.cameraPos; }


	// ���s���ɏ��������ăZ�[�u����f�[�^
	class ConfigData
	{
		friend class boost::serialization::access;
	public:
		ConfigData( double _zoom, int _startx, int _starty ) 
			: zoom(_zoom), cameraPos( Point<int>(_startx,_starty) ) {}
		double zoom;           // �u���b�N��100px���牽�{�Ɋg�傷�邩
		Point<int> cameraPos;  // �t�B�[���h���J�n����u�`����W�v
		Point<int> startPoint; // �t�B�[���h��ŔF�����J�n����ʒu
		int zPixel;            // �u���b�N�񂪁Az�����ɉ��s�N�Z�����Ƃɂ��邩
		Rect<int> deskRect;    // �f�v�X�摜���Ŋ��̗̈��\����`
		Point<int> displayBiasPoint; // FieldState3D�ŕ`��̒��S�ʒu
		Timer::LocalTime savedTime;  // �ۑ����ꂽ���ԁi1�b���x�j
		int additionalActuatorDown_mm; // �f����]���ɉ�mm��O�ɏo�����i�A�N�`���G�[�^��]���ɉ�mm���Ɉړ����邩�j
		int additionalShadowNear_mm;      // �e��]���ɉ�mm��O�ɏo����
	private:
		ConfigData(){}
		//�V���A���C�Y�p
		template<class Archive>
		void serialize( Archive& ar, unsigned int ver )
		{
			ar & boost::serialization::make_nvp("savedTime", this->savedTime);
			ar & boost::serialization::make_nvp("zoom", this->zoom);
			ar & boost::serialization::make_nvp("cameraPos", this->cameraPos);
			ar & boost::serialization::make_nvp("startPoint", this->startPoint);
			ar & boost::serialization::make_nvp("zPixel", this->zPixel);
			ar & boost::serialization::make_nvp("deskRect", this->deskRect);
			ar & boost::serialization::make_nvp("displayBiasPoint", this->displayBiasPoint);
			ar & boost::serialization::make_nvp("additionalActuatorDown_mm", this->additionalActuatorDown_mm);
			ar & boost::serialization::make_nvp("additionalShadowNear_mm", this->additionalShadowNear_mm);
		}
	};

	static ConfigData configData;


capcelled__:
	static map< string, double > m_const;   // �萔�e�[�u��


inner__:
	// �p�X���w�肵�Ē萔�f�[�^(csv)�ǂݍ���
	static void loadConst( stringc& _path );
	// �p�X���w�肵�ăR���t�B�O�f�[�^(xml)�ǂݍ���
	static void loadConfigData( string const & _path );

inner__:

};