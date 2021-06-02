#include "sithCog.h"

#include "jk.h"
#include "types.h"
#include "Cog/sithCogUtil.h"
#include "Cog/sithCogThing.h"
#include "Cog/sithCogPlayer.h"
#include "Cog/sithCogAI.h"
#include "Cog/sithCogSurface.h"
#include "Cog/sithCogSector.h"
#include "Cog/sithCogSound.h"
#include "Cog/sithCogVm.h"
#include "Cog/sithCogParse.h"
#include "jkCog.h"
#include "Engine/sithTimer.h"
#include "Engine/sithSound.h"
#include "Engine/sithKeyFrame.h"
#include "Engine/sithMaterial.h"
#include "Engine/sithModel.h"
#include "Engine/sithTemplate.h"
#include "AI/sithAIClass.h"
#include "General/stdHashTable.h"
#include "World/sithSector.h"
#include "stdPlatform.h"

#include "jk.h"

void (*sithDebugConsole_CmdCogList)(void) = (void*)0x004EE2F0;
static int (*sithCogScript_TimerTick)() = (void*)0x4E0640;

static int sithCog_bInitted = 0;

int sithCog_Startup()
{
    struct hashmap_entry *v3; // eax
    hashmap_entry *v4; // eax
    hashmap_entry *v5; // eax
    hashmap_entry *v6; // eax
    hashmap_entry *v7; // eax
    struct cogSymbol a2; // [esp+8h] [ebp-10h]

    g_cog_symbolTable = sithCogParse_NewSymboltable(512);
    if (!g_cog_symbolTable )
    {
        jk_assert(pSithHS->errorPrint, ".\\Cog\\sithCog.c", 118, "Could not allocate COG symboltable.");
        return 0;
    }
  
    sithCog_pScriptHashtable = stdHashTable_New(256);
    if (!sithCog_pScriptHashtable)
    {
        jk_assert(pSithHS->errorPrint, ".\\Cog\\sithCog.c", 124, "Could not allocate COG hashtable.");
        return 0;
    }
    g_cog_symbolTable->bucket_idx = 0x100;
    sithCogUtil_Initialize(g_cog_symbolTable);
    sithCogThing_Initialize(g_cog_symbolTable);
    sithCogAI_Initialize(g_cog_symbolTable);
    sithCogSurface_Initialize(g_cog_symbolTable);
    sithCogSound_Initialize(g_cog_symbolTable);
    sithCogSector_Initialize(g_cog_symbolTable);
    sithCogPlayer_Initialize(g_cog_symbolTable);
	sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 1, "activate");
	sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 1, "activated");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 3, "startup");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 4, "timer");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 5, "blocked");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 6, "entered");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 7, "exited");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 8, "crossed");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 9, "sighted");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 10, "damaged");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 11, "arrived");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 12, "killed");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 13, "pulse");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 14, "touched");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 15, "created");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 16, "loading");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 17, "selected");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 18, "deselected");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 20, "changed");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 21, "deactivated");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 22, "shutdown");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 23, "respawn");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 2, "removed");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 19, "autoselect");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 24, "aievent");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 25, "skill");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 26, "taken");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 27, "user0");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 28, "user1");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 29, "user2");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 30, "user3");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 31, "user4");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 32, "user5");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 33, "user6");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 34, "user7");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 35, "newplayer");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 36, "fire");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 37, "join");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 38, "leave");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 39, "splash");
    sithCogScript_RegisterMessageSymbol(g_cog_symbolTable, 40, "trigger");
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global0", 0);
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global1", 0);
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global2", 0);
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global3", 0);
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global4", 0);
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global5", 0);
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global6", 0);
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global7", 0);
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global8", 0);
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global9", 0);
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global10", 0);
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global11", 0);
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global12", 0);
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global13", 0);
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global14", 0);
    sithCogScript_RegisterGlobalMessage(g_cog_symbolTable, "global15", 0);
    sithTimer_RegisterFunc(4, sithCogScript_TimerTick, 0, 2);
    sithCog_bInitted = 1;
    return 1;
}

