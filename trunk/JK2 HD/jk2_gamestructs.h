#ifndef _JK2_GAMESTRUCTS_H
#define _JK2_GAMESTRUCTS_H

#include "jk2_hooks.h"
#include "jk2_main.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// mode parm for FS_FOpenFile
typedef enum {
	FS_READ,
	FS_WRITE,
	FS_APPEND,
	FS_APPEND_SYNC
} fsMode_t;

typedef enum {
	FS_SEEK_CUR,
	FS_SEEK_END,
	FS_SEEK_SET
} fsOrigin_t;

typedef int		qhandle_t;
typedef int		thandle_t; //rwwRMG - inserted
typedef int		fxHandle_t;
typedef int		sfxHandle_t;
typedef int		fileHandle_t;
typedef int		clipHandle_t;

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

//rwwRMG - new vec types
typedef vec3_t	vec3pair_t[2];

typedef int ivec3_t[3];
typedef int ivec4_t[4];
typedef int ivec5_t[5];

typedef	int	fixed4_t;
typedef	int	fixed8_t;
typedef	int	fixed16_t;

typedef enum {qfalse, qtrue}	qboolean;

#define BUTTON_ATTACK			0x01
#define BUTTON_USE				0x20
#define BUTTON_ALT_ATTACK		0x80

// plane_t structure
// !!! if this is changed, it must be changed in asm code too !!!

typedef struct cplane_s {
	vec3_t	normal;
	float	dist;
	BYTE	type;			// for fast side tests: 0,1,2 = axial, 3 = nonaxial
	BYTE	signbits;		// signx + (signy<<1) + (signz<<2), used as lookup during collision
	BYTE	pad[2];
} cplane_t;

// trajectory_t
// Completely reverse engineered by eezstreet

typedef struct trajectory_s {
	int trType;
	int trTime;
	int trDuration;
	vec3_t trBase;
	vec3_t trDelta;
} trajectory_t;

// usercmd_t
// Completely reverse engineered by eezstreet

typedef struct usercmd_s {
	int serverTime;
	int buttons;
	unsigned char weapon;
	unsigned char forceSel;		// assumed
	unsigned char invSel;		// assumed
	unsigned char generic_cmd;	// assumed
	vec3_t angles;
	signed char forwardmove;
	signed char rightmove;
	signed char upmove;
} usercmd_t;

typedef struct {
	BYTE		allsolid;	// if true, plane is not valid
	BYTE		startsolid;	// if true, the initial point was in a solid area
	short		entityNum;	// entity the contacted sirface is a part of

	float		fraction;	// time completed, 1.0 = didn't hit anything
	vec3_t		endpos;		// final position
	cplane_t	plane;		// surface normal at impact, transformed to world space
	int			surfaceFlags;	// surface hit
	int			contents;	// contents on other side of surface hit
/*
Ghoul2 Insert Start
*/
	//rww - removed this for now, it's just wasting space in the trace structure.
//	CollisionRecord_t G2CollisionMap[MAX_G2_COLLISIONS];	// map that describes all of the parts of ghoul2 models that got hit
/*
Ghoul2 Insert End
*/
} trace_t;

typedef struct cvar_s {
	char		*name;
	char		*string;
	char		*resetString;		// cvar_restart will reset to this value
	char		*latchedString;		// for CVAR_LATCH vars
	int			flags;
	qboolean	modified;			// set each time the cvar is changed
	int			modificationCount;	// incremented each time the cvar is changed
	float		value;				// atof( string )
	int			integer;			// atoi( string )
	struct cvar_s *next;
	struct cvar_s *hashNext;
} cvar_t;


typedef enum {
	AXIS_SIDE,
	AXIS_FORWARD,
	AXIS_UP,
	AXIS_ROLL,
	AXIS_YAW,
	AXIS_PITCH,
	MAX_JOYSTICK_AXIS
} joystickAxis_t;

