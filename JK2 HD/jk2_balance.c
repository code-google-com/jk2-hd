// Adjusts the balance of weapons and NPCs in JK2

#include "jk2_main.h"
#include "jk2_hooks.h"

void SpruceUpWeapons(void);

// Here's some of the changes I propose:

// Repeater primary: 
// Primarily retooled as an assault rifle, as opposed to a peashooter
// Slop:
//   * set to 1.4 when running
//   * set to 1.2 when standing still
//   * set to 1.0 when crouching
// Damage:
//   * primary damage increased from 8->12
//   * NPC damage increased from:
//      * Padawan: 2->4
//      * Jedi: 4->6
//      * Above: 6->8
// Ammo Usage:
//   * secondary fire increased from 20->40
// Fire Rate:
//   * primary fire rate reduced back to MP levels


// Bowcaster:
// Primarily retooled as a crossbow weapon/hallway clearer as opposed to a light blaster
// Damage:
//   * primary fire: increased from 45->75
//   * NPC damage increased from:
//       * Padawan: 12->25
//       * Jedi: 24->40
//       * Above: 36->45
// Ammo costs:
//   * primary fire: increased from 5->10
// Fire Rate:
//   * primary fire: reduced from 750ms->1000ms

// Bryar Pistol:
// Received a damage bump for primary fire, so that it's actually semi-useful
// Damage:
//   * primary fire: increased from 14->20

// Disruptor:
// Primary fire is now useful, and functions as a precision, power weapon
// Damage:
//   * primary fire: increased from 14->30
// Ammo costs:
//   * primary fire: increased from 3->10

void DoBalanceChanges(void)
{
	// Bryar Change:
	JK2_BytePatching(25, DLLJK2(0xC2341, jagamex86));
	//*((unsigned char*)DLLJK2(0xC2340, jagamex86)) = 20;

	// Disruptor Change:
	JK2_LongPatching(35, DLLJK2(0xC28CD, jagamex86));
	//*((unsigned char*)DLLJK2(0xC28C9, jagamex86)) = 30;

	// Bowcaster Primary:
	JK2_LongPatching(85, DLLJK2(0xC2C93, jagamex86));		// primary fire
	//JK2_LongPatching(25, DLLJK2(0xC2CBF, jagamex86));		// padawan difficulty
	//JK2_BytePatching(0x0F, DLLJK2(0x200C2CCB, jagamex86));	// jedi difficulty
	//JK2_BytePatching(0x05, DLLJK2(0x200C2CCE, jagamex86));	// jedi knight difficulty
	//*((unsigned char*)DLLJK2(0xC2C8F, jagamex86)) = 75;

	// Repeater Primary:
	JK2_BytePatching(12, DLLJK2(0xC325F, jagamex86));
	//*((unsigned char*)DLLJK2(0xC325E, jagamex86)) = 12;

	SpruceUpWeapons();
}


