#include "jkMain.h"

#include "../jk.h"
#include "Engine/sithNet.h"
#include "Engine/sith.h"
#include "Main/jkSmack.h"
#include "Main/jkGame.h"
#include "Main/jkCutscene.h"
#include "Main/jkHudInv.h"
#include "Main/jkEpisode.h"
#include "Gui/jkGUIRend.h"
#include "World/jkPlayer.h"
#include "World/jkSaber.h"
#include "Win95/stdControl.h"
#include "Win95/Windows.h"
#include "Win95/Video.h"
#include "stdPlatform.h"
#include "jkGame.h"
#include "Engine/sithSoundSys.h"
#include "Engine/sithSave.h"
#include "Engine/sithCamera.h"
#include "Win95/sithDplay.h"
#include "Gui/jkGUINet.h"
#include "Gui/jkGUIMultiTally.h"
#include "Engine/sithMulti.h"
#include "World/jkSaber.h"

#ifdef QOL_IMPROVEMENTS
#define TICKRATE_MS (1) // no cap
#else
#define TICKRATE_MS (20) // 50fps
#endif

#define jkMain_VideoShow ((void*)(0x00404270))
#define jkMain_VideoTick ((void*)(0x00404350))
#define jkMain_VideoLeave ((void*)(0x00404430))
#define jkMain_CutsceneShow ((void*)(0x00404450))
#define jkMain_CutsceneTick ((void*)(0x00404460))
#define jkMain_CutsceneLeave ((void*)(0x00404470))
#define jkMain_CreditsShow ((void*)(0x00404480))
#define jkMain_CreditsTick ((void*)(0x004044B0))
#define jkMain_CreditsLeave ((void*)(0x004044E0))
#define jkMain_UnkShow ((void*)(0x00404570))
#define jkMain_UnkTick ((void*)(0x00404580))
#define jkMain_UnkLeave ((void*)(0x004045F0))
#define jkMain_EscapeMenuShow ((void*)(0x00403F40))
#define jkMain_EscapeMenuLeave ((void*)(0x004040A0))
#define jkMain_EndLevelScreenShow ((void*)(0x004041A0))
#define jkMain_EndLevelScreenTick ((void*)(0x00404240))
#define jkMain_EndLevelScreenLeave ((void*)(0x00404250))
#define jkMain_CdSwitchShow ((void*)(0x00404260))

static jkEpisodeEntry* jkMain_pEpisodeEnt = NULL;
static jkEpisodeEntry* jkMain_pEpisodeEnt2 = NULL;

static jkGuiStateFuncs jkMain_aGuiStateFuncs[15] = {
    {0,  0,  0},
    {jkMain_VideoShow, jkMain_VideoTick, jkMain_VideoLeave},
    {jkMain_TitleShow, jkMain_TitleTick, jkMain_TitleLeave},
    {jkMain_MainShow, jkMain_MainTick, jkMain_MainLeave},
    {jkMain_VideoShow, jkMain_VideoTick, jkMain_VideoLeave},
    {jkMain_GameplayShow, jkMain_GameplayTick, jkMain_GameplayLeave},
    {jkMain_EscapeMenuShow, jkMain_EscapeMenuTick, jkMain_EscapeMenuLeave},
    {jkMain_CdSwitchShow,  0,  0},
    {jkMain_VideoShow, jkMain_VideoTick, jkMain_VideoLeave},
    {jkMain_EndLevelScreenShow, jkMain_EndLevelScreenTick, jkMain_EndLevelScreenLeave},
    {jkMain_VideoShow, jkMain_VideoTick, jkMain_VideoLeave},
    {jkMain_ChoiceShow, jkMain_ChoiceTick, jkMain_ChoiceLeave},
    {jkMain_CutsceneShow, jkMain_CutsceneTick, jkMain_CutsceneLeave},
    {jkMain_CreditsShow, jkMain_CreditsTick, jkMain_CreditsLeave},
    {jkMain_UnkShow, jkMain_UnkTick, jkMain_UnkLeave},
};

