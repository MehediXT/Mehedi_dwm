/*
bread's dwm build

patches applied (incl. some slight changes):

dwm-bartoggle-keybinds-6.4.diff ----- toggle pieces of the bar
dwm-bulkill-20231029-9f88553.diff ----- kill all except current; kill all in tag
dwm-colorbar-6.3.diff ----- fine control over statusbar colors
dwm-fixmultimon-6.4.diff ----- pretty much a bug fix
dwm-focusmaster-return-6.2.diff ----- switch to master from anywhere in the stack
dwm-focusmonmouse-6.2.diff ----- move mouse when you switch monitors via keybind
dwm-hide_vacant_tags-6.4.diff ----- hide tags with no windows
dwm-restartsig-20180523-6.2.diff ----- refresh dwm after recompile, without quitting
dwm-spawntag-6.2.diff ----- spawn an application when tag is middle-clicked
dwm-stacker-6.2.diff ----- more utilities to manage the stack
dwm-statuscmd-20210405-67d76bd.diff ----- execute cmds when statusbar is clicked (used for dwmblocks)
dwm-sticky-6.5.diff ----- make a window follow from tag to tag
dwm-swallow-6.3.diff ----- terminal swallows launched programs
dwm-vanitygaps-6.2.diff ----- gaps are functional: your eye is less inclined to drift while working.
dwm-xrdb-6.4.diff - xresource database colors
*/

/* appearance */
static unsigned int borderpx        = 1;        /* border pixel of windows */
static unsigned int snap            = 32;       /* snap pixel */
static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 20;       /* vert inner gap between windows */
static const unsigned int gappoh    = 20;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 30;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static int showbar                  = 1;        /* 0 means no bar */
static const int showtitle          = 1;        /* show the active app or window title */
static const int showtags           = 1;        /* 0 means no tags */
static const int showlayout         = 1;        /* 0 means no layout indicator */
static const int showstatus         = 1;        /* 0 means no status bar */
static const int showfloating       = 0;        /* 0 means no floating indicator */
static int topbar                   = 1;        /* 0 means bottom bar */
static const unsigned int baralpha  = 0x99;     /* ~60% opaque; lower = more transparent (0x00-0xff) */
static const unsigned int borderalpha = OPAQUE; /* keep client borders solid */
static const unsigned int alphas[]  = { OPAQUE, baralpha, borderalpha };
static char dmenufont[]             = "Hack:size=10"; /* match the bar font */
static const char *fonts[]          = { "Hack:size=10", "AtkynsonMono Nerd Font Mono:size=10" };
static char barbgcolor[]            = "#000000";
static char barfgcolor[]            = "#a89a80"; /* muted sand text */
static char baractivecolor[]        = "#e8d4a8"; /* active tag */
static char titlebgcolor[]          = "#a8763a"; /* orange title block */
static char titlefgcolor[]          = "#1a1611";

/* default colors used if xrdb is not loaded */
/* Black backgrounds keep translucent ARGB pixels premultiplied. */
static char normbgcolor[]           = "#000000";
static char normbordercolor[]       = "#6b5a45"; /* unfocused: visible muted brown */
static char normfgcolor[]           = "#d8dee9";
static char selfgcolor[]            = "#eceff4";
static char selbordercolor[]        = "#f0a040"; /* focused: bright orange */
static char selbgcolor[]            = "#b48ead";