typedef enum {
  // bk001129 - make sure SE_NONE is zero
	SE_NONE = 0,	// evTime is still valid
	SE_KEY,		// evValue is a key code, evValue2 is the down flag
	SE_CHAR,	// evValue is an ascii char
	SE_MOUSE,	// evValue and evValue2 are reletive signed x / y moves
	SE_JOYSTICK_AXIS,	// evValue is an axis number and evValue2 is the current state (-127 to 127)
	SE_CONSOLE,	// evPtr is a char*
	SE_PACKET	// evPtr is a netadr_t followed by data bytes to evPtrLength
} sysEventType_t;

// PS3 controller support
typedef enum {
	PS3_TRIANGLE,
	PS3_CIRCLE,
	PS3_CROSS,
	PS3_SQUARE,
	PS3_UNMAPPED1,
	PS3_UNMAPPED2,		
	PS3_L1,
	PS3_R1,
	PS3_SELECT,
	PS3_START,
	PS3_L3,
	PS3_R3,
	PS3_PS,
} playStationButtons_t;

typedef enum //# weapon_e
{
	WP_NONE,

	// Player weapons
	WP_SABER,				 // NOTE: lots of code assumes this is the first weapon (... which is crap) so be careful -Ste.
	WP_BRYAR_PISTOL,
	WP_BLASTER,
	WP_DISRUPTOR,
	WP_BOWCASTER,
	WP_REPEATER,
	WP_DEMP2,
	WP_FLECHETTE,
	WP_ROCKET_LAUNCHER,
	WP_THERMAL,
	WP_TRIP_MINE,
	WP_DET_PACK,
	WP_STUN_BATON,

	//NOTE: player can only have up to 16 weapons, anything after that is enemy only

	WP_MELEE,			// Any ol' melee attack

	// NPC enemy weapons

	WP_EMPLACED_GUN,

	WP_BOT_LASER,		// Probe droid	- Laser blast

	WP_TURRET,			// turret guns 

	WP_ATST_MAIN,
	WP_ATST_SIDE,

	WP_TIE_FIGHTER,

	WP_RAPID_FIRE_CONC,

	WP_BLASTER_PISTOL,	// apparently some enemy only version of the blaster

//	WP_CHAOTICA_GUARD_GUN,	//			- B/W version of scav rifle for Chaotica's guards.
//	WP_BOT_ROCKET,		// Hunter Seeker - Rocket projectile
//	WP_KLINGON_BLADE,
//	WP_IMPERIAL_BLADE,

//	WP_DESPERADO,			// special holo-weapon
//	WP_PALADIN,			// special holo-weapon

	//# #eol
	WP_NUM_WEAPONS
} weapon_t;

// AMMO_NONE must be first and AMMO_MAX must be last, cause weapon load validates based off of these vals
typedef enum //# ammo_e
{
	AMMO_NONE,
	AMMO_FORCE,		// AMMO_PHASER
	AMMO_BLASTER,	// AMMO_STARFLEET,
	AMMO_POWERCELL,	// AMMO_ALIEN,
	AMMO_METAL_BOLTS,
	AMMO_ROCKETS,
	AMMO_EMPLACED,
	AMMO_THERMAL,
	AMMO_TRIPMINE,
	AMMO_DETPACK,
	AMMO_MAX
} ammo_t;

