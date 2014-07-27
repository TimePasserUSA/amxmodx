/* AMX Mod X
*   Counter-Strike Module
*
* by the AMX Mod X Development Team
*
* This file is part of AMX Mod X.
*
*
*  This program is free software; you can redistribute it and/or modify it
*  under the terms of the GNU General Public License as published by the
*  Free Software Foundation; either version 2 of the License, or (at
*  your option) any later version.
*
*  This program is distributed in the hope that it will be useful, but
*  WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
*  General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software Foundation,
*  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*  In addition, as a special exception, the author gives permission to
*  link the code of this program with the Half-Life Game Engine ("HL
*  Engine") and Modified Game Libraries ("MODs") developed by Valve,
*  L.L.C ("Valve"). You must obey the GNU General Public License in all
*  respects for all of the code used other than the HL Engine and MODs
*  from Valve. If you modify this file, you may extend this exception
*  to your version of the file, but you are not obligated to do so. If
*  you do not wish to do so, delete this exception statement from your
*  version.
*/
#include "amxxmodule.h"
#include "MemoryUtils.h"

bool UTIL_IsPlayer(AMX* amx, edict_t* pPlayer) 
{
	bool player = false;

	if (strcmp(STRING(pPlayer->v.classname), "player") == 0)
	{
		player = true;
	}

	return player;
}

void UTIL_TextMsg_Generic(edict_t* pPlayer, const char* message)
{
	MESSAGE_BEGIN(MSG_ONE, GET_USER_MSG_ID(PLID, "TextMsg", NULL), NULL, pPlayer);
	WRITE_BYTE(HUD_PRINTCENTER); // 1 = console, 2 = console, 3 = chat, 4 = center, 5 = radio
	WRITE_STRING(message);
	MESSAGE_END();
	/*
	The byte above seems to use these:
	#define HUD_PRINTNOTIFY		1
	#define HUD_PRINTCONSOLE	2
	#define HUD_PRINTTALK		3
	#define HUD_PRINTCENTER		4
	#define HUD_PRINTRADIO		5
	However both 1 and 2 seems to go to console with Steam CS.
	*/
}

void *UTIL_FindAddressFromEntry(const char *entry, bool isHidden, const char *library)
{
	void *addressInBase = NULL;
	void *finalAddress;

	if (strcmp(library, "mod") == 0)
	{
		addressInBase = (void *)MDLL_Spawn;
	}
	else if (strcmp(library, "engine") == 0)
	{
		addressInBase = (void *)gpGlobals;
	}

	finalAddress = NULL;

	if (*entry != '\\')
	{
#if defined(WIN32)

		MEMORY_BASIC_INFORMATION mem;

		if (VirtualQuery(addressInBase, &mem, sizeof(mem)))
		{
			finalAddress = g_MemUtils.ResolveSymbol(mem.AllocationBase, entry);
		}

#elif defined(__linux__) || defined(__APPLE__)

		Dl_info info;
		void *handle = NULL;

		if (dladdr(addressInBase, &info) != 0)
		{
			void *handle = dlopen(info.dli_fname, RTLD_NOW);
			if (handle)
			{
				if (isHidden)
				{
					finalAddress = g_MemUtils.ResolveSymbol(handle, entry);
				}
				else
				{
					finalAddress = dlsym(handle, entry);
				}

				dlclose(handle);
			}
		}
#endif
	}
	else
	{
		finalAddress = g_MemUtils.DecodeAndFindPattern(addressInBase, entry);
	}

	return finalAddress != NULL ? finalAddress : NULL;
}

bool UTIL_CheckForPublic(const char *publicname)
{
	AMX* amx;
	int iFunctionIndex;
	int i = 0;
	char blah[64];

	strncpy(blah, publicname, sizeof(blah)-  1);

	while ((amx = MF_GetScriptAmx(i++)) != NULL)
	{
		if (MF_AmxFindPublic(amx, blah, &iFunctionIndex) == AMX_ERR_NONE)
		{
			return true;
		}
	}

	return false; 
}

char *UTIL_StringToLower(char *str)
{
	char *p;
	for (p = str; *p != '\0'; ++p)
	{
		*p = tolower(*p);
	}

	return str;
}