static char *colors[][3] = {
       /*               fg           bg           border   */
		[SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
		[SchemeSel]  = { selbgcolor,  selfgcolor,  selbordercolor  },
		/* for bar --> {text, background, null} */
		[SchemeStatus]  = { barfgcolor, barbgcolor, barbgcolor },
		[SchemeTagsSel]  = { baractivecolor, barbgcolor, barbgcolor },
		[SchemeTagsNorm]  = { barfgcolor, barbgcolor, barbgcolor },
		[SchemeInfoSel]  = { titlefgcolor, titlebgcolor, titlebgcolor },
		[SchemeInfoNorm]  = { barfgcolor, barbgcolor, barbgcolor },
};


/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
/* static const char *tags[] = { "󰎤", "󰎧", "󰎪", "󰎭", "󰎱", "󰎳", "󰎶", "󰎹", "󰎼" }; */


static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "St",      NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ "fzfmenu", NULL,     "fzf", 	0,         1,          1,           1,        -1 }, /* xev */
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
	/* browsers must never be swallowed by the terminal they were opened from */
	{ "Brave-browser",        NULL, NULL,  0,         0,          0,           1,        -1 },
	{ "firefox",              NULL, NULL,  0,         0,          0,           1,        -1 },
	/* small utility windows float instead of wrecking the tiling layout
	 * (later rules win, so keep these below the browser rules) */
	{ "Pavucontrol",          NULL, NULL,  0,         1,          0,           0,        -1 },
	{ "pavucontrol",          NULL, NULL,  0,         1,          0,           0,        -1 },
	{ "Nm-connection-editor", NULL, NULL,  0,         1,          0,           0,        -1 },
	{ "Blueman-manager",      NULL, NULL,  0,         1,          0,           0,        -1 },
	{ "Gpick",                NULL, NULL,  0,         1,          0,           0,        -1 },
	{ NULL,      NULL,     "Picture-in-Picture", 0,   1,          0,           1,        -1 },
	{ NULL,      NULL,     "Picture in picture", 0,   1,          0,           1,        -1 },
};

#include "vanitygaps.c"


/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 0: tiled terminals fill their slot exactly (1 leaves uneven gaps) */
static const int lockfullscreen = 0; /* 1 will force focus on the fullscreen window */


static const Layout layouts[] = { /* alt glyphs: 󱡗 󱏋 */
	/* symbol     arrange function */
	{ "󰓒",      tile },    /* first entry is default */
	{ "󰇥",      NULL },    /* no layout function means floating behavior */
	{ "",      monocle },
	{ "󰫣",      spiral },
	{ "󰫥",      dwindle },
};

/* Mod+Shift+m: monocle on, press again to go back to the previous layout */
static void
togglemonocle(const Arg *arg)
{
	if (selmon->lt[selmon->sellt]->arrange == monocle)
		setlayout(&(Arg){0});           /* swap back to the previous layout */
	else
		setlayout(&(Arg){.v = &layouts[2]});
}

/* static const char *gromittoggle[] = { */
/* 	"gromit-mpx", "--toggle", NULL */
/* }; */
/* static const char *gromitclear[] = { */
/* 	"gromit-mpx", "--clear", NULL */
/* }; */

/* key definitions */
#include <X11/XF86keysym.h> /* multimedia keys (volume, play/pause, ...) */
#define MODKEY Mod4Mask // windows key
static const int slideduration = 250; /* Super+1..9 slide animation in ms, 0 = off */
static const int slidefps = 100;      /* match the monitor refresh rate (HDMI-0 runs at 100 Hz) */
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      slideview,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \
	/*{ MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \
	{ MOD, XK_q,     ACTION##stack, {.i = 0 } }, \
	{ MOD, XK_a,     ACTION##stack, {.i = 1 } }, \
	{ MOD, XK_z,     ACTION##stack, {.i = 2 } }, \
	{ MOD, XK_x,     ACTION##stack, {.i = -1 } }, */

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* helper for launching gtk application */
#define GTKCMD(cmd) { .v = (const char*[]){ "/usr/bin/gtk-launch", cmd, NULL } }

#define BROWSER "dwm-browser"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *volupcmd[]   = { "dwm-volume", "up",   NULL };
static const char *voldowncmd[] = { "dwm-volume", "down", NULL };
static const char *volmutecmd[] = { "dwm-volume", "mute", NULL };
static const char *micmutecmd[] = { "dwm-volume", "mic-mute", NULL };
static const char *keyhelpcmd[] = { "dwm-keybinds", NULL };
static const char *ctrlcentercmd[] = { "dwm-control-center", NULL };


