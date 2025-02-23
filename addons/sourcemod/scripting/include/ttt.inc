#if defined _ttt_included
    #endinput
#endif
#define _ttt_included

#define LoopValidClients(%1) for(int %1 = 1; %1 <= MaxClients; %1++) if(TTT_IsClientValid(%1))

#define TTT_TEAM_UNASSIGNED (1 << 0)
#define TTT_TEAM_INNOCENT (1 << 1)
#define TTT_TEAM_TRAITOR (1 << 2)
#define TTT_TEAM_DETECTIVE (1 << 3)

#define ROLE_LENGTH 24
#define MAX_ITEM_LENGTH 64
#define TTT_LOG_SIZE 512
#define CS_SLOT_MELEE CS_SLOT_KNIFE

#include <ttt_stock>
#include <sdktools>
#include <sdkhooks>
#include <cstrike>

enum KarmaUpdate
{
    Karma_Add,
    Karma_Set,
    Karma_Subtract
}

enum RoundStatus
{
    Round_Inactive = -1,   // -1
    Round_Warmup,          //  0
    Round_Selection,       //  1
    Round_Active,          //  2
    Round_Ending,          //  3
    Round_Ended            //  4
}

enum Collision_Group_t
{
    COLLISION_GROUP_NONE  = 0,
    COLLISION_GROUP_DEBRIS,			// Collides with nothing but world and static stuff
    COLLISION_GROUP_DEBRIS_TRIGGER, // Same as debris, but hits triggers
    COLLISION_GROUP_INTERACTIVE_DEBRIS,	// Collides with everything except other interactive debris or debris
    COLLISION_GROUP_INTERACTIVE,	// Collides with everything except interactive debris or debris
    COLLISION_GROUP_PLAYER,
    COLLISION_GROUP_BREAKABLE_GLASS,
    COLLISION_GROUP_VEHICLE,
    COLLISION_GROUP_PLAYER_MOVEMENT,  // For HL2, same as Collision_Group_Player, for
                                        // TF2, this filters out other players and CBaseObjects
    COLLISION_GROUP_NPC,			// Generic NPC group
    COLLISION_GROUP_IN_VEHICLE,		// for any entity inside a vehicle
    COLLISION_GROUP_WEAPON,			// for any weapons that need collision detection
    COLLISION_GROUP_VEHICLE_CLIP,	// vehicle clip brush to restrict vehicle movement
    COLLISION_GROUP_PROJECTILE,		// Projectiles!
    COLLISION_GROUP_DOOR_BLOCKER,	// Blocks entities not permitted to get near moving doors
    COLLISION_GROUP_PASSABLE_DOOR,	// Doors that the player shouldn't collide with
    COLLISION_GROUP_DISSOLVING,		// Things that are dissolving are in this group
    COLLISION_GROUP_PUSHAWAY,		// Nonsolid on client and server, pushaway in player code

    COLLISION_GROUP_NPC_ACTOR,		// Used so NPCs in scripts ignore the player.
    COLLISION_GROUP_NPC_SCRIPTED,	// USed for NPCs in scripts that should not collide with each other

    LAST_SHARED_COLLISION_GROUP
};

// edict->solid values
// NOTE: Some movetypes will cause collisions independent of SOLID_NOT/SOLID_TRIGGER when the entity moves
// SOLID only effects OTHER entities colliding with this one when they move - UGH!

// Solid type basically describes how the bounding volume of the object is represented
// NOTE: SOLID_BBOX MUST BE 2, and SOLID_VPHYSICS MUST BE 6
// NOTE: These numerical values are used in the FGD by the prop code (see prop_dynamic)
enum SolidType_t
{
    SOLID_NONE			= 0,	// no solid model
    SOLID_BSP			= 1,	// a BSP tree
    SOLID_BBOX			= 2,	// an AABB
    SOLID_OBB			= 3,	// an OBB (not implemented yet)
    SOLID_OBB_YAW		= 4,	// an OBB, constrained so that it can only yaw
    SOLID_CUSTOM		= 5,	// Always call into the entity for tests
    SOLID_VPHYSICS		= 6,	// solid vphysics object, get vcollide from the model and collide with that
    SOLID_LAST,
};

/**
 * Called before players roles are picked/assigned.
 * Return Plugin_Handled or higher to prevent round start.
 * You could slay players here to force them to sit out.
 */
forward Action TTT_OnRoundStart_Pre();

/**
 * Called when the actual TTT round starts (after selection time).
 *
 * @param roundid           ID of the current round
 * @param innocents         Count of innocent players.
 * @param traitors          Count of traitor players.
 * @param detectives         Count of detective players.
 */
forward void TTT_OnRoundStart(int roundid, int innocents, int traitors, int detectives);

/**
 * Called when round start is failed or prevented.
 *
 * @param players           The amount of players we have.
 *                          -1 if this round was prevented by
 *                          another plugin.
 * @param requiredPlayers   The amount of players we need.
 * @param detective         The amount needed for detective.
 */
