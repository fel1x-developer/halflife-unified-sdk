/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#pragma once

#include "gamerules.h"
#include "pm_materials.h"
#include "ctf/CTFDefs.h"
#include "palette.h"

class CRope;

#define PLAYER_FATAL_FALL_SPEED 1024															  // approx 60 feet
#define PLAYER_MAX_SAFE_FALL_SPEED 580															  // approx 20 feet
#define DAMAGE_FOR_FALL_SPEED (float)100 / (PLAYER_FATAL_FALL_SPEED - PLAYER_MAX_SAFE_FALL_SPEED) // damage per unit per second.
#define PLAYER_MIN_BOUNCE_SPEED 200
#define PLAYER_FALL_PUNCH_THRESHHOLD (float)350 // won't punch player's screen/make scrape noise unless player falling at least this fast.

//
// Player PHYSICS FLAGS bits
//
#define PFLAG_ONLADDER (1 << 0)
#define PFLAG_ONSWING (1 << 0)
#define PFLAG_ONTRAIN (1 << 1)
#define PFLAG_ONBARNACLE (1 << 2)
#define PFLAG_DUCKING (1 << 3)	// In the process of ducking, but totally squatted yet
#define PFLAG_USING (1 << 4)	// Using a continuous entity
#define PFLAG_OBSERVER (1 << 5) // player is locked in stationary cam mode. Spectators can move, observers can't.
#define PFLAG_ONROPE (1 << 6)

//
// generic player
//
//-----------------------------------------------------
//This is Half-Life player entity
//-----------------------------------------------------
#define CSUITPLAYLIST 4 // max of 4 suit sentences queued up at any time

#define SUIT_GROUP true
#define SUIT_SENTENCE false

#define SUIT_REPEAT_OK 0
#define SUIT_NEXT_IN_30SEC 30
#define SUIT_NEXT_IN_1MIN 60
#define SUIT_NEXT_IN_5MIN 300
#define SUIT_NEXT_IN_10MIN 600
#define SUIT_NEXT_IN_30MIN 1800
#define SUIT_NEXT_IN_1HOUR 3600

#define CSUITNOREPEAT 32

#define SOUND_FLASHLIGHT_ON "items/flashlight1.wav"
#define SOUND_FLASHLIGHT_OFF "items/flashlight1.wav"

#define SOUND_NIGHTVISION_ON "items/nightvision1.wav"
#define SOUND_NIGHTVISION_OFF "items/nightvision2.wav"

#define TEAM_NAME_LENGTH 16

typedef enum
{
	PLAYER_IDLE,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_SUPERJUMP,
	PLAYER_DIE,
	PLAYER_ATTACK1,
	PLAYER_GRAPPLE,
} PLAYER_ANIM;

#define MAX_ID_RANGE 2048
#define SBAR_STRING_SIZE 128

enum sbar_data
{
	SBAR_ID_TARGETNAME = 1,
	SBAR_ID_TARGETHEALTH,
	SBAR_ID_TARGETARMOR,
	SBAR_END,
};

enum Cheat
{
	InfiniteAir = 0,
	InfiniteArmor,
};

class CBasePlayer : public CBaseMonster
{
public:
	// Spectator camera
	void Observer_FindNextPlayer(bool bReverse);
	void Observer_HandleButtons();
	void Observer_SetMode(int iMode);
	void Observer_CheckTarget();
	void Observer_CheckProperties();
	EHANDLE m_hObserverTarget;
	float m_flNextObserverInput;
	int m_iObserverWeapon;	 // weapon of current tracked target
	int m_iObserverLastMode; // last used observer mode
	bool IsObserver() { return 0 != pev->iuser1; }

	int random_seed; // See that is shared between client & server for shared weapons code

	Vector m_DisplacerReturn;
	int m_DisplacerSndRoomtype;

	int m_iPlayerSound;		// the index of the sound list slot reserved for this player
	int m_iTargetVolume;	// ideal sound volume.
	int m_iWeaponVolume;	// how loud the player's weapon is right now.
	int m_iExtraSoundTypes; // additional classification for this weapon's sound
	int m_iWeaponFlash;		// brightness of the weapon flash
	float m_flStopExtraSoundTime;

	/**
	*	@brief What type of suit light the player can use
	*	@details The initial value here is the default
	*	To change this setting at runtime call @see SetSuitLightType so that player state is updated properly
	*/
	SuitLightType m_SuitLightType = SuitLightType::Flashlight;
	float m_flFlashLightTime; // Time until next battery draw/Recharge
	int m_iFlashBattery;	  // Flashlight Battery Draw