void jkMain_GuiAdvance()
{
    unsigned int v1; // esi
    int v3; // esi
    int v4; // esi
    void (__cdecl *v5)(int, int); // ecx
    int v6; // eax
    void (__cdecl *v7)(int, int); // ecx
    int (__cdecl *v8)(int); // ecx

    if ( !g_app_suspended )
    {
        if ( thing_nine )
            stdControl_ToggleCursor(0);
        if ( thing_eight )
        {
            if ( net_isMulti && !thing_six)
            {
                v1 = stdPlatform_GetTimeMsec();
                
                if (v1 > jkMain_lastTickMs + TICKRATE_MS)
                {
                    jkMain_lastTickMs = v1;
                    if (!sith_Tick()) return;
                }
                
                if ( g_sithMode == 5 )
                {
                    if ( jkGuiRend_thing_five )
                        jkGuiRend_thing_four = 1;
                    jkSmack_stopTick = 1;
                    jkSmack_nextGuiState = 3;
                    thing_nine = 0;
                    return;
                }
                if ( net_dword_832638 )
                {
                    net_dword_832638 = 0;
                    if ( net_isServer )
                        jkSaber_cogMsg_SendEndLevel();
                }
                if ( sith_bEndLevel )
                {
                    sith_bEndLevel = 0;
                    jkMain_EndLevel(1);
                }
                jkPlayer_nullsub_1(&playerThings[playerThingIdx]);
                dword_552B5C += stdPlatform_GetTimeMsec() - v1;
                v3 = stdPlatform_GetTimeMsec();
                if ( g_app_suspended && jkSmack_currentGuiState != 6 )
                    jkGame_Update();
                game_updateMsecsTotal += stdPlatform_GetTimeMsec() - v3;
            }
        }
        thing_nine = 0;
        return;
    }

    if ( !thing_nine )
    {
        switch ( jkSmack_currentGuiState )
        {
            case 1:
            case 4:
            case 8:
            case 10:
                jkCutscene_PauseShow(0);
                break;
            case 5:
                stdControl_ToggleCursor(1);
                jkGame_ddraw_idk_palettes(0);
                break;
            default:
                break;
        }
        stdControl_Flush();
        thing_nine = 1;
    }
    if ( jkSmack_stopTick && !jkGuiRend_thing_five )
    {
        jkGuiRend_thing_four = 0;
        v4 = jkSmack_currentGuiState;
        v5 = jkMain_aGuiStateFuncs[jkSmack_currentGuiState].leaveFunc;
        if ( v5 )
            v5(jkSmack_currentGuiState, jkSmack_nextGuiState);
        //jk_printf("leave %u\n", jkSmack_currentGuiState);

        v6 = jkSmack_nextGuiState;
        jkSmack_stopTick = 0;
        jkSmack_currentGuiState = jkSmack_nextGuiState;
        v7 = jkMain_aGuiStateFuncs[jkSmack_nextGuiState].showFunc;
        if ( !v7 )
            goto LABEL_35;
        v7(jkSmack_nextGuiState, v4);
        //jk_printf("show %u\n", jkSmack_currentGuiState);
    }
    v6 = jkSmack_currentGuiState;
LABEL_35:
    if ( !jkSmack_stopTick )
    {
        //jk_printf("tick %u %x\n", jkSmack_currentGuiState, jkMain_aGuiStateFuncs[v6].tickFunc);
        v8 = jkMain_aGuiStateFuncs[v6].tickFunc;
        if ( v8 )
            v8(v6);
    }
}