forward void TTT_OnRoundStartFailed(int players, int requiredPlayers, int detective);

/**
 * Called when the actual TTT round ended.
 *
 * @param winner             The Winning Team.
 * @param array              Round logs as handle (if you want ArrayList instead of Handle use this: `ArrayList aLogs = view_as<ArrayList>(array);´)
 */
forward void TTT_OnRoundEnd(int winner, Handle array);

/**
 * Called when a client's role is assigned.
 *
 * @param client            Client index.
 * @param role              Role.
 */
forward void TTT_OnClientGetRole(int client, int role);

/**
 * Called when a client take damage
 *
 * @param victim            Person who take damage.
 * @param attacker          Attacker.
 * @param damage            How much damage
 * @param weapon            Weapon index
 * @param badAction         If damage was a bad action (example: Innocent damaged Detective)
 */
forward void TTT_OnTakeDamage(int victim, int attacker, float damage, int weapon, bool badAction);

/**
 * Called when a client dies, after Karma process
 *
 * @param victim            Person who died.
 * @param attacker          Attacker.
 * @param badAction         If kill was a bad action (example: Innocent killed Detective)
 */
forward void TTT_OnClientDeath(int victim, int attacker, bool badAction);

/**
 * Called when a client dies, before Karma process
 *
 * @param victim            Person who died.
 * @param attacker          Attacker.
 */
forward Action TTT_OnClientDeathPre(int victim, int attacker);

/**
 * Called before we create rules menu.
 * If want to change it use "return Plugin_Changed;"
 *
 * @param client            The client index.
 * @param show              True - Menu will be opened, False - Menu will not open
 */
forward Action TTT_OnRulesMenu(int client, bool &show);

/**
 * Called before we create detectives menu.
 * If want to change it use "return Plugin_Changed;"
 *
 * @param client            The client index.
 * @param show              True - Menu will be opened, False - Menu will not open
 */
forward Action TTT_OnDetectiveMenu(int client, bool &show);

/**
 * Called on updated player models
 * (if ttt_update_client_model is true)
 *
 * @param client            The client index after updated model
 * @param model             Path of current model
 */
forward void TTT_OnModelUpdate(int client, const char[] model);

/**
 * Called after TTT_RespawnPlayer execute
 *
 * @param client            The client index
 */
forward void TTT_OnPlayerRespawn(int client);

forward void TTT_OnButtonPress(int client, int button);
forward void TTT_OnButtonRelease(int client, int button);

/**
 * Called before RDM will be handled.
 *
 * @param victim            Person who died.
 * @param attacker          Attacker.
 */
forward Action TTT_OnPlayerDeath(int victim, int attacker);

/**
 * Called before Karma has been updated on a client.
 *
 * @param client           Client index.
 * @param type             Type of update. (Enum above)
 * @param karma            Amount of Karma
 */
 forward Action TTT_OnPreKarmaUpdate(int client, KarmaUpdate type, int karma);

/**
 * Called once Karma has been updated on a client.
 *
 * @param client            Client index.
 * @param type              Type of update. (Enum above)
 * @param karma              Amount of Karma.
 */
forward void TTT_OnKarmaUpdate(int client, KarmaUpdate type, int karma);

/**
 * This forward will be called when a client hasn't access to command (TTT_CheckCommandAccess)
 * It's possible to give a client access to an not accessible command
 * You must use 'return Plugin_Changed;' if you want to change the bool 'access'
 */
forward bool TTT_OnCheckCommandAccess(int client, const char[] command, const char[] flags, bool access);

/**
 * This forward is called PRIOR to the role selection done from the core TTT.
 * The purpose of this forward is to be able to override the original Role selector
 * and write your own for potential custom modes.
 *
 * @param traitors              Amount of traitors
 * @param detectives            Amount of detectives
 * @param innocents             Amount of innocents
 */
forward Action TTT_OnRoleSelection(int& traitors, int& detectives, int& innocents);


/**
 * @param startRoundTimer      Start round timer (true - by default), or not
 * @param time                 Time in seconds in how many seconds the round should end
                               Note: This will not modify the round time!
 */
forward Action TTT_OnRoundTimerStart_Pre(bool& startRoundTimer, float& time);



/**
 * Checks if the round is active.
 */
#pragma deprecated TTT_IsRoundActive will be removed. Please use TTT_GetRoundStatus
native bool TTT_IsRoundActive();

/**
 * Retrieve a client's role.
 *
 * @param client            Client index.
 */
native int TTT_GetClientRole(int client);

/**
 * Retrieve a client's karma.
 *
 * @param client            Client index.
 */
native int TTT_GetClientKarma(int client, bool publicKarma = true);

/**
 * Set a client's role.
 *
 * @param client            Client index.
 * @param role              The new role.
 */
native int TTT_SetClientRole(int client, int role);

/**
 * Sets the karma of a client.
 *
 * @param client            Client index.
 * @param karma             Karma for the client.
 */
native int TTT_SetClientKarma(int client, int karma, bool force);

/**
 * Add karma to a client (currentKarma+karma).
 *
 * @param client            Client index.
 * @param karma             Karma that will be added.
 */