void sithCog_Shutdown()
{
    sithCogParse_FreeSymboltable(g_cog_symbolTable);
    if ( sithCog_pScriptHashtable )
    {
        stdHashTable_Free(sithCog_pScriptHashtable);
        sithCog_pScriptHashtable = 0;
    }
    sithCogParse_Reset();
    sithCog_bInitted = 0;
}

int sithCog_Open()
{
    sithWorld *world; // ecx
    signed int result; // eax
    sithCog *v2; // ebx
    sithCogIdk *v3; // ebp
    sithCog *v5; // ebp
    sithCogIdk *v6; // ebx
    char *v7; // esi
    sithCogSymbol *v8; // edx
    int v10; // [esp+4h] [ebp-14h]
    int v12; // [esp+8h] [ebp-10h]
    char *v13; // [esp+Ch] [ebp-Ch]
    sithCogSymbol *v14; // [esp+10h] [ebp-8h]
    sithWorld *world_; // [esp+14h] [ebp-4h]

    world = sithWorld_pCurWorld;
    world_ = sithWorld_pCurWorld;
    if ( sithCog_bOpened )
        return 0;
    if ( sithWorld_pStatic )
    {
        v2 = sithWorld_pStatic->cogs;
        for (int i = 0; i < sithWorld_pStatic->numCogsLoaded; i++)
        {
            for (int j = 0; i < v2->cogscript->numIdk; j++)
            {
                v3 = &v2->cogscript->aIdk[j];
                if ( _strlen(v3->value) )
                    sithCog_LoadEntry(&v2->symbolTable->buckets[v3->hash], v3, v3->value);
            }
            sithCog_SendMessage(v2++, SITH_MESSAGE_LOADING, 0, 0, 0, 0, 0);
            world = world_;
        }
    }
    sithCog* cogs = world->cogs;
    v12 = 0;
    if ( world->numCogsLoaded )
    {
        while ( 1 )
        {
            v10 = 0;
            v6 = cogs->cogscript->aIdk;
            if ( cogs->cogscript->numIdk )
                break;
LABEL_25:
            sithCog_SendMessage(cogs++, SITH_MESSAGE_LOADING, 0, 0, 0, 0, 0);
            if ( (unsigned int)++v12 >= world_->numCogsLoaded )
                goto LABEL_26;
        }

        v13 = cogs->field_4BC;
        while ( 1 )
        {
            sithCogIdk* idk = &cogs->cogscript->aIdk[v10];
            v8 = &cogs->symbolTable->buckets[idk->hash];
            v14 = v8;
            if ( (idk->flags & 1) != 0 )
            {
                if ( _strlen(idk->value) )
                    sithCog_LoadEntry(v8, v6, idk->value);
                goto LABEL_24;
            }
            if ( _strlen(v13) )
                break;
            if ( _strlen(idk->value) )
            {
                sithCog_LoadEntry(v8, v6, idk->value);
                goto LABEL_20;
            }
LABEL_21:
            v13 += 32;
            sithCog_ThingsSectorsRegSymbolIdk(cogs, v6, v8);
LABEL_24:
            ++v6;
            if ( (unsigned int)++v10 >= cogs->cogscript->numIdk )
                goto LABEL_25;
        }
        sithCog_LoadEntry(v8, v6, v13);
LABEL_20:
        v8 = v14;
        goto LABEL_21;
    }
LABEL_26:
    result = 1;
    sithCog_bOpened = 1;
    return result;
}