void jkMain_EscapeMenuTick(int a2)
{
    unsigned int v1; // esi
    int v3; // esi

    if ( net_isMulti )
    {
        if ( !thing_six )
        {
            if ( thing_eight )
            {
                v1 = stdPlatform_GetTimeMsec();
                
                if (v1 > jkMain_lastTickMs + TICKRATE_MS)
                {
                    jkMain_lastTickMs = v1;
                    if (sith_Tick()) return;
                }
                
                if ( g_sithMode == 5 )
                {
                    if ( jkGuiRend_thing_five )
                        jkGuiRend_thing_four = 1;
                    jkSmack_stopTick = 1;
                    jkSmack_nextGuiState = 3;
                }
                else
                {
                    if ( net_dword_832638 )
                    {
                        net_dword_832638 = 0;
                        if ( net_isServer )
                            jkSaber_cogMsg_SendEndLevel();
                    }
                    if ( sith_bEndLevel )
                    {
                        sith_bEndLevel = 0;
                        jkMain_EndLevel(1);
                    }
                    jkPlayer_nullsub_1(&playerThings[playerThingIdx]);
                    dword_552B5C += stdPlatform_GetTimeMsec() - v1;
                    v3 = stdPlatform_GetTimeMsec();
                    if ( g_app_suspended && a2 != 6 )
                        jkGame_Update();
                    game_updateMsecsTotal += stdPlatform_GetTimeMsec() - v3;
                }
            }
        }
    }
}

void jkMain_GameplayShow(int a1, int a2)
{
    signed int level_loaded; // esi
    signed int v3; // eax
    wchar_t *v4; // eax
    DWORD v5; // eax
    wchar_t *v6; // [esp-4h] [ebp-Ch]

    level_loaded = 0;
    if ( a2 == 6 )
    {
        sithSoundSys_ResumeAll();
        sithSoundSys_ResumeMusic(1);
        goto LABEL_39;
    }

    if ( jkSmack_gameMode == 4 )
    {
        jkPlayer_Startup();
        jkPlayer_InitForceBins();
        jkMain_bInit = 1;
        jkPlayer_InitSaber();
        sith_AutoSave();
LABEL_39:
        if ( jkMain_SetVideoMode() )
        {
            stdControl_ToggleCursor(1);
            stdControl_Flush();
            jkGame_Update();
            thing_eight = 1;
        }
        else
        {
            if ( jkGuiRend_thing_five )
                jkGuiRend_thing_four = 1;
            jkSmack_stopTick = 1;
            thing_eight = 1;
            jkSmack_nextGuiState = 3;
        }
        return;
    }

    if ( jkSmack_gameMode == 1 )
    {
        jkGui_copies_string(gamemode_1_str);
        jkGuiTitle_ShowLoading(gamemode_1_str, 0);
    }
    else
    {
        jkGui_copies_string(gamemode_0_2_str);
        jkGuiTitle_ShowLoading(gamemode_0_2_str, 0);
    }

    if ( jkSmack_gameMode )
    {
        if ( jkSmack_gameMode == 1 )
        {
            v3 = sithSave_Load(gamemode_0_2_str, 0, 1);
        }
        else
        {
            if ( jkSmack_gameMode != 2 )
                goto LABEL_15;
            v3 = sith_Mode1Init_3(gamemode_0_2_str);
        }
    }
    else
    {
        v3 = sith_Mode1Init(gamemode_0_2_str);
    }

    level_loaded = v3;
LABEL_15:
    jkGuiTitle_LoadingFinalize();
    if ( !level_loaded )
    {
        if ( jkGame_isDDraw )
        {
            Windows_ShutdownGdi();
            Video_SwitchToGDI(1);
            jkPlayer_nullsub_30();
            jkGame_isDDraw = 0;
        }
        if ( jkGuiRend_thing_five )
            jkGuiRend_thing_four = 1;
        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = 3;
        v6 = jkStrings_GetText("ERR_CANNOT_LOAD_LEVEL");
        v4 = jkStrings_GetText("ERROR");
        jkGuiDialog_ErrorDialog(v4, v6);
        return;
    }

    if ( !net_isMulti )
    {
        jkPlayer_Startup();
        jkPlayer_InitForceBins();
        jkMain_bInit = 1;
        if ( jkSmack_gameMode == 2 || !jkSmack_gameMode )
        {
            sithCamera_SetsFocus();
            jkPlayer_InitSaber();
            sith_AutoSave();
        }

        goto LABEL_39;
    }
    if ( net_isServer )
    {
LABEL_28:
        sithInventory_ClearInventory(g_localPlayerThing);
        jkPlayer_MpcInitBins((int)g_selfPlayerInfo);
        
        jkPlayer_Startup();
        jkPlayer_InitForceBins();
        jkMain_bInit = 1;
        if ( jkSmack_gameMode == 2 || !jkSmack_gameMode )
        {
            sithCamera_SetsFocus();
            jkPlayer_InitSaber();
            sith_AutoSave();
        }
        if ( net_isMulti )
        {
//#ifdef TARGET_HAS_DPLAY
            if ( net_isServer )
            {
                DirectPlay_SetSessionFlagidk(1);
                v5 = idx_13b4_related;
                if ( idx_13b4_related >= (unsigned int)jkPlayer_maxPlayers )
                    v5 = jkPlayer_maxPlayers;
                DirectPlay_SetSessionDesc((int)gamemode_0_2_str, v5);
            }
            if ( net_isMulti )
                jkSaber_cogMsg_wrap_SendSaberInfo_alt();
//#endif
        }
        goto LABEL_39;
    }
    thing_six = 1;
    stdControl_ToggleCursor(0);
    if ( jkGuiNet_ShowSynchronizing() == 1 )
    {
        thing_six = 0;
        stdControl_ToggleCursor(1);
        goto LABEL_28;
    }
    sith_Close();
    sithMulti_Shutdown();
    if ( jkGuiRend_thing_five )
        jkGuiRend_thing_four = 1;
    jkSmack_stopTick = 1;
    jkSmack_nextGuiState = 3;
    thing_six = 0;
}

