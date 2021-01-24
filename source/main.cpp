#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/FactoryLoader.hpp>

#include <cstdint>

#include <cdll_int.h>
#include <iachievementmgr.h>

static SourceSDK::FactoryLoader engine_loader( "engine" );

static IAchievementMgr *achievements = nullptr;

LUA_FUNCTION_STATIC( Get )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::NUMBER );

	int32_t idx = static_cast<int32_t>( LUA->GetNumber( 1 ) );
	if( idx < 0 || idx >= achievements->GetAchievementCount( ) )
		return 0;

	IAchievement *achievement = achievements->GetAchievementByIndex( idx );
	if( achievement == nullptr )
		return 0;

	LUA->CreateTable( );

	LUA->PushString( achievement->GetName( ) );
	LUA->SetField( -2, "name" );

	LUA->PushNumber( achievement->GetAchievementID( ) );
	LUA->SetField( -2, "id" );

	LUA->PushNumber( achievement->GetGoal( ) );
	LUA->SetField( -2, "goal" );

	LUA->PushNumber( achievement->GetPointValue( ) );
	LUA->SetField( -2, "pointvalue" );

	LUA->PushNumber( achievement->GetCount( ) );
	LUA->SetField( -2, "count" );

	LUA->PushNumber( achievement->GetFlags( ) );
	LUA->SetField( -2, "flags" );

	LUA->PushBool( achievement->IsAchieved( ) );
	LUA->SetField( -2, "achieved" );

	return 1;
}

LUA_FUNCTION_STATIC( Award )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::NUMBER );

	int idx = static_cast<int>( LUA->GetNumber( 1 ) );
	if( idx < 0 || idx >= achievements->GetAchievementCount( ) )
		return 0;

	IAchievement *achievement = achievements->GetAchievementByIndex( idx );
	if( achievement == nullptr )
		return 0;

	achievements->AwardAchievement( achievement->GetAchievementID( ) );
	return 0;
}

GMOD_MODULE_OPEN( )
{
	IVEngineClient *engine = engine_loader.GetInterface<IVEngineClient>("VEngineClient015");
	if( engine == nullptr )
		LUA->ThrowError( "unable to initialize IVEngineClient" );

	achievements = engine->GetAchievementMgr( );
	if( achievements == nullptr )
		LUA->ThrowError( "unable to initialize IAchievementManager" );

	LUA->GetField( GarrysMod::Lua::INDEX_GLOBAL, "achievements" );

	LUA->PushCFunction( Get );
	LUA->SetField( -2, "Get" );

	LUA->PushCFunction( Award );
	LUA->SetField( -2, "Award" );

	return 0;
}

GMOD_MODULE_CLOSE( )
{
	LUA->GetField( GarrysMod::Lua::INDEX_GLOBAL, "achievements" );

	LUA->PushNil( );
	LUA->SetField( -2, "Get" );

	LUA->PushNil( );
	LUA->SetField( -2, "Award" );

	return 0;
}
