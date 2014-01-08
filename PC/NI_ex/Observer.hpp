#pragma once
#include "includes.hpp"

class Observer
{
	friend class Observer;
protected:
	Observer()
	{

	}
	~Observer()
	{
		foreach(it,this->m_aObject){
			it->second->deleteObj(this);
		}
	}
	virtual void addObj( Observer * _aObj, bool _endFlag = false )
	{
		this->m_aObject[_aObj] = _aObj;
		if( _endFlag == false ){
			_aObj->addObj(this,true);
		}
	}

	virtual void deleteObj( Observer * _aObj )
	{
		auto it = this->m_aObject.find(_aObj);
		assert(it!=this->m_aObject.end());
		this->m_aObject.erase(it);
	}

	void* getObjAddr( void* _pT ) const
	{
		Observer* ad = reinterpret_cast<Observer*>(_pT);
		auto it = this->m_aObject.find( ad );
		if(it!=this->m_aObject.end()){
			return _pT;
		}
		el{
			return NULL;
		}
	}

inner__:
	map<Observer*,Observer*> m_aObject; // ƒAƒhƒŒƒXˆê——

};