extern int Q_stricmpn (const char *s1, const char *s2, int n);		// gah you fucker, why won't you link properly >_>'
extern int Q_stricmp ( const char *s1, const char *s2);
int NPC_WeaponsForTeam(team_t team, int spawnflags, const char *NPC_type)
{
	switch(team)
	{
		case TEAM_PLAYER:
			{
				if(spawnflags & ( 1 << SFB_RIFLEMAN ) )
				{
					return (1 << WP_REPEATER);
				}

				else if(spawnflags & ( 1 << SFB_PHASER ) || !Q_stricmpn(NPC_type, "bespincop", 9))
				{
					return (1 << WP_BLASTER_PISTOL);
				}

				else if(!Q_stricmpn(NPC_type, "jedi", 4) || !Q_stricmp(NPC_type, "luke"))
				{
					return (1 << WP_SABER);
				}

				else if(!Q_stricmpn(NPC_type, "prisoner_bowcaster", 18))
				{
					return (1 << WP_BOWCASTER);
				}

				else if(!Q_stricmpn(NPC_type, "prisoner", 9) || !Q_stricmp(NPC_type, "MonMothma"))
				{
					return WP_NONE;
				}

				return (1 << WP_BLASTER);
				break;
			}
		case TEAM_NEUTRAL:
			return WP_NONE;
			break;

		case TEAM_ENEMY:
			if ( Q_stricmp( "tavion", NPC_type ) == 0 || 
				Q_stricmpn( "reborn", NPC_type, 6 ) == 0 || 
				Q_stricmp( "desann", NPC_type ) == 0 || 
				Q_stricmpn( "shadowtrooper", NPC_type, 13 ) == 0 )
				return ( 1 << WP_SABER);

			else if ( Q_stricmpn( "stofficer", NPC_type, 9 ) == 0 )
			{
				return ( 1 << WP_FLECHETTE);
			}
			else if ( Q_stricmp( "stcommander", NPC_type ) == 0 )
			{
				return ( 1 << WP_REPEATER);
			}
			else if ( Q_stricmp( "swamptrooper", NPC_type ) == 0 )
			{
				return ( 1 << WP_FLECHETTE);
			}
			else if ( Q_stricmp( "swamptrooper2", NPC_type ) == 0 )
			{
				return ( 1 << WP_REPEATER);
			}
			else if ( Q_stricmp( "rockettrooper", NPC_type ) == 0 )
			{
				return ( 1 << WP_ROCKET_LAUNCHER);
			}
			else if ( Q_stricmp( "imperial", NPC_type ) == 0 )
			{
				return ( 1 << WP_BLASTER_PISTOL);
			}
			else if ( Q_stricmpn( "impworker", NPC_type, 9 ) == 0 )
			{
				return ( 1 << WP_BLASTER_PISTOL);
			}
			else if ( Q_stricmp( "stormpilot", NPC_type ) == 0 )
			{
				return ( 1 << WP_BLASTER_PISTOL);
			}
			else if ( Q_stricmp( "galak_mech", NPC_type ) == 0 )
			{
				return ( 1 << WP_REPEATER);
			}
			else if ( Q_stricmpn( "ugnaught", NPC_type, 8 ) == 0 )
			{
				// Aren't these guys neutral? --eez
				return WP_NONE;
			}
			else if ( Q_stricmp( "granboxer", NPC_type ) == 0 )
			{
				return ( 1 << WP_MELEE);
			}
			else if ( Q_stricmpn( "gran", NPC_type, 4 ) == 0 )
			{
				return (( 1 << WP_THERMAL)|( 1 << WP_MELEE));
			}
			else if ( Q_stricmp( "rodian", NPC_type ) == 0 )
			{
				return ( 1 << WP_DISRUPTOR);
			}

			else if (( Q_stricmp( "interrogator",NPC_type) == 0) || ( Q_stricmp( "sentry",NPC_type) == 0) || (Q_stricmpn( "protocol",NPC_type,8) == 0) )
			{
				return WP_NONE;
			}

			else if ( Q_stricmp( "weequay_blaster", NPC_type ) == 0 || Q_stricmp( "weequay2_blaster", NPC_type ) == 0 ||
				Q_stricmp( "weequay3_blaster", NPC_type ) == 0 || Q_stricmp( "weequay4_blaster", NPC_type ) == 0)
			{
				// Really ugly hack here. Prevents weequay_blasterX from using bowcaster. --eez
				return ( 1 << WP_BLASTER );
			}

			else if ( Q_stricmp( "weequay_repeater", NPC_type ) == 0 || Q_stricmp( "weequay2_repeater", NPC_type ) == 0 ||
				Q_stricmp( "weequay3_repeater", NPC_type ) == 0 || Q_stricmp( "weequay4_repeater", NPC_type ) == 0)
			{
				// see above --eez
				return ( 1 << WP_REPEATER );
			}

			else if ( Q_stricmpn( "weequay", NPC_type, 7 ) == 0 )
			{
				return ( 1 << WP_BOWCASTER);//|( 1 << WP_STAFF )(FIXME: new weap?)
			}
			else if (( Q_stricmp( "probe", NPC_type ) == 0 ) || ( Q_stricmp( "seeker", NPC_type ) == 0 ))
			{
				return ( 1 << WP_BOT_LASER);
			}	
			else if ( Q_stricmpn( "remote", NPC_type, 6 ) == 0 )
			{
				return ( 1 << WP_BOT_LASER );
			}	
			else if ( Q_stricmp( "trandoshan", NPC_type ) == 0 )
			{
				return (1<<WP_REPEATER);
			}
			else if ( Q_stricmp( "atst", NPC_type ) == 0 )
			{
				return (( 1 << WP_ATST_MAIN)|( 1 << WP_ATST_SIDE));
			}
			else if ( Q_stricmp( "mark1", NPC_type ) == 0 )
			{	// why are these classified as neuts... --eez
				return ( 1 << WP_BOT_LASER);
			}
			else if ( Q_stricmp( "mark2", NPC_type ) == 0 )
			{
				return ( 1 << WP_BOT_LASER);
			}
			else if ( Q_stricmp( "minemonster", NPC_type ) == 0 )
			{
				return ( 1 << WP_MELEE);
			}
			else if ( Q_stricmp( "howler", NPC_type ) == 0 )
			{
				return ( 1 << WP_MELEE);
			}
			//Stormtroopers, etc.
			return ( 1 << WP_BLASTER);
			break;
	}
	return WP_NONE;
}

PatchData_t *NPCweaponHacks[2];
void SpruceUpWeapons(void)
{
	// This code here hooks the current NPC_WeaponsForTeam and replaces it, allowing us control over all the NPCs' weapons ;)
	JK2_PlacePatch(PATCH_CALL, DLLJK2(0xD3ED5, jagamex86), (unsigned int)NPC_WeaponsForTeam);
	JK2_PlacePatch(PATCH_CALL, DLLJK2(0xD6C96, jagamex86), (unsigned int)NPC_WeaponsForTeam);
}