typedef struct weaponData_s
{
	char	classname[32];		// Spawning name
	char	weaponMdl[64];		// Weapon Model
	char	firingSnd[64];		// Sound made when fired
	char	altFiringSnd[64];	// Sound made when alt-fired
//	char	flashSnd[64];		// Sound made by flash
//	char	altFlashSnd[64];	// Sound made by an alt-flash
	char	stopSnd[64];		// Sound made when weapon stops firing
	char	chargeSnd[64];		// sound to start when the weapon initiates the charging sequence
	char	altChargeSnd[64];	// alt sound to start when the weapon initiates the charging sequence
	char	selectSnd[64];		// the sound to play when this weapon gets selected

#ifdef _IMMERSION
	char	firingFrc[64];
	char	altFiringFrc[64];
	char	stopFrc[64];
	char	chargeFrc[64];
	char	altChargeFrc[64];
	char	selectFrc[64];
#endif // _IMMERSION
	int		ammoIndex;			// Index to proper ammo slot
	int		ammoLow;			// Count when ammo is low

	int		energyPerShot;		// Amount of energy used per shot
	int		fireTime;			// Amount of time between firings
	int		range;				// Range of weapon
	
	int		altEnergyPerShot;	// Amount of energy used for alt-fire
	int		altFireTime;		// Amount of time between alt-firings
	int		altRange;			// Range of alt-fire

	char	weaponIcon[64];		// Name of weapon icon file
	int		numBarrels;			// how many barrels should we expect for this weapon?

	char	missileMdl[64];		// Missile Model
	char	missileSound[64];	// Missile flight sound
	float  	missileDlight;		// what is says
	vec3_t 	missileDlightColor;	// ditto

	char	alt_missileMdl[64];		// Missile Model
	char	alt_missileSound[64];	// Missile sound
	float  	alt_missileDlight;		// what is says
	vec3_t 	alt_missileDlightColor;	// ditto

	char	missileHitSound[64];	// Missile impact sound
	char	altmissileHitSound[64];	// alt Missile impact sound
#ifndef _USRDLL
	void	*func;
	void	*altfunc;

	char	mMuzzleEffect[64];
	int		mMuzzleEffectID;
	char	mAltMuzzleEffect[64];
	int		mAltMuzzleEffectID;

#endif

} weaponData_t;

typedef struct ammoData_s
{
	char	icon[32];	// Name of ammo icon file
	int		max;		// Max amount player can hold of ammo
} ammoData_t;

typedef enum //# team_e
{
	TEAM_FREE,			// caution, some code checks a team_t via "if (!team_t_varname)" so I guess this should stay as entry 0, great or what? -slc
//	TEAM_STARFLEET,
//	TEAM_BORG,
//	TEAM_PARASITE,
//	TEAM_SCAVENGERS,
//	TEAM_KLINGON,
//	TEAM_MALON,
//	TEAM_HIROGEN,
//	TEAM_IMPERIAL,
//	TEAM_STASIS,
//	TEAM_8472,
//	TEAM_BOTS,
//	TEAM_FORGE,
//	TEAM_DISGUISE,
	TEAM_PLAYER,
	TEAM_ENEMY,
	TEAM_NEUTRAL,	// most droids are team_neutral, there are some exceptions like Probe,Seeker,Interrogator

	//# #eol
	TEAM_NUM_TEAMS
} team_t;

// This list is made up from the model directories, this MUST be in the same order as the ClassNames array in NPC_stats.cpp
typedef enum 
{
	CLASS_NONE,				// hopefully this will never be used by an npc, just covering all bases
	CLASS_ATST,				// technically droid...
	CLASS_BARTENDER,
	CLASS_BESPIN_COP,		
	CLASS_CLAW,
	CLASS_COMMANDO,
	CLASS_DESANN,			
	CLASS_FISH,
	CLASS_FLIER2,
	CLASS_GALAK,
	CLASS_GLIDER,
	CLASS_GONK,				// droid
	CLASS_GRAN,
	CLASS_HOWLER,
	CLASS_IMPERIAL,
	CLASS_IMPWORKER,
	CLASS_INTERROGATOR,		// droid 
	CLASS_JAN,				
	CLASS_JEDI,				
	CLASS_KYLE,				
	CLASS_LANDO,			
	CLASS_LIZARD,
	CLASS_LUKE,				
	CLASS_MARK1,			// droid
	CLASS_MARK2,			// droid
	CLASS_GALAKMECH,		// droid
	CLASS_MINEMONSTER,
	CLASS_MONMOTHA,			
	CLASS_MORGANKATARN,
	CLASS_MOUSE,			// droid
	CLASS_MURJJ,
	CLASS_PRISONER,
	CLASS_PROBE,			// droid
	CLASS_PROTOCOL,			// droid
	CLASS_R2D2,				// droid
	CLASS_R5D2,				// droid
	CLASS_REBEL,
	CLASS_REBORN,
	CLASS_REELO,
	CLASS_REMOTE,
	CLASS_RODIAN,
	CLASS_SEEKER,			// droid
	CLASS_SENTRY,
	CLASS_SHADOWTROOPER,
	CLASS_STORMTROOPER,
	CLASS_SWAMP,
	CLASS_SWAMPTROOPER,
	CLASS_TAVION,
	CLASS_TRANDOSHAN,
	CLASS_UGNAUGHT,
	CLASS_WEEQUAY,
	
	CLASS_NUM_CLASSES
} class_t;

