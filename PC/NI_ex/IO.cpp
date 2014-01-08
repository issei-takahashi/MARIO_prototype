#include "IO.hpp"
#include "utils.hpp"

#define HERE IO
#define getcell(i,cells) cells.at(i)
#define getcelli(i,cells) utils::s2i(cells.at(i))
#define getcelld(i,cells) utils::s2d(cells.at(i))
#define INSERT(MAP,KEY,VAR) MAP.insert(make_pair(KEY,VAR)) 

Mutex                    HERE::mutex;
map< string, double >    HERE::m_const;
IO::ConfigData           HERE::configData( 1.0, 100, 700 );

namespace path
{
	static string const const_data = "data/const_data.csv";
	static string const config_latest = "config/latest.xml";
};

// loadPaths()�œǂݍ��񂾑S�Ẵp�X�̃f�[�^��ǂݍ���
void HERE::loadAllData()
{
		// �萔�f�[�^�icsv�j
	{
		HERE::loadConst(path::const_data);
	}
	// �R���t�B�O�f�[�^(xml)
	{
		HERE::loadConfigData(path::config_latest);
	}
}

// �萔�̒l���擾
double HERE::getConst( string const & _name )
{
	auto it = IO::m_const.find( _name );

	passert( _name+"�Ƃ����萔�͑��݂��܂���B", it != IO::m_const.end() );

	return it->second;
}

// �萔�f�[�^�ǂݍ���
void HERE::loadConst( stringc& _path )
{
	ifstream ifs( _path );
	//1�s���̃o�b�t�@
	string line;
	//�ŏ��̂P�s�͎̂Ă�
	getline( ifs, line );

	int lineCount = -1;
	while( ifs && getline( ifs, line ) ){
		lineCount++;
		vector<string> cells;
		utils::cutLine( line, cells );
		int i=0;
		if( cells.size() <= 1 ){ 
			continue;
		}
		string name = getcell(i++,cells);
		double var = getcelld(i++,cells);
		INSERT( HERE::m_const, name, var );
	}
}

// �p�X���w�肵�ăR���t�B�O�f�[�^(xml)�ǂݍ���
void HERE::loadConfigData( string const & _path )
{
	ifstream ifs( _path );
	// �t�@�C�����Ȃ�������
	if( !ifs ){
		ofstream ofs( _path );
		boost::archive::xml_oarchive oa(ofs);
		// �f�[�^����������
		oa << boost::serialization::make_nvp( "Root", HERE::configData );
	}
	// �t�@�C������������
	else{
		/// �������ɓǂݏo��
		boost::archive::xml_iarchive ia(ifs);
		ia >> boost::serialization::make_nvp( "Root", HERE::configData );
	}
}

void HERE::saveConfigData()
{
	IO::mutex.lock();

	ofstream ofs( path::config_latest );
	IO::configData.savedTime = Timer::getLocalTime();
	string timePath = "config/" + IO::configData.savedTime.getLocalTimeString() + ".xml";
	ofstream ofs2( timePath );
	boost::archive::xml_oarchive oa(ofs);
	boost::archive::xml_oarchive oa2(ofs2);
	// �f�[�^����������
	oa  << boost::serialization::make_nvp( "Root", HERE::configData );
	oa2 << boost::serialization::make_nvp( "Root", HERE::configData );
	cout << path::config_latest+" , "+timePath+" ��ۑ����܂����B" <<endl;

	IO::mutex.unlock();
}

void HERE::writeInputData( Point3D_mm _point )
{
	auto time = Timer::getLocalTime();
	static int lastHour = -999; // �K���Ȓl�ŏ�����
	static string fileName = "";
	static vector< pair<Timer::LocalTime,Point3D_mm> > buf;
	// ���Ԃ��ς���Ă�����
	if( time.hour != lastHour && !buf.empty() ){
		// �t�@�C������ݒ�
		fileName = "InteractionLog/" + utils::i2s( time.year ) + "_" + utils::i2s( time.month ) + "_" + utils::i2s( time.day ) + "_" + utils::i2s( time.hour-1 ) + ".csv";
		// �w�b�_��������
		std::ofstream ofs( fileName, std::ios::out | std::ios::app | std::ios::ate );
		ofs << "time,x(mm),y(mm),z(mm)" << std::endl;

		// �o�b�t�@�̒��g��������
		foreach(it,buf){
			string timeStr = utils::i2s( it->first.hour ) + ":" + utils::i2s( it->first.min ) + ":" + utils::i2s( it->first.sec );
			string data = utils::i2s( it->second.x ) + "," +  utils::i2s( it->second.y ) + "," +  utils::i2s( it->second.z );
			ofs << timeStr << "," << data << std::endl;
		}
		// �o�b�t�@�����
		buf.clear();	
	}

	// �o�b�t�@�����O
	buf.push_back( make_pair(time,_point) );

	// �ŏI�������L�^
	lastHour = time.hour;
}