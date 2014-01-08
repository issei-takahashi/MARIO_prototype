#ifdef __MAYFES_MODE__

#include "Mover.hpp"
#include "Image.hpp"
#include "Block.h"
#include "GameMain.hpp"
#include <DxLib.h>

#define HERE Mover
#define IS_IN_AIR (this->aStandBlock==NULL)
#define CHANGE_IMAGE(STR) auto it=this->m_pImg.find(STR);assert(it!=this->m_pImg.end());if(it!=this->it_m_pImg){this->it_m_pImg=it;this->it_m_pImg->second->initIterator();}

//////////////////////// by DDT /////////////////////////////
void HERE::move()
{
	// 地形との当たり判定。　地形を見て movingState と vel を管理
	// 地形によってmovingState を変更
	HERE::ChangeMovingStateByField();

	// movingState によって速度を決定
	HERE::ChangeVelByMovingState();

}


///////////// by DDT ///////////////////////////
// 地形によってmovingState を変更
void HERE::ChangeMovingStateByField()
{
	//cout << movingState  << std::endl;
	// どの位ブロックの手前からジャンプするか
	int distance_for_jump = 70;

	// 空中にいるとき
	if( this->movingState == eMovingState::IN_AIR )
	{
		// 上方向に動いている（ジャンプ中）ならそのまま　着地判定を行わない
		if(this->vel.y <= 0)
		{
			this->movingState = eMovingState::IN_AIR ;
		}
		// 着地したら
		ef( FieldState::get()->JudgeLandOn(this) )
		{
			this->movingState = eMovingState::LAND_ON ;
		}

	}
	// 着地したら
	ef( this->movingState == eMovingState::LAND_ON )
	{
		// 歩行に移行
		this->movingState = eMovingState::STAND_ON_BLOCK;
	}
	// 歩いていたら
	ef( this->movingState == eMovingState::STAND_ON_BLOCK )
	{
		// ブロック端までの距離
		int distance_to_block_end = this->aStandBlock->p2.x - pos.x;

		// 下のブロックがなくなっていたら
		if( FieldState::get()->shitani_block_nai(this) )
		{
			this->movingState = eMovingState::IN_AIR;
		}
		// 前にブロックの壁があったら
		ef( FieldState::get()->maeni_block_takai(this) && (distance_to_block_end <= distance_for_jump ) )
		{
			this->movingState = eMovingState::STOP ;
		}
		// 前が平坦だったら
		ef( FieldState::get()->maeni_block_heimen(this) )
		{
			// ブロック端に来たら、自分の乗っているブロックを前方のブロックに更新
			if(distance_to_block_end <= 0)
			{
				if( this->index == 1 ) cout << "ブロック端到達, distance:" << distance_to_block_end << endl;
				//out std::vector<vector<Block> >& _v_v_Block = FieldState::get()->getBlock();

				this->standOnBlockX = this->standOnBlockX+1;
				this->standOnBlockY = this->standOnBlockY;
				//out aStandBlock = &_v_v_Block[this->standOnBlockX][this->standOnBlockY];
				this->aStandBlock = FieldState::get()->getaBlock(this->standOnBlockX,this->standOnBlockY);
			}
		}
		// 前のブロックが凸状態だったら
		ef( FieldState::get()->maeni_block_aru(this) && (distance_to_block_end <= distance_for_jump ) )
		{
			this->movingState = eMovingState::JUMP ;
		}
		// 前のブロックが凹状態だったら
		ef( FieldState::get()->maeni_block_hekomi(this) && (distance_to_block_end <= -10) ) // カドにひっかるため-10
		{
			this->movingState = eMovingState::IN_AIR ;
		}
		// 前のブロックがなかったら
		ef( FieldState::get()->maeni_block_zero(this)  && (distance_to_block_end <= 20) )
		{
			if( ( this->standOnBlockX <= 3 )
				 && (FieldState::get()->getDan(this->standOnBlockX+2) > this->standOnBlockY + 1) )
			{
				this->movingState = eMovingState::STOP ;
			}
			el
			{
				this->movingState = eMovingState::JUMP ;
			}
		}
	}
	// ジャンプしたら空中に移行
	ef( this->movingState == eMovingState::JUMP )
	{
		this->movingState = eMovingState::IN_AIR ;
	}
	// 止まっているとき
	ef( this->movingState == eMovingState::STOP )
	{
		if( FieldState::get()->shitani_block_nai(this) )
		{
			this->movingState = eMovingState::IN_AIR;
		}
		ef( FieldState::get()->maeni_block_takai(this) )
		{
			this->movingState = eMovingState::STOP ;
		}
		ef( FieldState::get()->maeni_block_heimen(this) )
		{
			this->movingState = eMovingState::STAND_ON_BLOCK ;
		}
		ef( FieldState::get()->maeni_block_aru(this) )
		{
			this->movingState = eMovingState::JUMP ;
		}
	}

}

// this->movingState によって速度を決定
void HERE::ChangeVelByMovingState()
{
	Point<int> walking_speed = Point<int>(3,0);

	if( this->movingState == eMovingState::IN_AIR )
	{
		// 重力加速
		this->vel.y += GRAV;

		// 最大速度で頭打ち
		PEAK(this->vel.y,VEL_MAX_Y);
	}
	ef( this->movingState == eMovingState::LAND_ON )
	{
		vel = walking_speed;
	}
	ef( this->movingState == eMovingState::STAND_ON_BLOCK )
	{
		vel = walking_speed;
	}
	ef( this->movingState == eMovingState::JUMP )
	{
		vel.x = walking_speed.x;
		vel.y = -18;
	}
	ef( this->movingState == eMovingState::STOP )
	{
		vel.x = 0;
		vel.y = 0;
	}

}

#endif