void jkMain_GameplayTick(int a2)
{
    unsigned int v1; // esi
    int v3; // esi

    if ( !thing_six )
    {
        if ( thing_eight )
        {
            v1 = stdPlatform_GetTimeMsec();
            
            if (v1 > jkMain_lastTickMs + TICKRATE_MS)
            {
                jkMain_lastTickMs = v1;
                if (sith_Tick()) return;
            }
            
            if ( g_sithMode == 5 )
            {
                if ( jkGuiRend_thing_five )
                    jkGuiRend_thing_four = 1;
                jkSmack_stopTick = 1;
                jkSmack_nextGuiState = 3;
            }
            else
            {
                if ( net_dword_832638 )
                {
                    net_dword_832638 = 0;
                    if ( net_isServer )
                        jkSaber_cogMsg_SendEndLevel();
                }
                if ( sith_bEndLevel )
                {
                    sith_bEndLevel = 0;
                    jkMain_EndLevel(1);
                }
                jkPlayer_nullsub_1(&playerThings[playerThingIdx]);
                dword_552B5C += stdPlatform_GetTimeMsec() - v1;
                v3 = stdPlatform_GetTimeMsec();
                if ( g_app_suspended && a2 != 6 )
                    jkGame_Update();
                game_updateMsecsTotal += stdPlatform_GetTimeMsec() - v3;
            }
        }
    }
}

void jkMain_GameplayLeave(int a2, int a3)
{
    int v3; // eax

    if ( a3 == 6 )
    {
        stdControl_ToggleCursor(0);
        sithSoundSys_StopAll();
    }
    if ( jkGame_isDDraw )
    {
        Windows_ShutdownGdi();
        Video_SwitchToGDI();
        jkPlayer_nullsub_30();
        jkGame_isDDraw = 0;
    }
    if ( a3 != 6 && jkMain_bInit )
    {
        jkPlayer_Shutdown();
        sith_Close();
        jkMain_bInit = 0;
        thing_eight = 0;
    }
    if ( net_isMulti && a3 != 6 )
    {
        thing_eight = 0;
        if ( a3 == 3 )
            sithMulti_Shutdown();
        else
            sithMulti_LobbyMessage();
        if ( net_isServer )
            DirectPlay_SetSessionFlagidk(0);
        thing_six = 1;
        v3 = jkGuiMultiTally_Show(net_isMulti);
        thing_six = 0;
        if ( v3 == -1 )
        {
            sithMulti_Shutdown();
            if ( jkGuiRend_thing_five )
                jkGuiRend_thing_four = 1;
            jkSmack_stopTick = 1;
            jkSmack_nextGuiState = 3;
        }
    }
}