native int TTT_AddClientKarma(int client, int karma, bool force);

/**
 * Remove karma from a client (currentKarma-karma).
 *
 * @param client            Client index.
 * @param karma             Karma to remove.
 */
native int TTT_RemoveClientKarma(int client, int karma, bool force);

/**
 * Log a String to the ingame Log.
 *
 * @param string            The message that should get logged.
 */
native void TTT_LogString(const char[] format, any ...);

/**
 * Force a player to be traitor next round. (If there are enough Traitors)
 *
 * @param client             The client index to change.
 *
 * @return False if the client's role already forced for the next round.
 */
native bool TTT_ForceTraitor(int client);

/**
 * Force a player to be detective next round. (If there are enough Detectives)
 *
 * @param client             The client index to change.
 *
 * @return False if the client's role is already forced for the next round.
 */
native bool TTT_ForceDetective(int client);

/**
 * NO DESCRIPTION
 *
 * @param client             The client index.
 * @param rounds             The amount of rounds to add.
 * @param force              Begin directly if client is alive.
 *
 * @return                   Returns the amount of rounds, otherwise the client index is invalid
 */
native int TTT_AddRoundSlays(int client, int rounds, bool force);

/**
 * NO DESCRIPTION
 *
 * @param id                 The player id.
 * @param rounds             The amount of rounds to add.
 * @param force              Begin directly if client is alive.
 *
 * @return                   Returns the amount of rounds, otherwise the client index is invalid
 */
native void TTT_AddRoundSlaysByID(int id, int rounds, bool force);

/**
 * NO DESCRIPTION
 *
 * @param client             The client index.
 * @param rounds             The amount of rounds.
 * @param force              Begin directly if client is alive (Default: false).
 *                           This works now in a inactive round, when force is true
 * @return Returns the amount of rounds, otherwise the client index is invalid
 */
native int TTT_SetRoundSlays(int client, int rounds, bool force = false);

/**
 * Open rules menu for the client
 *
 * @param client            The client index.
 */
native void TTT_ClientOpenRules(int client);

/**
 * Returns the time of a active rounds (-1 for inactive round/failed to start)
 *
 * @param type                Which type is needed? (0 - Time, 1 - Minutes, 2 - Seconds)
 */
native void TTT_GetRoundTime(int type);

/**
 * Returns whether a client has access to a given command string.  The string 
 * can be any override string, as overrides can be independent of 
 * commands.  This feature essentially allows you to create custom 
 * flags using the override system.
 *
 * Note: If cvar is not set/empty, it will always return false
 *
 * @param client        Client index.
 * @param command        Command name.  If the command is not found, the default 
 *                        flags are used.
 * @param cvar            Flag convar to use as a default, if the command or override 
 *                        is not found.
 * @param override_only    If true, SourceMod will not attempt to find a matching 
 *                        command, and it will only use the default flags specified.
 *                        Otherwise, SourceMod will ignore the default flags if 
 *                        there is a matching admin command.
 *
 * @return                True if the client has access, false otherwise.
 *
 * THIS TEXT WAS COPIED FROM HERE: https://github.com/alliedmodders/sourcemod/blob/6c7e29cbdbe1fe17982a4531d839be65116abca2/plugins/include/console.inc#L479
 */
native bool TTT_CheckCommandAccess(int client, const char[] command, ConVar cvar, bool override_only = false);

/**
 * Respawns a player on their body location.
 *
 * @param client        Client Index
 * @noreturn
 *
 */
native void TTT_RespawnPlayer(int client);

/**
 * Respawns a player randomly as per CS_RespawnPlayer()
 *
 * @param client        Client index
 * @noreturn
 *
 */ 
native void TTT_RespawnPlayerRandom(int client);

/**
 * Returns the timestamp where "round_prestart" was executed.
 *
 * @return                Returns the timestamp where "round_prestart" was executed.
 */
native int TTT_GetRoundTimestamp();

/**
 * Disable the next TTT round. It has no effect if it's already set.
 * If you want to close the disabled round:
 *  - set status to false
 *  - terminate roundd with TTT_TerminateRound
 *
 * @param status            If true, next round will be disabled.
 *                          If false, next round will be normal.
 */
native bool TTT_DisableRounds(bool status);

/**
 * Returns the current status of the round
 *
 */
native RoundStatus TTT_GetRoundStatus();

/**
 * Check if the client is ready to play
 *
 */
native bool TTT_IsClientReady(int client);

/**
 * Called after a player was round slayed.
 *
 * @param client            The client index.
 * @param remaining         Amount of rounds left
 */
forward int TTT_OnRoundSlay(int client, int remaining);

native int TTT_GetRoundID();

/**
 * Returns the player id for the specific player
 *
 * @param client            The client index
 * 
 * @return                  The player id
 */
native int TTT_GetPlayerID(int client);

/**
 * Returns the client index for the specific player id
 *
 * @param id                The player id
 * 
 * @return                  The client index
 */