	int m_afButtonLast;
	int m_afButtonPressed;
	int m_afButtonReleased;

	EHANDLE m_SndLast;	   // last sound entity to modify player room type
	int m_SndRoomtype = 0; // last roomtype set by sound entity. Defaults to 0 on new maps to disable it by default.
	int m_ClientSndRoomtype;
	float m_flSndRange; // dist from player to sound entity

	float m_flFallVelocity;

	int m_rgItems[MAX_ITEMS];
	bool m_fKnownItem; // True when a new item needs to be added
	int m_fNewAmmo;	   // True when a new item has been added

	unsigned int m_afPhysicsFlags; // physics flags - set when 'normal' physics should be revisited or overriden
	float m_fNextSuicideTime;	   // the time after which the player can next use the suicide command


	// these are time-sensitive things that we keep track of
	float m_flTimeStepSound;  // when the last stepping sound was made
	float m_flTimeWeaponIdle; // when to play another weapon idle animation.
	float m_flSwimTime;		  // how long player has been underwater
	float m_flDuckTime;		  // how long we've been ducking
	float m_flWallJumpTime;	  // how long until next walljump

	float m_flSuitUpdate;						 // when to play next suit update
	int m_rgSuitPlayList[CSUITPLAYLIST];		 // next sentencenum to play for suit update
	int m_iSuitPlayNext;						 // next sentence slot for queue storage;
	int m_rgiSuitNoRepeat[CSUITNOREPEAT];		 // suit sentence no repeat list
	float m_rgflSuitNoRepeatTime[CSUITNOREPEAT]; // how long to wait before allowing repeat
	int m_lastDamageAmount;						 // Last damage taken
	float m_tbdPrev;							 // Time-based damage timer

	float m_flgeigerRange; // range to nearest radiation source
	float m_flgeigerDelay; // delay per update of range msg to client
	int m_igeigerRangePrev;
	int m_iStepLeft;						// alternate left/right foot stepping sound
	char m_szTextureName[CBTEXTURENAMEMAX]; // current texture name we're standing on
	char m_chTextureType;					// current texture type

	int m_idrowndmg;	  // track drowning damage taken
	int m_idrownrestored; // track drowning damage restored

	int m_bitsHUDDamage; // Damage bits for the current fame. These get sent to
						 // the hude via the DAMAGE message
	bool m_fInitHUD;	 // True when deferred HUD restart msg needs to be sent
	bool m_fGameHUDInitialized;
	int m_iTrain;	// Train control position
	bool m_fWeapon; // Set this to false to force a reset of the current weapon HUD info

	EHANDLE m_pTank;		 // the tank which the player is currently controlling,  nullptr if no tank
	EHANDLE m_hViewEntity;	 // The view entity being used, or null if the player is using itself as the view entity
	bool m_bResetViewEntity; //True if the player's view needs to be set back to the view entity
	float m_fDeadTime;		 // the time at which the player died  (used in PlayerDeathThink())

	bool m_fNoPlayerSound; // a debugging feature. Player makes no sound if this is true.
	bool m_fLongJump;	   // does this player have the longjump module?

	float m_tSneaking;
	int m_iUpdateTime;	  // stores the number of frame ticks before sending HUD update messages
	int m_iClientHealth;  // the health currently known by the client.  If this changes, send a new
	int m_iClientBattery; // the Battery currently known by the client.  If this changes, send a new
	int m_iHideHUD;		  // the players hud weapon info is to be hidden
	int m_iClientHideHUD;
	int m_iFOV;		  // field of view
	int m_iClientFOV; // client's known FOV

	// Opposing Force specific

	const char* m_szTeamModel;
	CTFTeam m_iTeamNum;
	CTFTeam m_iNewTeamNum;
	CTFItem::CTFItem m_iItems;
	unsigned int m_iClientItems;
	EHANDLE m_pFlag;
	int m_iCurrentMenu;
	float m_flNextHEVCharge;
	float m_flNextHealthCharge;
	float m_flNextAmmoCharge;
	int m_iLastPlayerTrace;
	bool m_fPlayingHChargeSound;
	bool m_fPlayingAChargeSound;
	int m_nLastShotBy;
	float m_flLastShotTime;
	int m_iFlagCaptures;
	int m_iCTFScore;
	bool m_fWONAuthSent;

	short m_iOffense;
	short m_iDefense;
	short m_iSnipeKills;
	short m_iBarnacleKills;
	short m_iSuicides;
	float m_flLastDamageTime;
	short m_iLastDamage;
	short m_iMostDamage;
	float m_flAccelTime;
	float m_flBackpackTime;
	float m_flHealthTime;
	float m_flShieldTime;
	float m_flJumpTime;