int jkMain_TitleShow()
{
    jkGuiTitle_ShowLoadingStatic();
    sith_Load("static.jkl");
#ifdef WIN32
    return jkHudInv_items_init();
#else
    return 1;
#endif
}

void jkMain_TitleTick()
{
    jkGuiTitle_LoadingFinalize();
    if ( jkGuiRend_thing_five )
        jkGuiRend_thing_four = 1;
    jkSmack_stopTick = 1;
    jkSmack_nextGuiState = 3;
}

void jkMain_TitleLeave()
{
    ;
}

void jkMain_MainShow()
{
    jkGuiMain_Show();
}

void jkMain_MainTick()
{
    ;
}

void jkMain_MainLeave()
{
    ;
}

void jkMain_ChoiceShow()
{
    int v1; // [esp+0h] [ebp-4h] BYREF

    if ( jkGuiForce_Show(0, 0.0, 1, 0, &v1, 1) == -1 )
    {
        if ( jkGuiRend_thing_five )
            jkGuiRend_thing_four = 1;
        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = 3;
    }
    else
    {
        jkMain_CdSwitch(0, v1);
    }
}

void jkMain_ChoiceTick()
{
    ;
}

void jkMain_ChoiceLeave()
{
    ;
}

int jkMain_sub_403470(char *a1)
{
    int result; // eax

    sithInventory_549FA0 = 1;
    _strncpy(gamemode_0_2_str, a1, 0x7Fu);
    result = 0;
    gamemode_0_2_str[127] = 0;
    jkSmack_gameMode = 0;
    if ( jkGuiRend_thing_five )
        jkGuiRend_thing_four = 1;
    jkSmack_stopTick = 1;
    jkSmack_nextGuiState = 5;
    return result;
}

int jkMain_LoadFile(char *a1)
{
    if (jkRes_LoadCD(1))
    {
        sithInventory_549FA0 = 1;
        jkRes_LoadGob(a1);
        if ( jkEpisode_mLoad.paEntries )
        {
            pHS->free(jkEpisode_mLoad.paEntries);
            jkEpisode_mLoad.paEntries = 0;
        }
        if ( jkEpisode_Load(&jkEpisode_mLoad) )
        {
            return jkMain_CdSwitch(1, 1);
        }
        else
        {
            Windows_ErrorMsgboxWide("ERR_CANNOT_LOAD_FILE %s", a1);
            return 0;
        }
    }
    return 0;
}

int jkMain_CdSwitch(int a1, int bIsAPath)
{
    jkEpisodeEntry *v2; // eax
    jkEpisodeEntry *v3; // ecx
    int v4; // eax
    signed int result; // eax

    if ( !jkEpisode_mLoad.numSeq )
    {
        if ( jkGuiRend_thing_five )
        {
            jkGuiRend_thing_four = 1;
        }
        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = 3;
        return 0;
    }
    if ( a1 )
    {
        v2 = jkEpisode_idk1(&jkEpisode_mLoad);
        v3 = v2;
        jkMain_pEpisodeEnt = v2;
        jkMain_pEpisodeEnt2 = v2;
        jkPlayer_dword_525470 = 0;
    }
    else
    {
        v3 = jkMain_pEpisodeEnt;
        v2 = jkMain_pEpisodeEnt2;
    }
    if ( jkPlayer_dword_525470 )
    {
        jkMain_pEpisodeEnt = jkEpisode_idk1(&jkEpisode_mLoad);
        v2 = jkEpisode_idk2(&jkEpisode_mLoad, bIsAPath);
        v3 = jkMain_pEpisodeEnt;
        jkMain_pEpisodeEnt2 = v2;
        jkPlayer_dword_525470 = 0;
    }
    if ( !v2 )
    {
        v4 = jkGuiRend_thing_five;
        if ( v3->gotoA == -1 )
        {
            if ( jkGuiRend_thing_five )
                jkGuiRend_thing_four = 1;
            jkSmack_stopTick = 1;
            jkSmack_nextGuiState = 13;
            return 1;
        }
        if ( v4 )
            jkGuiRend_thing_four = 1;

        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = 3;
        return 0;
    }
    if ( net_isMulti && (net_MultiModeFlags & 0x80u) != 0 )
    {
        v4 = jkGuiRend_thing_five;
        
        if ( v4 )
            jkGuiRend_thing_four = 1;
        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = 3;
        return 0;
    }
    if ( v3->level == v2->level || jkSmack_currentGuiState == 9 )
    {
        if ( v2->type == 1 && jkSmack_currentGuiState == 5 )
        {
            if ( jkGuiRend_thing_five )
                jkGuiRend_thing_four = 1;
            jkSmack_stopTick = 1;
            jkSmack_nextGuiState = 7;
            result = 1;
        }
        else
        {
            jkPlayer_dword_525470 = 1;
            jkMain_cd_swap_reverify(v2);
            result = 1;
        }
    }
    else
    {
        if ( jkGuiRend_thing_five )
            jkGuiRend_thing_four = 1;
        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = 9;
        result = 1;
    }
    return result;
}