native int TTT_GetClientOfPlayerID(int id);

native RoundStatus TTT_SetRoundStatus(RoundStatus status);

native void TTT_StopRoundTimer();

/**
 * Forces round to end
 *
 * @param delay           Time (in seconds) to delay before new round starts
 * @param reason          Reason for the round ending
 *
 */
native void TTT_TerminateRound(float delay, CSRoundEndReason reason);

native bool TTT_GetClientName(int client, char[] buffer, int size);

stock void TTT_GetRoleNameByID(int role, char[] name, int length = ROLE_LENGTH)
{
    if (role == TTT_TEAM_INNOCENT)
    {
        Format(name, length, "Innocent");
    }
    else if (role == TTT_TEAM_DETECTIVE)
    {
        Format(name, length, "Detective");
    }
    else if (role == TTT_TEAM_TRAITOR)
    {
        Format(name, length, "Traitor");
    }
    else
    {
        Format(name, length, "Unassigned");
    }
}

stock void TTT_GetRoleNameByShortName(const char[] shortName, char[] name, int length = ROLE_LENGTH)
{
    if (StrEqual(shortName, "inno", false))
    {
        Format(name, length, "Innocent");
    }
    else if (StrEqual(shortName, "dete", false))
    {
        Format(name, length, "Detective");
    }
    else if (StrEqual(shortName, "trai", false))
    {
        Format(name, length, "Traitor");
    }
    else
    {
        Format(name, length, "Unassigned");
    }
}

stock int TTT_GetRoleIDByName(const char[] name)
{
    if (StrEqual(name, "innocent", false))
    {
        return TTT_TEAM_INNOCENT;
    }
    else if (StrEqual(name, "detective", false))
    {
        return TTT_TEAM_DETECTIVE;
    }
    else if (StrEqual(name, "traitor", false))
    {
        return TTT_TEAM_TRAITOR;
    }
    else
    {
        return TTT_TEAM_UNASSIGNED;
    }
}

stock int TTT_GetRoleIDByShortName(const char[] shortName)
{
    if (StrEqual(shortName, "inno", false))
    {
        return TTT_TEAM_INNOCENT;
    }
    else if (StrEqual(shortName, "dete", false))
    {
        return TTT_TEAM_DETECTIVE;
    }
    else if (StrEqual(shortName, "trai", false))
    {
        return TTT_TEAM_TRAITOR;
    }
    else
    {
        return TTT_TEAM_UNASSIGNED;
    }
}

stock void TTT_GetShortRoleNameByID(int role, char[] shortName, int length = ROLE_LENGTH)
{
    if (role == TTT_TEAM_INNOCENT)
    {
        Format(shortName, length, "inno");
    }
    else if (role == TTT_TEAM_DETECTIVE)
    {
        Format(shortName, length, "dete");
    }
    else if (role == TTT_TEAM_TRAITOR)
    {
        Format(shortName, length, "trai");
    }
    else
    {
        Format(shortName, length, "unas");
    }
}

stock void TTT_GetShortRoleNameByName(const char[] name, char[] shortName, int length = ROLE_LENGTH)
{
    if (StrEqual(name, "Innocent", false))
    {
        Format(shortName, length, "inno");
    }
    else if (StrEqual(name, "Detective", false))
    {
        Format(shortName, length, "dete");
    }
    else if (StrEqual(name, "Traitor", false))
    {
        Format(shortName, length, "trai");
    }
    else
    {
        Format(shortName, length, "unas");
    }
}

/**
 * Returns if the client is alive or dead.
 *
 * @param client          Player's index.
 *
 * @return                True if the client is alive, false otherwise.
 */
native bool TTT_IsPlayerAlive(int client);

/**
 * Check if TTT plugin is loaded and running.
 */
stock bool TTT_IsLoaded()
{
    return LibraryExists("ttt");
}

/**
 * Check if a client index is valid.
 */
stock bool TTT_IsClientValid(int client)
{
    if (client > 0 && client <= MaxClients)
    {
        if (!IsClientInGame(client))
        {
            return false;
        }

        if (IsClientSourceTV(client))
        {
            return false;
        }

        return true;
    }
    return false;
}

#pragma deprecated TTT_HasCVarFlags is "unsafe". Please use TTT_CheckCommandAccess
stock bool TTT_HasCVarFlags(int client, ConVar cFlags)
{
    char flags[24];
    cFlags.GetString(flags, sizeof(flags));

    AdminFlag aFlags[24];
    FlagBitsToArray(ReadFlagString(flags), aFlags, sizeof(aFlags));
    
    return tHasFlags(client, aFlags);
}

#pragma deprecated TTT_HasFlags is "unsafe". Please use TTT_CheckCommandAccess
stock bool TTT_HasFlags(int client, const char[] flags)
{
    AdminFlag aFlags[24];
    FlagBitsToArray(ReadFlagString(flags), aFlags, sizeof(aFlags));
    
    return tHasFlags(client, aFlags);
}

