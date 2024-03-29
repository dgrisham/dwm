/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
/*  Display modes of the tab bar: never shown, always shown, shown only in  */
/*  monocle mode in the presence of several windows.                        */
/*  Modes after showtab_nmodes are disabled.                                */
enum showtab_modes { showtab_never, showtab_auto, showtab_nmodes, showtab_always };
static const int showtab			= showtab_auto;        /* Default tab bar show mode */
static const int toptab				= False;               /* False means bottom tab bar */
static const char *fonts[]          = { "Inconsolata Nerd Font Mono:pixelsize=16:antialias=true" };
static const char col_white[]       = "#ffffff";
static const char col_black[]       = "#000000";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#00cccc";
static const char col_yellow[]      = "#b1ec40";
static const char *colors[][3]      = {
	/*                  fg         bg         border   */
	[SchemeNorm]    = { col_white, col_black, col_black },
	[SchemeSel]     = { col_black, col_gray4, col_white },
	[SchemeSelWin]  = { col_white, col_black, col_white },
};
static const int focusonwheel       = 0;

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	// { "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "<T,T>",    tiletab },
	{ "<T,=>",    tiletableft },
};

static const char* Logfile = "/home/grish/.cache/dwm/debug.log";
// static const char* Logfile = NULL;

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]       = { "dmenu_run", "-m", dmenumon, NULL };
static const char *bgcmd[]          = { "fehbg", NULL };
static const char *termcmd[]        = { "st", NULL };
// static const char *browsercmd[]     = { "tabbed", "-c", "-r2", "surf", "-e", "x", "-SN", NULL };
static const char *browsercmd[]  = { "brave", NULL };
static const char *musiccmd[]    = { "bbmp", NULL };

/* volume control */
static const char *upvol[]   = { "/usr/bin/pactl", "set-sink-volume", "0", "+5%",     NULL };
static const char *downvol[] = { "/usr/bin/pactl", "set-sink-volume", "0", "-5%",     NULL };
static const char *mutevol[] = { "/usr/bin/pactl", "set-sink-mute",   "0", "toggle",  NULL };

/* brightness control */
static const char *light_up[]   = {"/usr/bin/sudo", "light", "-A", "5", NULL};
static const char *light_down[] = {"/usr/bin/sudo", "light", "-U", "5", NULL};