	// usable player items
	CBasePlayerItem* m_rgpPlayerItems[MAX_ITEM_TYPES];
	CBasePlayerItem* m_pActiveItem;
	CBasePlayerItem* m_pClientActiveItem; // client version of the active item
	CBasePlayerItem* m_pLastItem;

	std::uint64_t m_WeaponBits;

	//Not saved, used to update client.
	std::uint64_t m_ClientWeaponBits;

	// shared ammo slots
	int m_rgAmmo[MAX_AMMO_SLOTS];
	int m_rgAmmoLast[MAX_AMMO_SLOTS];

	Vector m_vecAutoAim;
	bool m_fOnTarget;
	int m_iDeaths;
	float m_iRespawnFrames; // used in PlayerDeathThink() to make sure players can always respawn

	int m_lastx, m_lasty; // These are the previous update's crosshair angles, DON"T SAVE/RESTORE

	int m_nCustomSprayFrames; // Custom clan logo frames for this player
	float m_flNextDecalTime;  // next time this player can spray a decal

	char m_szTeamName[TEAM_NAME_LENGTH];

	void Spawn() override;
	void Pain();

	//	void Think() override;
	virtual void Jump();
	virtual void Duck();
	virtual void PreThink();
	virtual void PostThink();
	Vector GetGunPosition() override;
	bool TakeHealth(float flHealth, int bitsDamageType) override;
	void TraceAttack(entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType) override;
	bool TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	void Killed(entvars_t* pevAttacker, int iGib) override;
	Vector BodyTarget(const Vector& posSrc) override { return Center() + pev->view_ofs * RANDOM_FLOAT(0.5, 1.1); } // position to shoot at
	void StartSneaking() override { m_tSneaking = gpGlobals->time - 1; }
	void StopSneaking() override { m_tSneaking = gpGlobals->time + 30; }
	bool IsSneaking() override { return m_tSneaking <= gpGlobals->time; }
	bool IsAlive() override { return (pev->deadflag == DEAD_NO) && pev->health > 0; }
	bool ShouldFadeOnDeath() override { return false; }
	bool IsPlayer() override { return true; } // Spectators should return false for this, they aren't "players" as far as game logic is concerned

	bool IsNetClient() override { return true; } // Bots should return false for this, they can't receive NET messages
												 // Spectators should return true for this
	const char* TeamID() override;

	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	void RenewItems();
	void PackDeadPlayerItems();
	void RemoveAllItems(bool removeSuit);
	bool SwitchWeapon(CBasePlayerItem* pWeapon);

	/**
	*	@brief Equips an appropriate weapon for the player if they don't have one equipped already.
	*/
	void EquipWeapon();

	void SetWeaponBit(int id);
	void ClearWeaponBit(int id);

	bool HasSuit() const;
	void SetHasSuit(bool hasSuit);

	// JOHN:  sends custom messages if player HUD data has changed  (eg health, ammo)
	virtual void UpdateClientData();

	void UpdateCTFHud();

	static TYPEDESCRIPTION m_playerSaveData[];