//bstate.h
typedef enum //# bState_e
{//These take over only if script allows them to be autonomous
	BS_DEFAULT = 0,//# default behavior for that NPC
	BS_ADVANCE_FIGHT,//# Advance to captureGoal and shoot enemies if you can
	BS_SLEEP,//# Play awake script when startled by sound
	BS_FOLLOW_LEADER,//# Follow your leader and shoot any enemies you come across
	BS_JUMP,//# Face navgoal and jump to it.
	BS_SEARCH,//# Using current waypoint as a base, search the immediate branches of waypoints for enemies
	BS_WANDER,//# Wander down random waypoint paths
	BS_NOCLIP,//# Moves through walls, etc.
	BS_REMOVE,//# Waits for player to leave PVS then removes itself
	BS_CINEMATIC,//# Does nothing but face it's angles and move to a goal if it has one
	//# #eol
	//internal bStates only
	BS_WAIT,//# Does nothing but face it's angles
	BS_STAND_GUARD,
	BS_PATROL,
	BS_INVESTIGATE,//# head towards temp goal and look for enemies and listen for sounds
	BS_STAND_AND_SHOOT,
	BS_HUNT_AND_KILL,
	BS_FLEE,//# Run away!
	NUM_BSTATES
} bState_t;

typedef enum //# soundChannel_e
{
	CHAN_AUTO,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" # Auto-picks an empty channel to play sound on
	CHAN_LOCAL,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" # menu sounds, etc
	CHAN_WEAPON,//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" 
	CHAN_VOICE, //## %s !!"W:\game\base\!!sound\voice\*.wav;*.mp3" # Voice sounds cause mouth animation
	CHAN_VOICE_ATTEN, //## %s !!"W:\game\base\!!sound\voice\*.wav;*.mp3" # Causes mouth animation but still use normal sound falloff 
	CHAN_ITEM,  //## %s !!"W:\game\base\!!sound\*.wav;*.mp3"
	CHAN_BODY,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3"
	CHAN_AMBIENT,//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" # added for ambient sounds
	CHAN_LOCAL_SOUND,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" #chat messages, etc
	CHAN_ANNOUNCER,		//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" #announcer voices, etc
	CHAN_LESS_ATTEN,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" #attenuates similar to chan_voice, but uses empty channel auto-pick behaviour
	CHAN_MENU1,		//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" #menu stuff, etc
	CHAN_VOICE_GLOBAL,  //## %s !!"W:\game\base\!!sound\voice\*.wav;*.mp3" # Causes mouth animation and is broadcast, like announcer
} soundChannel_t;

typedef enum
{
	// Guesswork here. Looks like it could be correct, but I honestly have no idea.
	SFB_TRICORDER,
	SFB_RIFLEMAN,
	SFB_PHASER,
} ravenNPCSpawnFlags_t;