static const Arg tagexec[] = { /* spawn application when tag is middle-clicked */
	{ .v = termcmd }, /* 1 */
	{ .v = termcmd }, /* 2 */
	{ .v = termcmd }, /* 3 */
	{ .v = termcmd }, /* 4 */
	{ .v = termcmd }, /* 5 */
	{ .v = termcmd }, /* 6 */
	{ .v = termcmd }, /* 7 */
	{ .v = termcmd }, /* 8 */
	{ .v = termcmd }, /* 9 */
    /* GTKCMD("gtkapplication") */
};

static const Key keys[] = {
	/* modifier                     key        function        argument */
	/* { MODKEY,                       XK_F8,     spawn,		   {.v = gromittoggle} }, */
	/* { MODKEY|ShiftMask,             XK_F8,     spawn,		   {.v = gromitclear} }, */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	/* transparency of the focused window (like Mod+-/= for gaps):
	 * Mod+Alt+-  more transparent, Mod+Alt+=  more solid, Mod+Alt+0  reset
	 * st keeps its own background-only alpha (also Alt+a/s/d inside st) */
	{ MODKEY|Mod1Mask,              XK_minus,  spawn,          SHCMD("dwm-opacity down") },
	{ MODKEY|Mod1Mask,              XK_equal,  spawn,          SHCMD("dwm-opacity up") },
	{ MODKEY|Mod1Mask,              XK_0,      spawn,          SHCMD("dwm-opacity reset") },
	{ MODKEY|ShiftMask,             XK_c,      spawn,          {.v = ctrlcentercmd } },
	{ MODKEY,                       XK_slash,  spawn,          {.v = keyhelpcmd } }, /* shortcut cheat sheet */
	{ MODKEY,                       XK_F2,     spawn,          {.v = keyhelpcmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_b,      togglebar,      {0} },
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	{ MODKEY|ShiftMask,             XK_i,      incnmaster,     {.i = +1 } }, /* increase # of master windows */
	{ MODKEY|ControlMask,           XK_i,      incnmaster,     {.i = -1 } }, /* decrease # of master windows */
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },


	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },


	{ MODKEY|ControlMask|ShiftMask, XK_q,		quit,			{1} },  /* refresh dwm (restartsig) */
	{ MODKEY|ShiftMask,				XK_BackSpace, quit,			{0} }, /* quit dwm */
	{ MODKEY,						XK_q,		killclient,     {0} }, /* quit window */
	{ MODKEY|ShiftMask,				XK_q,		killclient,     {.ui = 1} },  /* kill all windows besides current */
    /* { MODKEY|ShiftMask|ControlMask,  XK_q,	killclient,     {.ui = 2} }, */
	{ MODKEY|ControlMask,			XK_backslash, xrdb,			{.v = NULL } }, /* refresh xrdb; run this when setting new colors */


	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,						XK_f,	   togglefullscreen, {0} }, /* toggle focused window fullscreen */
	{ MODKEY|ShiftMask,				XK_m,      togglemonocle,  {0} }, /* monocle on/off */
	{ MODKEY|ShiftMask,				XK_f,      setlayout,      {.v = &layouts[1]} }, /* floating */
	{ MODKEY,						XK_s,      setlayout,      {.v = &layouts[3]} }, /* spiral */
	{ MODKEY|ShiftMask,				XK_t,      setlayout,      {.v = &layouts[4]} }, /* dwindle */
	{ MODKEY|ControlMask,			XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,				XK_space,  togglefloating, {0} },
	{ MODKEY,						XK_space,  zoom,           {0} },
	{ MODKEY,						XK_grave,  spawn,          {.v = (const char*[]){ "kblayout", "toggle", NULL } } }, /* English <-> Arabic */
	{ MODKEY|ControlMask,           XK_Return, focusmaster,    {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_s,      togglesticky,   {0} },


/* multi-monitor control */
	{ MODKEY|ControlMask,		XK_bracketright,   focusmon,       {.i = -1 } },
	{ MODKEY|ShiftMask,		XK_bracketright,   tagmon,         {.i = -1 } },
	{ MODKEY|ControlMask,		XK_bracketleft,    focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,		XK_bracketleft,    tagmon,         {.i = +1 } },
	/* standard dwm monitor keys */
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },

