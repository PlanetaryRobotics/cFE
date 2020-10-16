/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/*
**  File:
**  cfe_es_apps.h
**
**  Purpose:
**  This file contains the Internal interface for the cFE Application control functions of ES.
**  These functions and data structures manage the Applications and Child tasks in the cFE.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
*/


#ifndef _cfe_es_apps_
#define _cfe_es_apps_

/*
** Include Files
*/
#include "common_types.h"
#include "osapi.h"

/*
** Macro Definitions
*/
#define CFE_ES_STARTSCRIPT_MAX_TOKENS_PER_LINE      8

/*
** Type Definitions
*/

/*
** CFE_ES_AppState_t is a structure of information for External cFE Apps.
**   This information is used to control/alter the state of External Apps.
**   The fields in this structure are not needed or used for the cFE Core Apps.
*/
typedef struct
{
    uint32     AppControlRequest;   /* What the App should be doing next */
    int32      AppTimerMsec;        /* Countdown timer for killing an app, in milliseconds */

} CFE_ES_ControlReq_t;


/*
** CFE_ES_AppStartParams_t is a structure of information used when an application is
**   created in the system. It is stored in the cFE ES App Table
*/
typedef struct
{
  char                  Name[OS_MAX_API_NAME];
  char                  EntryPoint[OS_MAX_API_NAME];
  char                  FileName[OS_MAX_PATH_LEN];

  uint32                StackSize;
  cpuaddr               StartAddress;
  osal_id_t             ModuleId;

  uint16                ExceptionAction;
  uint16                Priority;

} CFE_ES_AppStartParams_t;

/*
** CFE_ES_AppRecord_t is an internal structure used to keep track of
** CFE Applications that are active in the system.
*/
typedef struct
{
   CFE_ES_ResourceID_t     AppId;                       /* The actual AppID of this entry, or undefined */
   CFE_ES_AppState_Enum_t  AppState;                    /* Is the app running, or stopped, or waiting? */
   uint32                  Type;                        /* The type of App: CORE or EXTERNAL */
   CFE_ES_AppStartParams_t StartParams;                 /* The start parameters for an App */
   CFE_ES_ControlReq_t     ControlReq;                  /* The Control Request Record for External cFE Apps */
   CFE_ES_ResourceID_t     MainTaskId;                  /* The Application's Main Task ID */

} CFE_ES_AppRecord_t;


/*
** CFE_ES_TaskRecord_t is an internal structure used to keep track of
** CFE Tasks that are active in the system.
*/
typedef struct
{
   CFE_ES_ResourceID_t     TaskId;            /* The actual TaskID of this entry, or undefined */
   CFE_ES_ResourceID_t     AppId;             /* The parent Application's App ID */
   uint32    ExecutionCounter;                /* The execution counter for the Child task */
   char      TaskName[OS_MAX_API_NAME];       /* Task Name */


} CFE_ES_TaskRecord_t;

/*
** CFE_ES_LibRecord_t is an internal structure used to keep track of
** CFE Shared Libraries that are loaded in the system.
*/
typedef struct
{
   CFE_ES_ResourceID_t     LibId;             /* The actual LibID of this entry, or undefined */
   char      LibName[OS_MAX_API_NAME];        /* Library Name */
} CFE_ES_LibRecord_t;

/*
** CFE_ES_AppTableScanState_t is an internal structure used to keep state of
** the background app table scan/cleanup process
*/
typedef struct
{
    uint32 PendingAppStateChanges;
    uint32 BackgroundScanTimer;
    uint8  LastScanCommandCount;
} CFE_ES_AppTableScanState_t;



/*****************************************************************************/
/*
** Function prototypes
*/

/*
** Internal function start applications based on the startup script
*/
void  CFE_ES_StartApplications(uint32 ResetType, const char *StartFilePath );

/*
** Internal function to parse/execute a line of the cFE application startup 'script'
*/
int32 CFE_ES_ParseFileEntry(const char **TokenList, uint32 NumTokens);

/*
** Internal function to create/start a new cFE app
** based on the parameters passed in
*/
int32 CFE_ES_AppCreate(CFE_ES_ResourceID_t *ApplicationIdPtr,
                       const char   *FileName,
                       const void   *EntryPointData,
                       const char   *AppName,
                       uint32  Priority,
                       uint32  StackSize,
                       uint32  ExceptionAction);
/*
** Internal function to load a a new cFE shared Library
*/
int32 CFE_ES_LoadLibrary(CFE_ES_ResourceID_t *LibraryIdPtr,
                       const char   *FileName,
                       const void   *EntryPointData,
                       const char   *LibName);

/*
** Get Application List
*/
int32 CFE_ES_AppGetList(uint32 AppIdArray[], uint32 ArraySize);

/*
** Dump All application properties to a file
**  Note: Do we want to specify a file here?
*/
int32 CFE_ES_AppDumpAllInfo(void);

/*
** Scan the Application Table for actions to take
*/
bool CFE_ES_RunAppTableScan(uint32 ElapsedTime, void *Arg);

/*
** Scan for new exceptions stored in the PSP
*/
bool CFE_ES_RunExceptionScan(uint32 ElapsedTime, void *Arg);

/*
** Check if ER log dump request is pending
*/
bool CFE_ES_RunERLogDump(uint32 ElapsedTime, void *Arg);

/*
** Perform the requested control action for an application
*/
void CFE_ES_ProcessControlRequest(CFE_ES_ResourceID_t AppId);

/*
** Clean up all app resources and delete it
*/
int32 CFE_ES_CleanUpApp(CFE_ES_ResourceID_t AppId);

/*
** Clean up all Task resources and detete the task
*/
int32 CFE_ES_CleanupTaskResources(CFE_ES_ResourceID_t TaskId);

/*
** Populate the cFE_ES_AppInfo structure with the data for an app
** This is an internal function for use in ES.
** The newer external API is : CFE_ES_GetAppInfo
*/
int32 CFE_ES_GetAppInfoInternal(CFE_ES_AppRecord_t *AppRecPtr, CFE_ES_AppInfo_t *AppInfoPtr );

/*
 * Populate the CFE_ES_TaskInfo_t structure with the data for a task
 * This is an internal function for use in ES.
 * (Equivalent pattern to CFE_ES_GetAppInfoInternal() but for tasks)
 */
int32 CFE_ES_GetTaskInfoInternal(CFE_ES_TaskRecord_t *TaskRecPtr, CFE_ES_TaskInfo_t *TaskInfoPtr );

#endif  /* _cfe_es_apps_ */