typedef struct {
	// print message on the local console
	void	(*Printf)( const char *fmt, ... );

	// Write a camera ref_tag to cameras.map
	void	(*WriteCam)( const char *text );
	void	(*FlushCamFile)();

	// abort the game
	void	(*Error)( int, const char *fmt, ... );

	// get current time for profiling reasons
	// this should NOT be used for any game related tasks,
	// because it is not journaled
	int		(*Milliseconds)( void );

	// console variable interaction
	int		*unk1;
	void	(*cvar_set)( const char *var_name, const char *value );
	int		(*Cvar_VariableIntegerValue)( const char *var_name );
	void	(*Cvar_VariableStringBuffer)( const char *var_name, char *buffer, int bufsize );

	// ClientCommand and ServerCommand parameter access
	int		(*argc)( void );
	char	*(*argv)( int n );

	int		(*FS_FOpenFile)( const char *qpath, fileHandle_t *file, fsMode_t mode );
	int		(*FS_Read)( void *buffer, int len, fileHandle_t f );
	int		(*FS_Write)( const void *buffer, int len, fileHandle_t f );
	void	(*FS_FCloseFile)( fileHandle_t f );
	int		(*FS_ReadFile)( const char *name, void **buf );
	void	(*FS_FreeFile)( void *buf );
	int		(*FS_GetFileList)(  const char *path, const char *extension, char *listbuf, int bufsize );

	qboolean	(*AppendToSaveGame)(unsigned long chid, const void *data, int length);

	int			(*ReadFromSaveGame)(unsigned long chid, void *pvAddress, int iLength, void **ppvAddressPtr);
	int			(*ReadFromSaveGameOptional)(unsigned long chid, void *pvAddress, int iLength, void **ppvAddressPtr);

	// add commands to the console as if they were typed in
	// for map changing, etc
	void	(*SendConsoleCommand)( const char *text );

	//=========== server specific functionality =============

	// kick a client off the server with a message
	void	(*DropClient)( int clientNum, const char *reason );

	// reliably sends a command string to be interpreted by the given
	// client.  If clientNum is -1, it will be sent to all clients
	void	(*SendServerCommand)( int clientNum, const char *fmt, ... );

	// config strings hold all the index strings, and various other information
	// that is reliably communicated to all clients
	// All of the current configstrings are sent to clients when
	// they connect, and changes are sent to all connected clients.
	// All confgstrings are cleared at each level start.
	void	(*SetConfigstring)( int num, const char *string );
	void	(*GetConfigstring)( int num, char *buffer, int bufferSize );

	// userinfo strings are maintained by the server system, so they
	// are persistant across level loads, while all other game visible
	// data is completely reset
	void	(*GetUserinfo)( int num, char *buffer, int bufferSize );
	void	(*SetUserinfo)( int num, const char *buffer );

	// the serverinfo info string has all the cvars visible to server browsers
	void	(*GetServerinfo)( char *buffer, int bufferSize );

	void	(*SetBrushModel)( int *ent, const char *name );

	// sets mins and maxs based on the brushmodel name
	void	(*trace)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, 
			const int passEntityNum, const int contentmask , const int eG2TraceType, const int useLod);

	// point contents against all linked entities
	int		(*pointcontents)( const vec3_t point, int passEntityNum );
	// what contents are on the map? 
	int		(*totalMapContents)();

	qboolean	(*inPVS)( const vec3_t p1, const vec3_t p2 );
	qboolean	(*inPVSIgnorePortals)( const vec3_t p1, const vec3_t p2 );
	void		(*AdjustAreaPortalState)( DWORD *ent, qboolean open );
	qboolean	(*AreasConnected)( int area1, int area2 );

	// an entity will never be sent to a client or used for collision
	// if it is not passed to linkentity.  If the size, position, or
	// solidity changes, it must be relinked.
	void	(*linkentity)( void *ent );
	void	(*unlinkentity)( void *ent );		// call before removing an interactive entity

	// EntitiesInBox will return brush models based on their bounding box,
	// so exact determination must still be done with EntityContact
	int		(*EntitiesInBox)( const vec3_t mins, const vec3_t maxs, DWORD **list, int maxcount );

	// perform an exact check against inline brush models of non-square shape
	qboolean	(*EntityContact)( const vec3_t mins, const vec3_t maxs, const DWORD *ent );

	// sound volume values
	int		*VoiceVolume;

	// dynamic memory allocator for things that need to be freed
	void		*(*Malloc)( int iSize, DWORD eTag, qboolean bZeroIt);	// see qcommon/tags.h for choices
	int			(*Free)( void *buf );
	qboolean	(*bIsFromZone)( void *buf, DWORD eTag);	// see qcommon/tags.h for choices

	qhandle_t	(*G2API_PrecacheGhoul2Model)(const char *fileName);
} game_import_t;