	// Player is moved across the transition by other means
	int ObjectCaps() override { return CBaseMonster::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	void Precache() override;
	bool IsOnLadder();

	int GetFlashlightFlag() const;

	bool FlashlightIsOn();
	void FlashlightTurnOn();
	void FlashlightTurnOff();

	void SetSuitLightType(SuitLightType type);

	void UpdatePlayerSound();
	void DeathSound() override;

	int Classify() override;
	void SetAnimation(PLAYER_ANIM playerAnim);
	char m_szAnimExtention[32];

	// custom player functions
	virtual void ImpulseCommands();
	void CheatImpulseCommands(int iImpulse);

	void StartDeathCam();
	void StartObserver(Vector vecPosition, Vector vecViewAngle);

	void AddPoints(int score, bool bAllowNegativeScore) override;
	void AddPointsToTeam(int score, bool bAllowNegativeScore) override;
	bool AddPlayerItem(CBasePlayerItem* pItem) override;
	bool RemovePlayerItem(CBasePlayerItem* pItem) override;
	void DropPlayerItem(const char* pszItemName);
	bool HasPlayerItem(CBasePlayerItem* pCheckItem);
	bool HasNamedPlayerItem(const char* pszItemName);
	bool HasWeapons(); // do I have ANY weapons?
	void SelectPrevItem(int iItem);
	void SelectNextItem(int iItem);
	void SelectLastItem();
	void SelectItem(const char* pstr);
	void ItemPreFrame();
	void ItemPostFrame();
	void GiveNamedItem(const char* szName);
	void GiveNamedItem(const char* szName, int defaultAmmo);
	void EnableControl(bool fControl);

	int GiveAmmo(int iAmount, const char* szName, int iMax) override;
	void SendAmmoUpdate();
	void SendSingleAmmoUpdate(int ammoIndex);

private:
	void InternalSendSingleAmmoUpdate(int ammoIndex);

public:
	void WaterMove();
	void EXPORT PlayerDeathThink();
	void PlayerUse();

	void CheckSuitUpdate();
	void SetSuitUpdate(const char* name, bool fgroup, int iNoRepeat);
	void UpdateGeigerCounter();
	void CheckTimeBasedDamage();

	bool FBecomeProne() override;
	void BarnacleVictimBitten(entvars_t* pevBarnacle) override;
	void BarnacleVictimReleased() override;
	static int GetAmmoIndex(const char* psz);
	int AmmoInventory(int iAmmoIndex);
	int Illumination() override;

	void ResetAutoaim();
	Vector GetAutoaimVector(float flDelta);
	Vector GetAutoaimVectorFromPoint(const Vector& vecSrc, float flDelta);
	Vector AutoaimDeflection(const Vector& vecSrc, float flDist, float flDelta);

	void ForceClientDllUpdate(); // Forces all client .dll specific data to be resent to client.

	void SetCustomDecalFrames(int nFrames);
	int GetCustomDecalFrames();

	void TabulateAmmo();

	float m_flStartCharge;
	float m_flAmmoStartCharge;
	float m_flPlayAftershock;
	float m_flNextAmmoBurn; // while charging, when to absorb another unit of player's ammo?

	//Player ID
	void InitStatusBar();
	void UpdateStatusBar();
	int m_izSBarState[SBAR_END];
	float m_flNextSBarUpdateTime;
	float m_flStatusBarDisappearDelay;
	char m_SbarString0[SBAR_STRING_SIZE];
	char m_SbarString1[SBAR_STRING_SIZE];

	void UpdateEntityInfo();

	//Not saved, reset on map change.
	float m_NextEntityInfoUpdateTime;

	// Assume this is false on map start since the client clears it anyway.
	bool m_EntityInfoEnabled{false};

	float m_flNextChatTime;

	void Player_Menu();

	void ResetMenu();

	bool Menu_Team_Input(int inp);
	bool Menu_Char_Input(int inp);

	void SetPrefsFromUserinfo(char* infobuffer);

	int m_iAutoWepSwitch;

	bool m_bRestored;

	//True if the player is currently spawning.
	bool m_bIsSpawning = false;

	bool IsOnRope() const { return (m_afPhysicsFlags & PFLAG_ONROPE) != 0; }

	void SetOnRopeState(bool bOnRope)
	{
		if (bOnRope)
			m_afPhysicsFlags |= PFLAG_ONROPE;
		else
			m_afPhysicsFlags &= ~PFLAG_ONROPE;
	}

	CRope* GetRope() { return m_pRope; }

	void SetRope(CRope* pRope)
	{
		m_pRope = pRope;
	}

	void SetIsClimbing(const bool bIsClimbing)
	{
		m_bIsClimbing = bIsClimbing;
	}

private:
	CRope* m_pRope;
	float m_flLastClimbTime = 0;
	bool m_bIsClimbing = false;

	//For saving and level changes.
	int m_HudColor = RGB_HUD_COLOR.ToInteger();

	bool m_bInfiniteAir;
	bool m_bInfiniteArmor;

public:
	/**
	*	@brief Sets the player's hud color
	*	@details The player must be fully connected and ready to receive user messages for this to work
	*/
	void SetHudColor(RGB24 color);

	void SendScoreInfo(CBasePlayer* destination);
	void SendScoreInfoAll();

	void ToggleCheat(Cheat cheat);
};

inline void CBasePlayer::SetWeaponBit(int id)
{
	m_WeaponBits |= 1ULL << id;
}

inline void CBasePlayer::ClearWeaponBit(int id)
{
	m_WeaponBits &= ~(1ULL << id);
}

inline bool CBasePlayer::HasSuit() const
{
	return (m_WeaponBits & (1ULL << WEAPON_SUIT)) != 0;
}

inline void CBasePlayer::SetHasSuit(bool hasSuit)
{
	if (hasSuit)
	{
		SetWeaponBit(WEAPON_SUIT);
	}
	else
	{
		ClearWeaponBit(WEAPON_SUIT);
	}
}

#define AUTOAIM_2DEGREES 0.0348994967025
#define AUTOAIM_5DEGREES 0.08715574274766
#define AUTOAIM_8DEGREES 0.1391731009601
#define AUTOAIM_10DEGREES 0.1736481776669

class CPlayerIterator
{
public:
	static constexpr int FirstPlayerIndex = 1;

public:
	CPlayerIterator()
		: m_pPlayer(nullptr), m_iNextIndex(gpGlobals->maxClients + 1)
	{
	}