stock bool tHasFlags(int client, AdminFlag flags[24])
{
    int iFlags = GetUserFlagBits(client);

    if (iFlags & ADMFLAG_ROOT)
    {
        return true;
    }

    for (int i = 0; i < sizeof(flags); i++)
    {
        if (iFlags & FlagToBit(flags[i]))
        {
            return true;
        }
    }

    return false;
}

stock bool TTT_RemoveWeaponByClassname(int client, const char[] classname, int slot)
{
    for(int i = 0; i < GetEntPropArraySize(client, Prop_Send, "m_hMyWeapons"); i++)
    {
        int weapon = GetEntPropEnt(client, Prop_Send, "m_hMyWeapons", i);

        if (IsValidEntity(weapon))
        {
            char sClass[32];
            GetEntityClassname(weapon, sClass, sizeof(sClass));

            if (StrEqual(sClass, classname, false))
            {
                return TTT_SafeRemoveWeapon(client, weapon, slot);
            }
        }
    }
    
    return false;
}

stock bool TTT_SafeRemoveWeapon(int client, int weapon, int slot)
{
    if (HasEntProp(weapon, Prop_Send, "m_iItemDefinitionIndex"))
    {
        int iDefIndex = GetEntProp(weapon, Prop_Send, "m_iItemDefinitionIndex");
        
        if (iDefIndex < 0 || iDefIndex > 700)
        {
            return false;
        }
    }
    
    if (HasEntProp(weapon, Prop_Send, "m_bInitialized"))
    {
        if (GetEntProp(weapon, Prop_Send, "m_bInitialized") == 0)
        {
            return false;
        }
    }
    
    if (HasEntProp(weapon, Prop_Send, "m_bStartedArming"))
    {
        if (GetEntSendPropOffs(weapon, "m_bStartedArming") > -1)
        {
            return false;
        }
    }
    
    if (GetPlayerWeaponSlot(client, slot) != weapon)
    {
        return false;
    }
    
    if (!RemovePlayerItem(client, weapon))
    {
        return false;
    }
    
    int iWorldModel = GetEntPropEnt(weapon, Prop_Send, "m_hWeaponWorldModel");
    
    if (IsValidEdict(iWorldModel) && IsValidEntity(iWorldModel))
    {
        if (!AcceptEntityInput(iWorldModel, "Kill"))
        {
            return false;
        }
    }
    
    if (weapon == GetEntPropEnt(client, Prop_Send, "m_hActiveWeapon"))
    {
        SetEntPropEnt(client, Prop_Send, "m_hActiveWeapon", -1);
    }
    
    AcceptEntityInput(weapon, "Kill");
    
    return true;
}

stock void TTT_ShowOverlayToClient(int client, const char[] overlaypath)
{
    int iFlags = GetCommandFlags("r_screenoverlay");

    SetCommandFlags("r_screenoverlay", iFlags &~ FCVAR_CHEAT);
    ClientCommand(client, "r_screenoverlay \"%s\"", overlaypath);
    SetCommandFlags("r_screenoverlay", iFlags);
}

stock void TTT_ShowOverlayToAll(const char[] overlaypath)
{
    LoopValidClients(i)
    {
        if (!IsFakeClient(i))
        {
            TTT_ShowOverlayToClient(i, overlaypath);
        }
    }
}

stock int TTT_GetRandomPlayer(bool alive = true, int targetRole = -1)
{
    int[] clients = new int[MaxClients + 1];
    int clientCount = 0;

    LoopValidClients(i)
    {
        if (alive && !TTT_IsPlayerAlive(i))
        {
            continue;
        }

        if (targetRole != -1 && TTT_IsValidRole(i, targetRole))
        {
            continue;
        }

        clients[clientCount++] = i;
    }

    if (clientCount == 0)
    {
        return -1;
    }

    SortIntegers(clients, clientCount-1, Sort_Random);

    return clients[GetRandomInt(0, clientCount-1)];
} 

stock bool TTT_IsWorldDamage(int iAttacker, int damagetype)
{
    if (damagetype == DMG_FALL
         || damagetype == DMG_GENERIC
         || damagetype == DMG_CRUSH
         || damagetype == DMG_SLASH
         || damagetype == DMG_BURN
         || damagetype == DMG_VEHICLE
         || damagetype == DMG_FALL
         || damagetype == DMG_BLAST
         || damagetype == DMG_SHOCK
         || damagetype == DMG_SONIC
         || damagetype == DMG_ENERGYBEAM
         || damagetype == DMG_DROWN
         || damagetype == DMG_PARALYZE
         || damagetype == DMG_NERVEGAS
         || damagetype == DMG_POISON
         || damagetype == DMG_ACID
         || damagetype == DMG_AIRBOAT
         || damagetype == DMG_PLASMA
         || damagetype == DMG_RADIATION
         || damagetype == DMG_SLOWBURN
         || iAttacker == 0
        )
    {
        return true;
    }
    return false;
}

stock void TTT_ClearTimer(Handle &timer, bool autoClose = false)
{
    if (timer != null)
    {
        KillTimer(timer, autoClose);
        timer = null;
    }
}