static Key keys[] = {
    /* Add to keys[] array. With 0 as modifier, you are able to use the keys directly. */
	/* modifier                     key        function           argument */
	{ 0,                       		XF86XK_AudioLowerVolume, spawn, {.v = downvol } },
	{ 0,                       		XF86XK_AudioMute,        spawn, {.v = mutevol } },
	{ 0,                       		XF86XK_AudioRaiseVolume, spawn, {.v = upvol   } },
    { 0,							XF86XK_MonBrightnessUp,	 spawn,	{.v = light_up} },
	{ 0,							XF86XK_MonBrightnessDown,spawn,	{.v = light_down} },
	{ MODKEY,                       XK_p,      spawn,               {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,               {.v = termcmd } },
	{ MODKEY,                       XK_b,      spawn,               {.v = browsercmd } },
	{ MODKEY|ShiftMask,             XK_c,      spawn,               {.v = bgcmd } },
	{ MODKEY|ShiftMask,             XK_b,      spawn,               SHCMD("~/bin/bookmark.sh") },
	{ MODKEY,                       XK_m,      spawn,               {.v = musiccmd } },
	{ MODKEY,                       XK_s,      togglebar,           {0} },
	{ MODKEY,                       XK_t,      tabmode,             {-1} },
	{ MODKEY|ShiftMask,             XK_t,      spawn,               SHCMD("~/bin/add_otp.sh") },
	{ MODKEY,                       XK_j,      focusstack,          {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,          {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,          {.i = +1 } },
	// { MODKEY|ShiftMask,             XK_i,      spawn,             SHCMD("id=$(xinput list | grep -oP '\\KTranslated.*?id=\\K(\\d+)') && link=$(~/bin/get_bookmark.sh) && { xinput set-int-prop $id 'Device Enabled' 8 0; xdotool type \"$link\"; xinput set-int-prop $id 'Device Enabled' 8 1; }")},
	{ MODKEY|ShiftMask,             XK_i,      spawn,               SHCMD("link=$(~/bin/get_bookmark.sh) && xdotool type --clearmodifiers \"$link\"")},
	{ MODKEY|ShiftMask,             XK_p,      spawn,               SHCMD("~/bin/passmenu.sh")},
	{ MODKEY,                       XK_d,      incnmaster,          {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,            {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,            {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_h,      setcfact,       	    {.f = +0.25} },
	{ MODKEY|ShiftMask,             XK_l,      setcfact,       	    {.f = -0.25} },
	// { MODKEY|ShiftMask,             XK_o,      spawn,             SHCMD("[ $(xdotool getactivewindow getwindowclassname) = 'Brave-browser' ] || exit 0; link=$(~/bin/get_bookmark.sh) && { xdotool key Escape ctrl+l && xdotool type \"$link\" && xdotool key ENTER ; }")},
	// { MODKEY|ShiftMask,             XK_m,      spawn,             SHCMD("[ $(xdotool getactivewindow getwindowclassname) = 'Brave-browser' ] || exit 0; link=$(~/bin/get_brave_history.sh) && { xdotool key Escape ctrl+l && xdotool type \"$link\" && xdotool key ENTER ; }") },
	{ MODKEY|ShiftMask,             XK_o,      spawn,               SHCMD("[ $(xdotool getactivewindow getwindowclassname) = 'Brave-browser' ] || exit 0; id=$(xinput list | grep -oP '\\KTranslated.*?id=\\K(\\d+)') && link=$(~/bin/get_bookmark.sh) && { xinput set-int-prop $id 'Device Enabled' 8 0; xdotool key Escape ctrl+l && xdotool type \"$link\" && xdotool key ENTER; xinput set-int-prop $id 'Device Enabled' 8 1; }")},
	{ MODKEY|ShiftMask,             XK_m,      spawn,               SHCMD("[ $(xdotool getactivewindow getwindowclassname) = 'Brave-browser' ] || exit 0; id=$(xinput list | grep -oP '\\KTranslated.*?id=\\K(\\d+)') && link=$(~/bin/get_brave_history.sh) && { xinput set-int-prop $id 'Device Enabled' 8 0; xdotool key Escape ctrl+l && xdotool type \"$link\" && xdotool key ENTER; xinput set-int-prop $id 'Device Enabled' 8 1; }") },
	// { MODKEY|ShiftMask,             XK_o,      setcfact,       	  {.f =  0.00} },
	{ MODKEY|ShiftMask,             XK_f,      togglefullscr,       {0} },
	{ MODKEY,                       XK_Return, zoom,                {0} },
	{ MODKEY,                       XK_Tab,    view,                {0} },
	{ MODKEY,                       XK_q,      killclient,          {0} },
	{ MODKEY,                       XK_F1,     setlayout,           {.v = &layouts[0]} },
	{ MODKEY,                       XK_F2,     setlayout,           {.v = &layouts[1]} },
	{ MODKEY,                       XK_F3,     setlayout,           {.v = &layouts[2]} },
	{ MODKEY,                       XK_F4,     setlayout,           {.v = &layouts[3]} },
	{ MODKEY,                       XK_F5,     setlayout,           {.v = &layouts[4]} },
	// { MODKEY|ShiftMask,             XK_m,      monocle_togglebar, {0} },
	{ MODKEY,                       XK_space,  setlayout,           {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating,      {0} },
	{ MODKEY,                       XK_0,      view,                {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,                 {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,            {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,            {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,              {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,              {.i = +1 } },
	TAGKEYS(                        XK_1,                           0)
	TAGKEYS(                        XK_2,                           1)
	TAGKEYS(                        XK_3,                           2)
	TAGKEYS(                        XK_4,                           3)
	TAGKEYS(                        XK_5,                           4)
	TAGKEYS(                        XK_6,                           5)
	TAGKEYS(                        XK_7,                           6)
	TAGKEYS(                        XK_8,                           7)
	TAGKEYS(                        XK_9,                           8)
	{ MODKEY|ShiftMask,             XK_q,      quit,                {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkTabBar,            0,              Button1,        focuswin,       {0} },
};

