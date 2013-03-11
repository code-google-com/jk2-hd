// Adjusts the ingame visuals.

#include "jk2_hooks.h"
#include "jk2_gamefuncs.h"

extern _CG_ConfigString CG_ConfigString;
extern _Info_ValueForKey Info_ValueForKey;
extern _cgi_R_RegisterShaderNoMip cgi_R_RegisterShaderNoMip;
extern _cgi_R_SetColor cgi_R_SetColor;
extern _CG_DrawPic CG_DrawPic;
extern _CG_DrawSavegamePic CG_DrawSavegamePic;
extern _CG_DrawLevelStats CG_DrawLevelStats;
extern _CG_DrawLoadBar CG_DrawLoadBar;
extern _va va;

extern _Cvar_Get Cvar_Get;

int r_mode;
int r_customwidth;
int r_customheight;

// This serves as a replacement to CG_DrawInformation, which left much to be desired...
// TEST: We need to make sure this works, first! If it does, we're going to do some evil sorcery here :P
void replacement_CG_DrawInformation(void)
{
	DWORD *e_savegameJustLoaded = (DWORD *)DLLJK2(0x6CBE0, jagamex86);
	DWORD *doLevelLoadStats = (DWORD *)DLLJK2(0x162654, jagamex86);
	const char *serverinfo = CG_ConfigString(CS_SERVERINFO);
	char *mapname = Info_ValueForKey(serverinfo, "mapname");
	qhandle_t shader = 0;

	if(r_mode == -1)
	{
		// Here is where the magic is...
		// We want to use a custom shader for each set of good resolutions :3
		if(r_customwidth == 1280 &&
			r_customheight == 720)
		{
			// 720p
			shader = cgi_R_RegisterShaderNoMip(va("levelshots/%s_720p"));
		}
		else if(r_customwidth == 1920 &&
			r_customheight == 1080)
		{
			// 1080p
			shader = cgi_R_RegisterShaderNoMip(va("levelshots/%s_1080p"));
		}
		else if(r_customwidth == 1440 &&
			r_customheight == 900)
		{
			// 1440x900
			shader = cgi_R_RegisterShaderNoMip(va("levelshots/%s_1440"));
		}
		else if(r_customwidth == 1600 &&
			r_customheight == 1200)
		{
			// 1600x1200
			shader = cgi_R_RegisterShaderNoMip(va("levelshots/%s_1600"));
		}
	}

	if(!shader)
	{
		shader = cgi_R_RegisterShaderNoMip(va("levelshots/%s", mapname));
	}

	if(shader && *e_savegameJustLoaded != 1)
	{
		// Not using a savegame, so use the levelshot instead
		cgi_R_SetColor(NULL);
		CG_DrawPic(0.0f, 0.0f, 640.0f, 480.0f, shader);
	}
	else
	{
		CG_DrawSavegamePic(0.0f, 480.0f, 640.0f, -480.0f); // I don't really understand these args, but this is what the game uses, so...?
	}

	if(*doLevelLoadStats)
	{
		CG_DrawLevelStats();
	}
	CG_DrawLoadBar();
}



// ---------------------------------------------------------------------------------------------------------------------------------------

PatchData_t *cg_drawInformationHack[3];

void DoVisualHacks(void)
{
	// go find me some cvars
	cvar_t *r_mode_cvar = Cvar_Get("r_mode", "3", 33);
	cvar_t *r_customwidth_cvar = Cvar_Get("r_customwidth", "1600", 33);
	cvar_t *r_customheight_cvar = Cvar_Get("r_customheight", "1200", 33);
	r_mode = r_mode_cvar->integer;
	r_customwidth = r_customwidth_cvar->integer;
	r_customheight = r_customheight_cvar->integer;

	cg_drawInformationHack[0] = JK2_PlacePatch(PATCH_JUMP, DLLJK2(0x26BCF, jagamex86), (unsigned int)replacement_CG_DrawInformation);
	cg_drawInformationHack[1] = JK2_PlacePatch(PATCH_JUMP, DLLJK2(0x486E7, jagamex86), (unsigned int)replacement_CG_DrawInformation);
	cg_drawInformationHack[2] = JK2_PlacePatch(PATCH_JUMP, DLLJK2(0x48712, jagamex86), (unsigned int)replacement_CG_DrawInformation);
}

void RemoveVisualHacks(void)
{
	JK2_RemovePatch(&cg_drawInformationHack[0]);
	JK2_RemovePatch(&cg_drawInformationHack[1]);
	JK2_RemovePatch(&cg_drawInformationHack[2]);
}