/* gaps control */
	{ MODKEY,					XK_minus,  incrgaps,       {.i = -3 } }, /* all */
	{ MODKEY,					XK_equal,  incrgaps,       {.i = +3 } },
	{ MODKEY|Mod1Mask,              XK_i,      incrigaps,      {.i = +1 } }, /* inner */
	{ MODKEY|Mod1Mask|ShiftMask,    XK_i,      incrigaps,      {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_o,      incrogaps,      {.i = +1 } }, /* outer */
	{ MODKEY|Mod1Mask|ShiftMask,    XK_o,      incrogaps,      {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_6,      incrihgaps,     {.i = +1 } }, /* inner horiz */
	{ MODKEY|Mod1Mask|ShiftMask,    XK_6,      incrihgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_7,      incrivgaps,     {.i = +1 } }, /* inner vert */
	{ MODKEY|Mod1Mask|ShiftMask,    XK_7,      incrivgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_8,      incrohgaps,     {.i = +1 } }, /* outer horiz */
	{ MODKEY|Mod1Mask|ShiftMask,    XK_8,      incrohgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_9,      incrovgaps,     {.i = +1 } }, /* outer vert */
	{ MODKEY|Mod1Mask|ShiftMask,    XK_9,      incrovgaps,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_equal,  togglegaps,     {0} },
	{ MODKEY|ShiftMask,             XK_minus,  defaultgaps,    {0} },


/* tag keys */
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)


/* toggle statusbar pieces individually */
	{ MODKEY|ControlMask,			XK_t,      togglebartitle, {0} },
	{ MODKEY|ControlMask,			XK_s,      togglebarstatus,{0} },
	{ MODKEY|ControlMask|ShiftMask, XK_t,     togglebartags,  {0} },
    { MODKEY|ControlMask,			XK_e,      togglebarcolor, {0} }, /* swaps fg/bg for tag+win */
	{ MODKEY|ControlMask,			XK_r,      togglebarlt,    {0} },
	{ MODKEY|ControlMask,			XK_f,      togglebarfloat, {0} },


/* application bindings */
	{ MODKEY,			XK_m,          spawn,      {.v = (const char*[]){ "vlc", NULL } } },
	{ MODKEY,			XK_w,          spawn,      {.v = (const char*[]){ BROWSER, "--new-window", NULL } } },
	{ MODKEY,			XK_e,          spawn,      {.v = (const char*[]){ "thunar", NULL } } },
	{ MODKEY,			XK_n,          spawn,      {.v = (const char*[]){ "st", "-e", "nvim", NULL } } },
	{ MODKEY|ShiftMask,	XK_h,          spawn,      {.v = (const char*[]){ "st", "-e", "htop", NULL } } },
	{ MODKEY,			XK_p,          spawn,      {.v = (const char*[]){ "gwenview", NULL } } },


/* Save a screenshot directly; Spectacle's GUI exits without opening in dwm. */
	{ 0,                    XK_F6,     spawn,       {.v = (const char*[]){ "dwm-screenshot", "region", NULL } } },
	{ 0,					XK_Print,  spawn,       {.v = (const char*[]){ "dwm-screenshot", "full", NULL } } },
	{ MODKEY,				XK_F1,     spawn,       {.v = (const char*[]){ "dwm-screenshot", "full", NULL } } },
	{ MODKEY|ShiftMask,		XK_s,      spawn,       {.v = (const char*[]){ "dwm-screenshot", "region", NULL } } },
	{ ControlMask|Mod1Mask,		XK_s,      spawn,       {.v = (const char*[]){ "dwm-screenshot", "region", NULL } } },
	{ ControlMask|Mod1Mask|ShiftMask,	XK_s,      spawn,       {.v = (const char*[]){ "dwm-screenshot", "full", NULL } } },


/* other bindings */
	{ MODKEY|ShiftMask,			XK_l,      spawn,       SHCMD("\"$HOME/.local/bin/dwm-lock\"") },
	{ MODKEY|ShiftMask,			XK_p,      spawn,       {.v = (const char*[]){ "dwm-power-menu", NULL } } },
	{ MODKEY,				XK_F12,    spawn,       SHCMD("playerctl next") },
	{ MODKEY,				XK_F11,    spawn,       SHCMD("playerctl play-pause") },
	{ MODKEY,				XK_F10,    spawn,       SHCMD("playerctl previous") },
	/* dwm-volume caps at 100% and refreshes the bar instantly */
	{ MODKEY,				XK_F6,     spawn,       {.v = volupcmd } },
	{ MODKEY,				XK_F5,     spawn,       {.v = voldowncmd } },
	{ MODKEY,				XK_F4,     spawn,       {.v = volmutecmd } },
	{ MODKEY,				XK_bracketright, spawn,  {.v = volupcmd } },
	{ MODKEY,				XK_bracketleft,  spawn,  {.v = voldowncmd } },
	{ MODKEY,				XK_backslash,    spawn,  {.v = volmutecmd } },

/* keyboard multimedia keys */
	{ 0,			XF86XK_AudioRaiseVolume,   spawn,       {.v = volupcmd } },
	{ 0,			XF86XK_AudioLowerVolume,   spawn,       {.v = voldowncmd } },
	{ 0,			XF86XK_AudioMute,          spawn,       {.v = volmutecmd } },
	{ 0,			XF86XK_AudioMicMute,       spawn,       {.v = micmutecmd } },
	{ 0,			XF86XK_AudioPlay,          spawn,       SHCMD("playerctl play-pause") },
	{ 0,			XF86XK_AudioPause,         spawn,       SHCMD("playerctl play-pause") },
	{ 0,			XF86XK_AudioNext,          spawn,       SHCMD("playerctl next") },
	{ 0,			XF86XK_AudioPrev,          spawn,       SHCMD("playerctl previous") },
	{ 0,			XF86XK_AudioStop,          spawn,       SHCMD("playerctl stop") },
	{ 0,			XF86XK_Calculator,         spawn,       {.v = (const char*[]){ "st", "-e", "python3", "-q", NULL } } },
};