//This is the stuff we hook
typedef struct {
	int			apiversion;
	void		(*Init)( const char *mapname, const char *spawntarget, int checkSum, const char *entstring, int levelTime, int randomSeed, int globalTime, int eSavedGameJustLoaded, qboolean qbLoadTransition );
	void		(*Shutdown) (void);

	// ReadLevel is called after the default map information has been
	// loaded with SpawnEntities
	void		(*WriteLevel) (qboolean qbAutosave);
	void		(*ReadLevel)  (qboolean qbAutosave, qboolean qbLoadTransition);
	qboolean	(*GameAllowedToSaveHere)(void);

	// return NULL if the client is allowed to connect, otherwise return
	// a text string with the reason for denial
	char		*(*ClientConnect)( int clientNum, qboolean firstTime, int eSavedGameJustLoaded );

	void		(*ClientBegin)( int clientNum, DWORD *cmd, int eSavedGameJustLoaded);
	void		(*ClientUserinfoChanged)( int clientNum );
	void		(*ClientDisconnect)( int clientNum );
	void		(*ClientCommand)( int clientNum );
	void		(*ClientThink)( int clientNum, DWORD *cmd );

	void		(*RunFrame)( int levelTime );
	void		(*ConnectNavs)( const char *mapname, int checkSum );

// ConsoleCommand will be called when a command has been issued
	// that is not recognized as a builtin function.
	// The game can issue gi.argc() / gi.argv() commands to get the command
	// and parameters.  Return qfalse if the game doesn't recognize it as a command.
	qboolean	(*ConsoleCommand)( void );

	//void		(*PrintEntClassname)( int clientNum );
	//int			(*ValidateAnimRange)( int startFrame, int endFrame, float animSpeed );

	void		(*GameSpawnRMGEntity)(char *s);
	//
	// global variables shared between game and server
	//

	// The gentities array is allocated in the game dll so it
	// can vary in size from one game to another.
	// 
	// The size will be fixed when ge->Init() is called
	// the server can't just use pointer arithmetic on gentities, because the
	// server's sizeof(struct gentity_s) doesn't equal gentitySize
	void		*gentities;
	int			gentitySize;
	int			num_entities;		// current number, <= MAX_GENTITIES
} game_export_t; // This struct was decompiled from JKA. It's probably not correct for JK2 though. ~eez

//syscalls
typedef enum {
	SYSCALL_PRINT,
	SYSCALL_ERROR,
	SYSCALL_STARTSOUND=30,
	SYSCALL_REGISTERSOUND=37,
	SYSCALL_STOP_ALL_FF=41,
	SYSCALL_REGISTERSHADER=48,
	SYSCALL_REGISTERSHADERNOMIP=49,
	SYSCALL_GETGAMESTATE=76,
} syscalls_t;

// parameters to the main Error routine
typedef enum {
	ERR_FATAL,					// exit the entire game with a popup window
	ERR_DROP,					// print to console and disconnect from game
	ERR_SERVERDISCONNECT,		// don't kill server
	ERR_DISCONNECT,				// client disconnected from the server
	ERR_NEED_CD					// pop up the need-cd dialog
} errorParm_t;

extern game_import_t *gi;
extern game_export_t *ge;

#define CS_SERVERINFO		0
#define	CS_SYSTEMINFO		1

void JOY_Hacks(qboolean remove);

#endif