#pragma once


/*--------------- TemplateClass Singleton-----------------

目的：
シングルトンパターンの基底クラス。以下の3ステップでHogeクラ
スのシングルトンを実現する。
 1.型TをHogeに指定してSingletonを継承
 2.継承したHoge内でSingleton<Hoge>をfriendにする
 3.Hogeのコンストラクタ、デストラクタをprivate指定にする

属性:テンプレート

--------------------------------------------------------*/

template< class T >
class Singleton
{
public:
	static T* get()
	{
		static T instance;
		return &instance;
	}
private:
	// コピー禁止
	Singleton( const T& other ){}
	Singleton& operator = ( const Singleton& other ){}
protected:
	Singleton(){}
	virtual ~Singleton(){}
};