/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask           button          function        argument */
	{ ClkWinTitle,          0,			Button2,	zoom,           {0} },
	/* dwm-status.sh is one plain string, so the whole status area gets these actions */
	{ ClkStatusText,        0,			Button1,	spawn,          {.v = ctrlcentercmd } },
	{ ClkStatusText,        0,			Button2,	spawn,          {.v = volmutecmd } },
	{ ClkStatusText,        0,			Button3,	spawn,          {.v = (const char*[]){ "pavucontrol", NULL } } },
	{ ClkStatusText,        0,			Button4,	spawn,          {.v = volupcmd } },
	{ ClkStatusText,        0,			Button5,	spawn,          {.v = voldowncmd } },
	{ ClkStatusText,        ShiftMask,	Button3,	spawn,          SHCMD("st -e nvim \"$(readlink -f \"$(command -v dwm-status)\")\"") },
	{ ClkLtSymbol,          0,			Button1,	setlayout,      {0} }, /* previous layout */
	{ ClkLtSymbol,          0,			Button3,	setlayout,      {.v = &layouts[2]} }, /* monocle */
	{ ClkClientWin,			MODKEY,		Button1,	movemouse,      {0} }, /* left click */
	{ ClkClientWin,			MODKEY,		Button2,	defaultgaps,    {0} }, /* middle click */
	{ ClkClientWin,			MODKEY,		Button3,	resizemouse,    {0} }, /* right click */
	{ ClkClientWin,			MODKEY,		Button4,	incrgaps,       {.i = +1} }, /* scroll up */
	{ ClkClientWin,			MODKEY,		Button5,	incrgaps,       {.i = -1} }, /* scroll down */
	{ ClkTagBar,			0,			Button1,	view,           {0} },
	{ ClkTagBar,			0,			Button2,	spawntag,       {0} }, /* open tagexec[] app on that tag */
	{ ClkTagBar,			0,			Button3,	toggleview,     {0} },
	{ ClkTagBar,			MODKEY,		Button1,	tag,            {0} },
	{ ClkTagBar,			MODKEY,		Button3,	toggletag,      {0} },
	{ ClkRootWin,			0,			Button2,	togglebar,      {0} }, /* hide bar */
};