int sithCog_Load(sithWorld *world, int a2)
{
    int num_cogs; // esi
    signed int result; // eax
    sithCog *cogs; // eax
    char *v6; // esi
    unsigned int v7; // eax
    int *v8; // ebx
    sithCog* v9; // eax
    sithCog *v10; // edi
    sithWorld *lvl; // ebp
    sithCogScript *existing_cogscript; // eax
    sithCogScript *cogscript; // esi
    unsigned int v15; // eax
    sithCogSymboltable *cogscript_symboltable; // edx
    int v17; // ecx
    sithCogScript *v18; // ebp
    char **v19; // edi
    unsigned int v23; // [esp+14h] [ebp-84h]
    char cog_fpath[32]; // [esp+18h] [ebp-80h] BYREF

    if ( a2 )
        return 0;
    stdConffile_ReadArgs();
    if ( _strcmp(stdConffile_entry.args[0].value, "world") || _strcmp(stdConffile_entry.args[1].value, "cogs") )
        return 0;
    num_cogs = _atoi(stdConffile_entry.args[2].value);
    if ( !num_cogs )
        return 1;
    cogs = (sithCog *)pSithHS->alloc(sizeof(sithCog) * num_cogs);
    world->cogs = cogs;
    if (!cogs)
    {
        stdPrintf(pSithHS->errorPrint, ".\\Cog\\sithCog.c", 883, "Memory alloc failure initializing COGs.\n", 0, 0, 0, 0);
        return 0;
    }
    
    _memset(cogs, 0, sizeof(sithCog) * num_cogs);
    world->numCogs = num_cogs;
    world->numCogsLoaded = 0;
    while ( stdConffile_ReadArgs() )
    {
        if ( !_strcmp(stdConffile_entry.args[0].value, "end") )
            break;
        if ( stdConffile_entry.numArgs < 2u )
            return 0;

        v6 = stdConffile_entry.args[1].value;
        v7 = sithWorld_pLoading->numCogsLoaded;
        v8 = &sithWorld_pLoading->numCogsLoaded;
        if ( v7 < sithWorld_pLoading->numCogs )
        {
            v10 = &sithWorld_pLoading->cogs[v7];
            v10->selfCog = v7;
            if (sithWorld_pLoading->level_type_maybe & 1)
            {
                v10->selfCog |= 0x8000u;
            }
            lvl = sithWorld_pLoading;
            _sprintf(cog_fpath, "%s%c%s", "cog", 92, v6);
            existing_cogscript = (sithCogScript *)stdHashTable_GetKeyVal(sithCog_pScriptHashtable, v6);
            if ( existing_cogscript )
            {
                cogscript = existing_cogscript;
            }
            else
            {
                v15 = lvl->numCogScriptsLoaded;
                if ( v15 < lvl->numCogScripts && (cogscript = &lvl->cogScripts[v15], sithCogParse_Load(cog_fpath, cogscript, 0)) )
                {
                    stdHashTable_SetKeyVal(sithCog_pScriptHashtable, cogscript->cog_fpath, cogscript);
                    ++lvl->numCogScriptsLoaded;
                }
                else
                {
                    cogscript = 0;
                }
            }
            if ( cogscript )
            {
                _strncpy(v10->cogscript_fpath, cogscript->cog_fpath, 0x1Fu);
                cogscript_symboltable = cogscript->symbolTable;
                v17 = cogscript->debug_maybe;
                v10->cogscript_fpath[31] = 0;
                v10->cogscript = cogscript;
                v10->flags = v17;
                v10->symbolTable = sithCogParse_CopySymboltable(cogscript_symboltable);
                if ( v10->symbolTable )
                {
                    ++*v8;
                    v9 = &v10->cogscript;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                v9 = 0;
            }
        }
        else
        {
            v9 = 0;
        }
        if ( v9 )
        {
            v18 = v9->cogscript;
            char* v21 = &v9->field_4BC[0];
            unsigned int v22 = 2;
            for (int i = 0; i < v18->numIdk; i++)
            {
                stdConffileArg* entry = &stdConffile_entry.args[2+i];
                
                if ( !(v18->aIdk[i].flags & 1) && stdConffile_entry.numArgs > v22 )
                {
                    _strncpy(v21, entry->value, 0x1Fu);
                    v21[31] = 0;
                    v21 += 32;
                    ++v22;
                }
            }
        }
    }
    return 1;
}

sithCog* sithCog_LoadCogscript(const char *fpath)
{
    sithWorld *v1; // edi
    unsigned int v2; // eax
    sithCogSymboltable *result; // eax
    sithCog *v5; // ebx
    sithCogScript *v7; // eax
    sithCogScript *v8; // esi
    unsigned int v9; // eax
    char v12[128]; // [esp+10h] [ebp-80h] BYREF

    v1 = sithWorld_pLoading;
    v2 = sithWorld_pLoading->numCogsLoaded;
    if ( v2 >= sithWorld_pLoading->numCogs )
        return 0;
    v5 = &sithWorld_pLoading->cogs[v2];
    v5->selfCog = v2;
    if (sithWorld_pLoading->level_type_maybe & 1)
    {
        v5->selfCog |= 0x8000;
    }
    _sprintf(v12, "%s%c%s", "cog", 92, fpath);
    v7 = (sithCogScript *)stdHashTable_GetKeyVal(sithCog_pScriptHashtable, fpath);
    if ( v7 )
    {
        v8 = v7;
    }
    else
    {
        v9 = v1->numCogScriptsLoaded;
        if ( v9 < v1->numCogScripts && (v8 = &v1->cogScripts[v9], sithCogParse_Load(v12, v8, 0)) )
        {
            stdHashTable_SetKeyVal(sithCog_pScriptHashtable, v8->cog_fpath, v8);
            ++v1->numCogScriptsLoaded;
        }
        else
        {
            v8 = 0;
        }
    }
    if ( !v8 )
        return 0;
    _strncpy(v5->cogscript_fpath, v8->cog_fpath, 0x1Fu);
    v5->cogscript_fpath[31] = 0;
    v5->cogscript = v8;
    v5->flags = v8->debug_maybe;
    v5->symbolTable = sithCogParse_CopySymboltable(v8->symbolTable);
    if ( v5->symbolTable )
    {
        sithWorld_pLoading->numCogsLoaded++;
        return v5;
    }
    return NULL;
}

int sithCog_LoadEntry(sithCogSymbol *cogSymbol, sithCogIdk *cogIdk, char *val)
{
    sithCogSymbol *v5; // esi
    sithCogSymbol *v7; // ecx
    sithCogSymbol *v9; // esi
    rdMaterial *v10; // eax
    sithSound *v12; // eax
    sithThing *v14; // eax
    rdModel3 *v15; // eax
    rdKeyframe *v17; // eax
    sithAIClass *v19; // eax

    switch ( cogIdk->type )
    {
        case COG_TYPE_FLEX:
            cogSymbol->symbol_type = COG_VARTYPE_FLEX;
            cogSymbol->as_flex = _atof(val);
            return 1;

        case COG_TYPE_TEMPLATE:
            cogSymbol->symbol_type = COG_VARTYPE_INT;
            v14 = sithTemplate_GetEntryByName(val);
            if ( !v14 )
            {
                cogSymbol->as_int = -1;
                return 0;
            }
            cogSymbol->symbol_name = (char *)v14->thingIdx;
            return 1;

        case COG_TYPE_KEYFRAME:
            cogSymbol->symbol_type = COG_VARTYPE_INT;
            v17 = sithKeyFrame_LoadEntry(val);
            if ( !v17 )
            {
                cogSymbol->as_int = -1;
                return 0;
            }
            cogSymbol->as_int = v17->id;
            return 1;
        case COG_TYPE_SOUND:
            cogSymbol->symbol_type = COG_VARTYPE_INT;
            v12 = sithSound_LoadEntry(val, 0);
            if ( !v12 )
            {
                cogSymbol->as_int = -1;
                return 0;
            }
            cogSymbol->as_int = v12->id;
            return 1;
        case COG_TYPE_MATERIAL:
            cogSymbol->symbol_type = COG_VARTYPE_INT;
            v10 = sithMaterial_LoadEntry(val, 0, 0);
            if ( !v10 )
            {
                cogSymbol->as_int = -1;
                return 0;
            }
            cogSymbol->as_int = v10->id;
            return 1;
        case COG_TYPE_VECTOR:
            cogSymbol->symbol_type = COG_VARTYPE_VECTOR;
            if (_sscanf(val, "(%f/%f/%f)", &cogSymbol->as_flex, &cogSymbol->field_C, &cogSymbol->field_10) == 3 )
            {
                return 1;
            }
            else
            {
                cogSymbol->as_int = 0;
                cogSymbol->field_C = 0;
                cogSymbol->field_10 = 0;
                return 0;
            }
            break;

        case COG_TYPE_MODEL:
            cogSymbol->symbol_type = COG_VARTYPE_INT;
            v15 = sithModel_LoadEntry(val, 1);
            if ( !v15 )
            {
                cogSymbol->as_int = -1;
                return 0;
            }
            cogSymbol->as_int = v15->id;
            return 1;

        case COG_TYPE_AICLASS:
            cogSymbol->symbol_type = COG_VARTYPE_INT;
            v19 = sithAIClass_Load(val);
            if ( v19 )
            {
                cogSymbol->as_aiclass = v19;
                return 1;
            }
            else
            {
                cogSymbol->as_int = -1;
                return 0;
            }
            break;

        default:
            cogSymbol->symbol_type = COG_VARTYPE_INT;
            cogSymbol->as_int = _atoi(val);
            return 1;
    }
}


void sithCog_SendMessageFromThing(sithThing *a1, sithThing *a2, int msg)
{
    sithCog_SendMessageFromThingEx(a1, a2, msg, 0.0, 0.0, 0.0, 0.0);
}

// ex

void sithCog_SendMessageFromSector(sithSector *sector, sithThing *thing, int message)
{
    sithCog_SendMessageFromSectorEx(sector, thing, message, 0.0, 0.0, 0.0, 0.0);
}

float sithCog_SendMessageFromSectorEx(sithSector *a1, sithThing *sourceType, SITH_MESSAGE message, float param0, float param1, float param2, float param3)
{
    int v8; // ebp
    double v11; // st7
    double v12; // st7
    float v13; // [esp+10h] [ebp-Ch]
    int v14; // [esp+14h] [ebp-8h]
    int sourceTypea; // [esp+24h] [ebp+8h]

    v13 = 0.0;
    if ( sourceType )
    {
        v8 = sourceType->thingIdx;
        sourceTypea = SENDERTYPE_THING;
        v14 = 1 << sourceType->thingType;
    }
    else
    {
        v8 = -1;
        sourceTypea = 0;
        v14 = 1;
    }
    if ( &sithCog_aSectorLinks[sithCog_numSectorLinks] > sithCog_aSectorLinks )
    {
        for (int i = 0; i < sithCog_numSectorLinks; i++)
        {
            sithCogSectorLink* link = &sithCog_aSectorLinks[i];
            if ( link->sector == a1 && (link->mask & v14) != 0 )
            {
                if ( message == SITH_MESSAGE_DAMAGED )
                {
                    v11 = sithCog_SendMessageEx(
                              link->cog,
                              SITH_MESSAGE_DAMAGED,
                              SENDERTYPE_SECTOR,
                              a1->id,
                              sourceTypea,
                              v8,
                              link->linkid,
                              param0,
                              param1,
                              param2,
                              param3);
                    if ( v11 == -9999.9873 )
                    {
                        v13 = param0;
                    }
                    else
                    {
                        v13 = v11;
                        param0 = v11;
                    }
                }
                else
                {
                    v12 = sithCog_SendMessageEx(link->cog, message, SENDERTYPE_SECTOR, a1->id, sourceTypea, v8, link->linkid, param0, param1, param2, param3);
                    if ( v12 != -9999.9873 )
                        v13 = v12 + v13;
                }
            }
        }
    }
    
    return v13;
}

int sithCogScript_Load(sithWorld *lvl, int a2)
{
    int numCogScripts; // esi
    signed int result; // eax
    sithCogScript *cogScripts; // edi
    char *v5; // esi
    sithWorld *v6; // edi
    unsigned int v7; // eax
    sithCogScript *v8; // esi
    char cog_fpath[128]; // [esp+10h] [ebp-80h] BYREF

    if ( a2 )
        return 0;
    stdConffile_ReadArgs();
    if ( _strcmp(stdConffile_entry.args[0].value, "world") || _strcmp(stdConffile_entry.args[1].value, "scripts") )
        return 0;
    numCogScripts = _atoi(stdConffile_entry.args[2].value);
    if ( !numCogScripts )
        return 1;
    cogScripts = (sithCogScript *)pSithHS->alloc(sizeof(sithCogScript) * numCogScripts);
    lvl->cogScripts = cogScripts;
    if ( cogScripts )
    {
        _memset(cogScripts, 0, sizeof(sithCogScript) * numCogScripts);
        lvl->numCogScripts = numCogScripts;
        lvl->numCogScriptsLoaded = 0;
        while ( stdConffile_ReadArgs() )
        {
            if ( !_strcmp(stdConffile_entry.args[0].value, "end") )
                break;
            if ( lvl->numCogScriptsLoaded < (unsigned int)lvl->numCogScripts )
            {
                if ( !stdConffile_entry.numArgs )
                    return 0;
                v5 = stdConffile_entry.args[1].value;
                v6 = sithWorld_pLoading;
                _sprintf(cog_fpath, "%s%c%s", "cog", '\\', stdConffile_entry.args[1].value);
                if ( !stdHashTable_GetKeyVal(sithCog_pScriptHashtable, v5) )
                {
                    v7 = v6->numCogScriptsLoaded;
                    if ( v7 < v6->numCogScripts )
                    {
                        v8 = &v6->cogScripts[v7];
                        if ( sithCogParse_Load(cog_fpath, v8, 0) )
                        {
                            stdHashTable_SetKeyVal(sithCog_pScriptHashtable, v8->cog_fpath, v8);
                            ++v6->numCogScriptsLoaded;
                        }
                    }
                }
            }
        }
        result = 1;
    }
    else
    {
        stdPrintf(pSithHS->errorPrint, ".\\Cog\\sithCog.c", 843, "Memory alloc failure initializing COG scripts.\n", 0, 0, 0, 0);
        result = 0;
    }
    return result;
}

void sithCogScript_RegisterVerb(sithCogSymboltable *a1, intptr_t a2, char *a3)
{
    sithCogStackvar a2a;

    sithCogSymbol* symbol = sithCogParse_AddSymbol(a1, a3);
    if ( symbol )
    {
        a2a.type = COG_TYPE_VERB;
        a2a.data[0] = a2;
        sithCogParse_SetSymbolVal(symbol, &a2a);
    }
}

void sithCogScript_RegisterMessageSymbol(sithCogSymboltable *a1, int a2, const char *a3)
{
    sithCogStackvar a2a; // [esp+0h] [ebp-10h] BYREF

    sithCogSymbol* v3 = sithCogParse_AddSymbol(a1, a3);
    if ( v3 )
    {
        a2a.type = COG_TYPE_INT;
        a2a.data[0] = a2;
        sithCogParse_SetSymbolVal(v3, &a2a);
    }
}

void sithCogScript_RegisterGlobalMessage(sithCogSymboltable *a1, const char *a2, int a3)
{
    sithCogStackvar a2a; // [esp+0h] [ebp-10h] BYREF

    sithCogSymbol* v3 = sithCogParse_AddSymbol(a1, a2);
    if ( v3 )
    {
        a2a.type = COG_TYPE_FLEX;
        a2a.data[0] = a3;
        sithCogParse_SetSymbolVal(v3, &a2a);
    }
}