	CPlayerIterator(const CPlayerIterator&) = default;

	CPlayerIterator(CBasePlayer* pPlayer)
		: m_pPlayer(pPlayer), m_iNextIndex(pPlayer ? pPlayer->entindex() + 1 : FirstPlayerIndex)
	{
	}

	CPlayerIterator& operator=(const CPlayerIterator&) = default;

	const CBasePlayer* operator*() const { return m_pPlayer; }

	CBasePlayer* operator*() { return m_pPlayer; }

	CBasePlayer* operator->() { return m_pPlayer; }

	void operator++()
	{
		m_pPlayer = static_cast<CBasePlayer*>(FindNextPlayer(m_iNextIndex, &m_iNextIndex));
	}

	void operator++(int)
	{
		++*this;
	}

	bool operator==(const CPlayerIterator& other) const
	{
		return m_pPlayer == other.m_pPlayer;
	}

	bool operator!=(const CPlayerIterator& other) const
	{
		return !(*this == other);
	}

	static CBasePlayer* FindNextPlayer(int index, int* pOutNextIndex = nullptr)
	{
		while (index <= gpGlobals->maxClients)
		{
			auto pPlayer = UTIL_PlayerByIndex(index);

			if (pPlayer)
			{
				if (pOutNextIndex)
				{
					*pOutNextIndex = index + 1;
				}

				return static_cast<CBasePlayer*>(pPlayer);
			}

			++index;
		}

		if (pOutNextIndex)
		{
			*pOutNextIndex = gpGlobals->maxClients;
		}

		return nullptr;
	}

private:
	CBasePlayer* m_pPlayer;
	int m_iNextIndex;
};

class CPlayerEnumerator
{
public:
	using iterator = CPlayerIterator;

public:
	CPlayerEnumerator() = default;

	iterator begin()
	{
		return {static_cast<CBasePlayer*>(CPlayerIterator::FindNextPlayer(CPlayerIterator::FirstPlayerIndex))};
	}

	iterator end()
	{
		return {};
	}
};

class CPlayerEnumeratorWithStart
{
public:
	using iterator = CPlayerIterator;

public:
	CPlayerEnumeratorWithStart(CBasePlayer* pStartEntity)
		: m_pStartEntity(pStartEntity)
	{
	}

	iterator begin()
	{
		return {m_pStartEntity};
	}

	iterator end()
	{
		return {};
	}

private:
	CBasePlayer* m_pStartEntity = nullptr;
};

inline CPlayerEnumerator UTIL_FindPlayers()
{
	return {};
}

inline CPlayerEnumeratorWithStart UTIL_FindPlayers(CBasePlayer* pStartEntity)
{
	return {pStartEntity};
}

/**
*	@brief Tag type to log player info in the form <tt>\"netname<userid><steamid><teamname>\"</tt>.
*/
struct PlayerLogInfo
{
	CBasePlayer& Player;
};

template <>
struct fmt::formatter<PlayerLogInfo>
{
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
	{
		auto it = ctx.begin();

		if (it != ctx.end() && *it != '}')
		{
			throw format_error("invalid format");
		}

		return it;
	}

	template <typename FormatContext>
	auto format(const PlayerLogInfo& info, FormatContext& ctx) const -> decltype(ctx.out())
	{
		auto edict = info.Player.edict();

		return fmt::format_to(ctx.out(), "\"{}<{}><{}><{}>\"",
			STRING(info.Player.pev->netname),
			g_engfuncs.pfnGetPlayerUserId(edict),
			GETPLAYERAUTHID(edict),
			GetTeamName(edict));
	}
};

inline bool gInitHUD = true;
inline bool gEvilImpulse101 = false;
inline bool giPrecacheGrunt = false;

/**
*	@brief Display the game title if this key is set
*/
inline DLL_GLOBAL std::string g_DisplayTitleName;
inline DLL_GLOBAL CBaseEntity* g_pLastSpawn = nullptr;