stock bool TTT_IsWarmUp()
{
    return view_as<bool>(GameRules_GetProp("m_bWarmupPeriod"));
}

stock int TTT_IsValidRole(int client, int role)
{
    return (role & TTT_GetClientRole(client));
}

stock bool TTT_IsPrimaryWeapon(int weapon)
{
    if (IsValidEntity(weapon))
    {
        char sClass[32];
        GetEntityClassname(weapon, sClass, sizeof(sClass));

        if ((StrContains(sClass, "ak47", false) != -1) || (StrContains(sClass, "m4a1", false) != -1) ||
            (StrContains(sClass, "m4a1_silencer", false) != -1) || (StrContains(sClass, "awp", false) != -1) ||
            (StrContains(sClass, "p90", false) != -1) || (StrContains(sClass, "sg556", false) != -1) ||
            (StrContains(sClass, "aug", false) != -1) || (StrContains(sClass, "galilar", false) != -1) ||
            (StrContains(sClass, "famas", false) != -1) || (StrContains(sClass, "ssg08", false) != -1) ||
            (StrContains(sClass, "g3sg1", false) != -1) || (StrContains(sClass, "scar20", false) != -1) ||
            (StrContains(sClass, "m249", false) != -1) || (StrContains(sClass, "negev", false) != -1) ||
            (StrContains(sClass, "nova", false) != -1) || (StrContains(sClass, "xm1014", false) != -1) ||
            (StrContains(sClass, "sawedoff", false) != -1) || (StrContains(sClass, "mag7", false) != -1) ||
            (StrContains(sClass, "mac10", false) != -1) || (StrContains(sClass, "mp9", false) != -1) ||
            (StrContains(sClass, "mp7", false) != -1) || (StrContains(sClass, "ump45", false) != -1) ||
            (StrContains(sClass, "bizon", false) != -1) || (StrContains(sClass, "mp5sd", false) != -1))
        {
            return true;
        }
    }

    return false;
}

stock void TTT_ShowProgressbar(int client, Handle percentageSync, Handle textSync, int startTime, int endTime, const char[] sMessage)
{
    int iEnd = endTime - startTime;
    int iCurrent = GetTime() - startTime;
    float fPercentage = ((100.0 / float(iEnd)) * float(iCurrent));

    char sProgress[128];
    for(int i = 0; i < fPercentage / 5; i++)
    {
        Format(sProgress, sizeof(sProgress), "%s█", sProgress);
    }
    
    int iColor[4]; //0(R) 1(G) 2(B) 3(A)

    if (fPercentage >= 90.0)
    {
        // Green
        iColor[0] = 0;      //r
        iColor[1] = 204;    //g
        iColor[2] = 0;      //b
        iColor[3] = 0;      //a
    }
    else if(fPercentage >= 75.0)
    {
        // Lightgreen
        iColor[0] = 153;    //r
        iColor[1] = 255;    //g
        iColor[2] = 0;      //b
        iColor[3] = 0;      //a
    }
    else if(fPercentage >= 50.0)
    {
        // Yellow
        iColor[0] = 255;    //r
        iColor[1] = 255;    //g
        iColor[2] = 0;      //b
        iColor[3] = 0;      //a
    }
    else if(fPercentage >= 30.0)
    {
        // Orange
        iColor[0] = 255;    //r
        iColor[1] = 165;    //g
        iColor[2] = 0;      //b
        iColor[3] = 0;      //a
    }
    else
    {
        // Red
        iColor[0] = 255;    //r
        iColor[1] = 0;      //g
        iColor[2] = 0;      //b
        iColor[3] = 0;      //a
    }

    SetHudTextParams(0.17, 0.09, 1.1, iColor[0], iColor[1], iColor[2], iColor[3], 0, 0.0, 0.0, 0.0);
    ShowSyncHudText(client, textSync, sMessage, fPercentage, sProgress);

    SetHudTextParams(0.17, 0.14, 1.1, iColor[0], iColor[1], iColor[2], iColor[3], 0, 0.0, 0.0, 0.0);
    ShowSyncHudText(client, percentageSync, "%.1f%%\n%s", fPercentage, sProgress);
}

stock int TTT_GetRoundTimeLeft()
{
    if (TTT_GetRoundTimestamp() == -1)
    {
        LogError("Invalid timestamp (%d)", TTT_GetRoundTimestamp());
        return -1;
    }

    ConVar cRoundTime = FindConVar("mp_roundtime");
    int iElapsed = (GetTime() - TTT_GetRoundTimestamp());
    return ((cRoundTime.IntValue * 60) - iElapsed);
}

stock int TTT_GetRoundTimeElapsed()
{
    if (TTT_GetRoundTimestamp() == -1)
    {
        LogError("Invalid timestamp (%d)", TTT_GetRoundTimestamp());
        return -1;
    }

    return (GetTime() - TTT_GetRoundTimestamp());
}