int jkMain_cd_swap_reverify(jkEpisodeEntry *ent)
{
    int v1; // eax
    int v2; // eax
    signed int result; // eax
    signed int v4; // edi
    int v5; // edi
    signed int v6; // esi
    wchar_t *v7; // eax
    wchar_t *v8; // [esp-4h] [ebp-94h]
    char v9[128]; // [esp+10h] [ebp-80h] BYREF

    v1 = ent->type;
    if ( !v1 )
    {
        v5 = 0;
        v6 = 0;
        while ( !v6 )
        {
            if ( Windows_installType < 9 )
                v6 = jkRes_LoadCD(ent->cdNum);
            else
                v6 = 1;
            if ( !v6 )
            {
                v8 = jkStrings_GetText("GUI_CONFIRM_ABORTCD");
                v7 = jkStrings_GetText("GUI_ABORTCDREQUEST");
                if ( jkGuiDialog_YesNoDialog(v7, v8) )
                    v5 = 1;
            }
            if ( v5 )
            {
                if ( !v6 )
                {
                    if ( jkGuiRend_thing_five )
                        jkGuiRend_thing_four = 1;
                    jkSmack_stopTick = 1;
                    jkSmack_nextGuiState = 3;
                    return 1;
                }
                break;
            }
        }
        _strncpy(gamemode_0_2_str, ent->fileName, 0x7Fu);
        gamemode_0_2_str[127] = 0;
        jkSmack_gameMode = net_isMulti != 0 ? 2 : 0;
        if ( jkGuiRend_thing_five )
            jkGuiRend_thing_four = 1;
        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = 5;
        return 1;
    }
    v2 = v1 - 1;
    if ( v2 )
    {
        if ( v2 == 1 )
        {
            if ( jkGuiRend_thing_five )
                jkGuiRend_thing_four = 1;
            jkSmack_stopTick = 1;
            jkSmack_nextGuiState = 11; // force select/choice?
            return 1;
        }
        return 1;
    }
    if ( jkPlayer_setDisableCutscenes )
        v4 = 0;
    else
        v4 = jkRes_LoadCD(ent->cdNum);

    jkPlayer_WriteConfSwap(&playerThings[playerThingIdx], ent->cdNum, ent->fileName);
    if ( !v4 )
        return jkMain_CdSwitch(0, 1);
    _sprintf(v9, "video%c%s", 92, ent->fileName);
    if ( !util_FileExists(v9) )
        return jkMain_CdSwitch(0, 1);
    jkRes_FileExists(v9, gamemode_0_2_str, 128);
    switch ( jkSmack_currentGuiState )
    {
        case 3:
        case 9:
            if ( jkGuiRend_thing_five )
                jkGuiRend_thing_four = 1;
            jkSmack_stopTick = 1;
            jkSmack_nextGuiState = 10;
            result = 1;
            break;
        case 5:
        case 7:
        case 8:
            if ( jkGuiRend_thing_five )
                jkGuiRend_thing_four = 1;
            jkSmack_stopTick = 1;
            jkSmack_nextGuiState = 8;
            result = 1;
            break;
        default:
            return 1;
    }
    return result;
}
