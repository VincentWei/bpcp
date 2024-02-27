///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
** Copyright (C) 2003 ~ 2020 FMSoft (http://www.fmsoft.cn).
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#ifdef _MGRM_PROCESSES

static int nr_clients = 0;
static pid_t pid_dynamic = 0;
#ifdef _MGSCHEMA_COMPOSITING
static pid_t pid_welcome = 0;
static char* exe_cmd;
#endif

static pid_t exec_app (const char* file_name, const char* app_name)
{
    pid_t pid = 0;

    if ((pid = vfork ()) > 0) {
        _MG_PRINTF ("new child, pid: %d.\n", pid);
    }
    else if (pid == 0) {
        execl (file_name, app_name, NULL);
        perror ("execl");
        _exit (1);
    }
    else {
        perror ("vfork");
    }

    return pid;
}

static void on_new_del_client (int op, int cli)
{
    if (op == LCO_NEW_CLIENT) {
        nr_clients ++;
        _MG_PRINTF ("A new client: %d.\n", mgClients[cli].pid);
    }
    else if (op == LCO_DEL_CLIENT) {
        _MG_PRINTF ("A client left: %d.\n", mgClients[cli].pid);
        nr_clients --;
        if (nr_clients == 0) {
            _MG_PRINTF ("There is no any client.\n");
        }
        else if (nr_clients < 0) {
            _ERR_PRINTF ("Serious error: nr_clients less than zero.\n");
        }

#ifdef _MGSCHEMA_COMPOSITING
        if (pid_welcome == mgClients[cli].pid) {
            pid_welcome = 0;
            if (exe_cmd) {
                exec_app (exe_cmd, exe_cmd);
                free (exe_cmd);
                exe_cmd = NULL;
            }
        }
#endif
    }
    else
        _ERR_PRINTF ("Serious error: incorrect operations.\n");
}

static unsigned int old_tick_count;

static pid_t pid_scrnsaver = 0;

static inline void dump_key_messages (PMSG msg)
{
    if (msg->message == MSG_KEYDOWN || msg->message == MSG_KEYUP) {
        _WRN_PRINTF ("%s (%d) %s KS_REPEATED\n",
                (msg->message == MSG_KEYDOWN)?"MSG_KEYDOWN":"MSG_KEYUP",
                (int)msg->wParam,
                (msg->lParam & KS_REPEATED)?"with":"without");
    }
}

static HWND my_tooltip_win;
static void show_layer_messsage (const char* name)
{
    if (my_tooltip_win) {
        ResetToolTipWin (my_tooltip_win, 10, 10,
                "A new layer created: %s", name);
    }
    else {
        my_tooltip_win = CreateToolTipWin (HWND_DESKTOP, 10, 10, 3000,
                "A new layer created: %s", name);
    }
}

static MG_Layer* my_layers [4];
static void create_or_switch_layer (int slot, const char* name)
{
    if (my_layers[slot]) {
        ServerSetTopmostLayer (my_layers[slot]);
    }
    else {
        my_layers[slot] = ServerCreateLayer (name, 0, 0);
        if (my_layers[slot]) {
            show_layer_messsage (name);
        }
    }
}


static int my_event_hook (PMSG msg)
{
    old_tick_count = GetTickCount ();

    if (pid_scrnsaver) {
        kill (pid_scrnsaver, SIGINT);
        ShowCursor (TRUE);
        pid_scrnsaver = 0;
    }

    dump_key_messages(msg);

    if (msg->message == MSG_KEYDOWN) {
        switch (msg->wParam) {
        case SCANCODE_ESCAPE:
            if (nr_clients == 0) {
                if (my_tooltip_win) {
                    DestroyToolTipWin (my_tooltip_win);
                    my_tooltip_win = HWND_NULL;
                }
                SendNotifyMessage (HWND_DESKTOP, MSG_ENDSESSION, 0, 0);
            }
            else if (nr_clients == 1 && pid_dynamic) {
                kill (pid_dynamic, SIGINT);
                pid_dynamic = 0;
            }
            break;

        case SCANCODE_SPACE:
#ifdef _MGSCHEMA_COMPOSITING
            if (pid_welcome == 0 && pid_dynamic == 0)
                pid_dynamic = exec_app ("./wallpaper-dynamic", "wallpaper-dynamic");
#endif
           break;

        case SCANCODE_F1:
           exec_app ("./edit", "edit");
           break;
        case SCANCODE_F2:
           exec_app ("./menubutton", "menubutton");
           break;
        case SCANCODE_F3:
           exec_app ("./helloworld", "helloworld");
           break;
        case SCANCODE_F5:
           exec_app ("./combobox", "combobox");
           break;
        case SCANCODE_F4:
           exec_app ("./eventdumper", "eventdumper");
           break;

        case SCANCODE_F6:
           exec_app ("./docker", "docker");
           break;

        case SCANCODE_F7:
           exec_app ("./screenlock", "screenlock");
           break;

        case SCANCODE_F8: {
            MG_Layer* def_layer = ServerCreateLayer (NAME_DEF_LAYER, 0, 0);
            ServerSetTopmostLayer (def_layer);
            break;
        }

        case SCANCODE_F9:
            create_or_switch_layer (0, "F9");
            break;

        case SCANCODE_F10:
            create_or_switch_layer (1, "F10");
            break;

        case SCANCODE_F11:
            create_or_switch_layer (2, "F11");
            break;

        case SCANCODE_F12:
            create_or_switch_layer (3, "F12");
            break;
    }
    }

    return HOOK_GOON;
}

static void child_wait (int sig)
{
    int pid;
    int status;

    while ((pid = waitpid (-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED (status))
            _MG_PRINTF ("--pid=%d--status=%x--rc=%d---\n", pid, status, WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            _MG_PRINTF ("--pid=%d--signal=%d--\n", pid, WTERMSIG (status));
    }
}

#ifdef _MGSCHEMA_COMPOSITING
#include <mgeff/mgeff.h>

static void (*old_transit_to_layer)(CompositorCtxt*, MG_Layer*);
static CompositorOps* fallback_ops;

struct _my_ctxt {
    CompositorCtxt* ctxt;
    float last_percent;
    MG_Layer* layers [2];
};

static void animated_cb (MGEFF_ANIMATION handle, struct _my_ctxt *my_ctxt,
       int id, float *value)
{
    COMBPARAMS_FALLBACK cp = { FCM_HORIZONTAL | FCM_SCALE, 0.0f, 0.8f };
    cp.percent = *value;

    if (my_ctxt->last_percent != cp.percent) {
        fallback_ops->composite_layers (my_ctxt->ctxt, my_ctxt->layers, 2, &cp);
        my_ctxt->last_percent = cp.percent;
    }
}

static void my_transit_to_layer (CompositorCtxt* ctxt, MG_Layer* to_layer)
{
    MGEFF_ANIMATION handle;
    struct _my_ctxt my_ctxt = { ctxt, 0 };

    if ((time(NULL) % 2) == 0) {
        old_transit_to_layer (ctxt, to_layer);
        return;
    }

    handle = mGEffAnimationCreate ((void *)&my_ctxt, (void *)animated_cb, 0, MGEFF_FLOAT);

    if (handle) {
        float start_value, end_value;
        MG_Layer* layer = mgLayers;
        int idx_layer_0 = -1;
        int idx_layer_1 = -1;
        int i = 0, nr_layers = 0;

        while (layer) {
            if (layer == mgTopmostLayer)
                idx_layer_0 = i;
            if (layer == to_layer)
                idx_layer_1 = i;

            layer = layer->next;
            i++;
            nr_layers++;
        }

        if (idx_layer_0 == idx_layer_1 || idx_layer_0 < 0 || idx_layer_1 < 0) {
            _WRN_PRINTF("Bad layers to transit to (%d, %d).\n",
                    idx_layer_0, idx_layer_1);
            return;
        }

        idx_layer_0 = nr_layers - 1 - idx_layer_0;
        idx_layer_1 = nr_layers - 1 - idx_layer_1;

        if (idx_layer_1 > idx_layer_0) {
            /* to_layer is to the right of the topmost layer */
            my_ctxt.layers[0] = mgTopmostLayer;
            my_ctxt.layers[1] = to_layer;

            start_value = 95.0f;
            end_value = 5.0f;
        }
        else {
            /* to_layer is to the left of the topmost layer */
            my_ctxt.layers[0] = to_layer;
            my_ctxt.layers[1] = mgTopmostLayer;

            start_value = 5.0f;
            end_value = 95.0f;
        }

        mGEffAnimationSetStartValue (handle, &start_value);
        mGEffAnimationSetEndValue (handle, &end_value);
        mGEffAnimationSetDuration (handle, 200);
        mGEffAnimationSetProperty (handle, MGEFF_PROP_LOOPCOUNT, 1);
        mGEffAnimationSetCurve (handle, OutCubic);
        mGEffAnimationSyncRun (handle);
        mGEffAnimationDelete (handle);
    }
    else {
        old_transit_to_layer (ctxt, to_layer);
    }
}

static inline void override_fallback_compositor (void)
{
    fallback_ops = (CompositorOps*)ServerGetCompositorOps (COMPSOR_NAME_FALLBACK);
    if (fallback_ops) {
        old_transit_to_layer = fallback_ops->transit_to_layer;
        fallback_ops->transit_to_layer = my_transit_to_layer;
    }
    else {
        _ERR_PRINTF("Can not get operations of the fallback compositor.\n");
    }
}
#endif

int MiniGUIMain (int argc, const char* argv[])
{
    MSG msg;
    struct sigaction siga;

    siga.sa_handler = child_wait;
    siga.sa_flags  = 0;
    memset (&siga.sa_mask, 0, sizeof(sigset_t));
    sigaction (SIGCHLD, &siga, NULL);

    OnNewDelClient = on_new_del_client;

    if (!ServerStartup (0 , 0 , 0)) {
        _ERR_PRINTF("Can not start the server of MiniGUI-Processes: mginit.\n");
        return 1;
    }

#ifdef _MGSCHEMA_COMPOSITING
    if (argc > 1 && strcasecmp (argv[1], "auto") == 0) {
        pid_dynamic = exec_app ("./wallpaper-dynamic", "wallpaper-dynamic");
        exec_app ("./static", "static");
        exec_app ("./edit", "edit");
        exec_app ("./eventdumper", "eventdumper");
    }
    else if (argc > 1 && strcasecmp (argv[1], "none") == 0) {
        // start null-loop
        // exec_app ("./null-loop", "null-loop");
    }
    else {
        pid_welcome = exec_app ("./wallpaper-welcome", "wallpaper-welcome");
        if (argc > 1)
            exe_cmd = strdup (argv[1]);
    }

    mGEffInit();
    override_fallback_compositor ();
#endif

    SetServerEventHook (my_event_hook);

    old_tick_count = GetTickCount ();

    while (GetMessage (&msg, HWND_DESKTOP)) {
        DispatchMessage (&msg);
    }

    if (my_tooltip_win)
        DestroyToolTipWin (my_tooltip_win);

#ifdef _MGSCHEMA_COMPOSITING
    mGEffDeinit();
#endif

    return 0;
}

#else   /* defined _MGRM_PROCESSES */

int main (int argc, const char* argv[])
{
    _WRN_PRINTF ("This test program is the server for MiniGUI-Prcesses "
           "runtime mode. But your MiniGUI was not configured as "
           "MiniGUI-Processes\n");
    return 0;
}

#endif  /* not defined _MGRM_PROCESSES */