stock bool TTT_IsClientStuck(int client)
{
    float vOrigin[3], vMins[3], vMaxs[3];

    GetClientAbsOrigin(client, vOrigin);

    GetEntPropVector(client, Prop_Send, "m_vecMins", vMins);
    GetEntPropVector(client, Prop_Send, "m_vecMaxs", vMaxs);
    
    TR_TraceHullFilter(vOrigin, vOrigin, vMins, vMaxs, MASK_ALL, FilterOnlyPlayers, client);

    return TR_DidHit();
}

public bool FilterOnlyPlayers(int entity, int contentsMask, any data)
{
    if(entity != data && TTT_IsClientValid(entity) && TTT_IsClientValid(data))
    {
        return false;
    }
    else if (entity != data)
    {
        return true;
    }
    else
    {
        return false;
    }
}

stock int TTT_HasClientKnife(int client)
{
    for(int i = 0; i < GetEntPropArraySize(client, Prop_Send, "m_hMyWeapons"); i++)
    {
        int weapon = GetEntPropEnt(client, Prop_Send, "m_hMyWeapons", i);

        if (IsValidEntity(weapon))
        {
            char sClass[32];
            GetEntityClassname(weapon, sClass, sizeof(sClass));

            if ((StrContains(sClass, "knife", false) != -1) || (StrContains(sClass, "bayonet", false) != -1))
            {
                return weapon;
            }
        }
    }

    return -1;
}

stock int TTT_HasClientTaser(int client)
{
    for(int i = 0; i < GetEntPropArraySize(client, Prop_Send, "m_hMyWeapons"); i++)
    {
        int weapon = GetEntPropEnt(client, Prop_Send, "m_hMyWeapons", i);

        if (IsValidEntity(weapon))
        {
            char sClass[32];
            GetEntityClassname(weapon, sClass, sizeof(sClass));

            if (StrContains(sClass, "taser", false) != -1)
            {
                return weapon;
            }
        }
    }

    return -1;
}

stock int TTT_HasClientMelee(int client)
{
    for(int i = 0; i < GetEntPropArraySize(client, Prop_Send, "m_hMyWeapons"); i++)
    {
        int weapon = GetEntPropEnt(client, Prop_Send, "m_hMyWeapons", i);

        if (IsValidEntity(weapon))
        {
            char sClass[32];
            GetEntityClassname(weapon, sClass, sizeof(sClass));

            if (StrContains(sClass, "melee", false) != -1)
            {
                return weapon;
            }
        }
    }

    return -1;
}

stock int TTT_HasClientWeapon(int client, const char[] sWeapon)
{
    for(int i = 0; i < GetEntPropArraySize(client, Prop_Send, "m_hMyWeapons"); i++)
    {
        int weapon = GetEntPropEnt(client, Prop_Send, "m_hMyWeapons", i);

        if (IsValidEntity(weapon))
        {
            char sClass[32];
            GetEntityClassname(weapon, sClass, sizeof(sClass));

            if (StrContains(sClass, sWeapon, false) != -1)
            {
                return weapon;
            }
        }
    }

    return -1;
}

stock void TTT_RemoveMeleeWeapons(int client)
{
    for(int i = 0; i < GetEntPropArraySize(client, Prop_Send, "m_hMyWeapons"); i++)
    {
        int weapon = GetEntPropEnt(client, Prop_Send, "m_hMyWeapons", i);

        if (IsValidEntity(weapon))
        {
            char sClass[32];
            GetEntityClassname(weapon, sClass, sizeof(sClass));

            if ((StrContains(sClass, "melee", false) != -1) || (StrContains(sClass, "taser", false) != -1) || (StrContains(sClass, "knife", false) != -1) || (StrContains(sClass, "bayonet", false) != -1))
            {
                TTT_SafeRemoveWeapon(client, weapon, CS_SLOT_MELEE);
            }
        }
    }
}

stock bool TTT_LoadTranslations()
{
    char sFile[PLATFORM_MAX_PATH + 1];
    BuildPath(Path_SM, sFile, sizeof(sFile), "translations/ttt.custom.phrases.txt");

    if (FileExists(sFile))
    {
        LoadTranslations("ttt.custom.phrases");
        LogMessage("Custom translation file loaded.");
    }

    LoadTranslations("ttt.phrases");

    return true;
}

stock int TTT_ObserversOfTarget(int observers[MAXPLAYERS + 1], int target, float fMaxDistance = 0.0, float fThreshold = 0.73)
{
    float fTargetPosition[3];
    if (TTT_IsClientValid(target))
    {
        GetClientEyePosition(target, fTargetPosition);
    }
    else if (IsValidEntity(target) && HasEntProp(target, Prop_Send, "m_vecOrigin"))
    {
        GetEntPropVector(target, Prop_Send, "m_vecOrigin", fTargetPosition);
    }
    else
    {
        return -1;
    }

    int count = 0;
    LoopValidClients(i)
    {
        if (TTT_IsTargetInSightCommon(i, target, fTargetPosition, fMaxDistance, fThreshold))
        {
            observers[count] = i;
            ++count;
        }
    }

    return count;
}

stock bool TTT_TargetInSightOfClient(int client, int target, float fMaxDistance = 0.0, float fThreshold = 0.73)
{
    float fTargetPosition[3];
    if (TTT_IsClientValid(target))
    {
        GetClientEyePosition(target, fTargetPosition);
    }
    else if (IsValidEntity(target) && HasEntProp(target, Prop_Send, "m_vecOrigin"))
    {
        GetEntPropVector(target, Prop_Send, "m_vecOrigin", fTargetPosition);
    }

    if (IsNullVector(fTargetPosition))
    {
        return false;
    }

    return TTT_IsTargetInSightCommon(client, target, fTargetPosition, fMaxDistance, fThreshold);
}

/*
    Based on this snippet:
        https://forums.alliedmods.net/showpost.php?p=973411&postcount=4
*/
stock bool TTT_IsTargetInSightCommon(int client, int target, float fTargetPosition[3], float fMaxDistance = 0.0, float fThreshold = 0.73)
{
    // Retrieve view and target eyes position
    float fPosition[3];
    GetClientEyePosition(client, fPosition);

    float fAngle[3];
    GetClientEyeAngles(client, fAngle);

    // Calculate view direction
    float fDirection[3];
    fAngle[0] = fAngle[2] = 0.0;
    GetAngleVectors(fAngle, fDirection, NULL_VECTOR, NULL_VECTOR);

    // Calculate distance to client to see if it can be seen.
    float fDistance[3];
    fDistance[0] = fTargetPosition[0]-fPosition[0];
    fDistance[1] = fTargetPosition[1]-fPosition[1];
    fDistance[2] = 0.0;

    if (fMaxDistance != 0.0)
    {
        if ( ((fDistance[0] * fDistance[0]) + (fDistance[1] * fDistance[1])) >= (fMaxDistance * fMaxDistance) )
        {
            return false;
        }
    }

    // Check dot product. If it's negative, that means the client is facing
    // backwards to the target.
    float fTargetDirection[3];
    NormalizeVector(fDistance, fTargetDirection);
    if (GetVectorDotProduct(fDirection, fTargetDirection) < fThreshold)
    {
        return false;
    }

    // Now check if there are no obstacles in between through raycasting
    Handle hTrace = TR_TraceRayFilterEx(fPosition, fTargetPosition, PARTITION_NON_STATIC_EDICTS, RayType_EndPoint, TraceASDF);
    if (TR_DidHit(hTrace))
    {
        delete hTrace;
        return false;
    }

    delete hTrace;
    return true;
}

public bool TraceASDF(int entity, int mask)
{
    return entity >= MaxClients;
}

stock void PrintCenterText2(int client, const char[] title, const char[] message)
{
    char sBuffer[2048];
    Format(sBuffer, sizeof(sBuffer), "</font>     %s\n%s\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", title, message);
    Protobuf pbMessage = view_as<Protobuf>(StartMessageOne("TextMsg", client));
    pbMessage.SetInt("msg_dst", 4);
    pbMessage.AddString("params", "#SFUI_ContractKillStart");
    pbMessage.AddString("params", sBuffer);
    pbMessage.AddString("params", NULL_STRING);
    pbMessage.AddString("params", NULL_STRING);
    pbMessage.AddString("params", NULL_STRING);
    pbMessage.AddString("params", NULL_STRING);
    
    EndMessage();
}

stock void PrintCenterText2All(const char[] title, const char[] message, any ...)
{
    char sBuffer[2048];
    
    VFormat(sBuffer, sizeof(sBuffer), message, 3);
    Format(sBuffer, sizeof(sBuffer), "</font>     %s\n%s\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", title, sBuffer);

    Protobuf pbMessage = view_as<Protobuf>(StartMessageAll("TextMsg"));
    pbMessage.SetInt("msg_dst", 4);
    pbMessage.AddString("params", "#SFUI_ContractKillStart");
    pbMessage.AddString("params", sBuffer);
    pbMessage.AddString("params", NULL_STRING);
    pbMessage.AddString("params", NULL_STRING);
    pbMessage.AddString("params", NULL_STRING);
    pbMessage.AddString("params", NULL_STRING);
    
    EndMessage();
}

stock bool TTT_ClientValidRole(int client)
{
    int role = TTT_GetClientRole(client);

    if (role == TTT_TEAM_TRAITOR || role == TTT_TEAM_DETECTIVE || role == TTT_TEAM_INNOCENT)
    {
        return true;
    }

    return false;
}

stock bool IsStringNumeric(const char[] str)
{
	int x=0;
	int dotsFound=0;
	int numbersFound=0;

	if (str[x] == '+' || str[x] == '-')
    {
		x++;
	}

	while (str[x] != '\0') {

		if (IsCharNumeric(str[x]))
        {
			numbersFound++;
		}
		else if (str[x] == '.')
        {
			dotsFound++;

			if (dotsFound > 1)
            {
				return false;
			}
		}
		else
        {
			return false;
		}

		x++;
	}

	if (!numbersFound)
    {
		return false;
	}

	return true;
}
