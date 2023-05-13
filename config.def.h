/* See LICENSE file for copyright and license details. */
/* Constants */
#define TERM "st"
#define TERMCLASS "St"
#define BROWSER "librewolf"

/* appearance */
static unsigned int borderpx  = 3;        /* border pixel of windows */
static unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 30;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
static char font[]            = "monospace:size=10";
static char dmenufont[]       = "monospace:size=10";
static const char *fonts[]          = { font };
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#770000"; /* red border color */
static char selbgcolor[]            = "#005577";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },

};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {TERM, "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {TERM, "-n", "spcalc", "-f", "monospace:size=16", "-g", "50x20", "-e", "bc", "-lq", NULL };
/*const char *spcmd3[] = {"passmenu", NULL }; */
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",    spcmd1},
	{"spcalc",    spcmd2},
	/* {"spassmenu",   spcmd3}, */
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",    NULL,     NULL,           1 << 8,    0,          0,           0,        -1 },
	{ TERMCLASS, NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
	{ TERMCLASS, "spterm",	NULL,   	   SPTAG(0),  1,		  1,	       0,        -1 },
	{ TERMCLASS, "spcalc",	NULL,		   SPTAG(1),  1,		  1,	       0,        -1 },
    /*{ TERMCLASS, "passmenu",	NULL,		SPTAG(2), 0,		  1,	       0,        -1 }, */
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
#define FORCE_VSPLIT 1 /*nrogrid layout: force 2 clients to always split vertically */

#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
    { "TTT",      bstack },  /* bottomstack */

    { "[@]",      spiral }, /* fibonacci - spiral */
    { "[\\]",     dwindle }, /* fibonacci - dwindle */

    { "[D]",      deck },   /* deck layout */
    { "[M]",      monocle },

    { "|M|",      centeredmaster }, /* centered master */
    { ">M>",      centeredfloatingmaster }, /* centered master floating */

    { "><>",      NULL },    /* no layout function means floating behavior */
	{ "===",      bstackhoriz }, /* bottomstack horizontal */

	{ "HHH",      grid }, 
    { "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \
    { MOD, XK_v,     ACTION##stack, {.i = 0 } }, \
	/* { MOD, XK_grave, ACTION##stack, {.i = PREVSEL } },i \ */
	/* { MOD, XK_a,     ACTION##stack, {.i = 1 } },i \ */
	/* { MOD, XK_z,     ACTION##stack, {.i = 2 } },i \ */
	/* { MOD, XK_x,     ACTION##stack, {.i = -1 } }, \ */

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "st", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "font",               STRING,  &font },
		{ "dmenufont",          STRING,  &dmenufont },
		{ "normbgcolor",        STRING,  &normbgcolor },
		{ "normbordercolor",    STRING,  &normbordercolor },
		{ "normfgcolor",        STRING,  &normfgcolor },
		{ "selbgcolor",         STRING,  &selbgcolor },
		{ "selbordercolor",     STRING,  &selbordercolor },
		{ "selfgcolor",         STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          		INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	 	FLOAT,   &mfact },
};
#include "shiftview.c"

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	{ MODKEY,                       XK_g,      shiftview,      {.i = 1 } },
	{ MODKEY|ShiftMask,             XK_g,      shiftview,      {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_space,  zoom,           {0} },
	{ MODKEY,                       XK_x,      incrgaps,       {.i = +1 } }, /* increase gaps */
	{ MODKEY,                       XK_z,      incrgaps,       {.i = -1 } }, /* decrease gaps */
/*	{ MODKEY|Mod4Mask,              XK_i,      incrigaps,      {.i = +1 } }, */
/*	{ MODKEY|Mod4Mask|ShiftMask,    XK_i,      incrigaps,      {.i = -1 } }, */
/*	{ MODKEY|Mod4Mask,              XK_o,      incrogaps,      {.i = +1 } }, */
/*	{ MODKEY|Mod4Mask|ShiftMask,    XK_o,      incrogaps,      {.i = -1 } }, */
/*	{ MODKEY|Mod4Mask,              XK_6,      incrihgaps,     {.i = +1 } }, */
/*	{ MODKEY|Mod4Mask|ShiftMask,    XK_6,      incrihgaps,     {.i = -1 } }, */
/*	{ MODKEY|Mod4Mask,              XK_7,      incrivgaps,     {.i = +1 } }, */
/*	{ MODKEY|Mod4Mask|ShiftMask,    XK_7,      incrivgaps,     {.i = -1 } }, */
/*	{ MODKEY|Mod4Mask,              XK_8,      incrohgaps,     {.i = +1 } }, */
/*	{ MODKEY|Mod4Mask|ShiftMask,    XK_8,      incrohgaps,     {.i = -1 } }, */
/*	{ MODKEY|Mod4Mask,              XK_9,      incrovgaps,     {.i = +1 } }, */
/*	{ MODKEY|Mod4Mask|ShiftMask,    XK_9,      incrovgaps,     {.i = -1 } }, */
	{ MODKEY,                       XK_a,      togglegaps,     {0} }, /* toggle gaps */
	{ MODKEY|ShiftMask,             XK_a,      defaultgaps,    {0} }, /* default gaps */
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
    { MODKEY,                       XK_s,      togglesticky,   {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} }, /* tiling */
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[1]} }, /* bstack */
	{ MODKEY,                       XK_y,      setlayout,      {.v = &layouts[2]} }, /* spiral */
	{ MODKEY|ShiftMask,             XK_y,      setlayout,      {.v = &layouts[3]} }, /* dwindle */
	{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[4]} }, /* deck */
    { MODKEY|ShiftMask,             XK_u,      setlayout,      {.v = &layouts[5]} }, /* monocle */
	{ MODKEY,                       XK_i,      setlayout,      {.v = &layouts[6]} }, /* centered master */
	{ MODKEY|ShiftMask,             XK_i,      setlayout,      {.v = &layouts[7]} }, /* centered floating master */
    { MODKEY|ShiftMask,             XK_f,      setlayout,      {.v = &layouts[8]} }, /* floating */
	{ MODKEY|ShiftMask,             XK_j,      setlayout,      {.v = &layouts[9]} }, /* bstack horizontal */
    /*{ MODKEY,                       XK_i,      setlayout,      {.v = &layouts[10]} },*/ /* grid */
	/*{ MODKEY|ShiftMask,             XK_i,      setlayout,      {.v = &layouts[11]} },*/ /* nrowgrid */
    /*{ MODKEY,                       XK_j,      setlayout,      {.v = &layouts[12]} },*/ /* horizontal grid */
	/*{ MODKEY|ShiftMask,             XK_j,      setlayout,      {.v = &layouts[13]} },*/ /* gapless grid */
    { MODKEY,                       XK_o,      incnmaster,     {.i = +1 } },
    { MODKEY|ShiftMask,             XK_o,      incnmaster,     {.i = -1 } },
	/*{ MODKEY,                       XK_space,  setlayout,      {0} }, */
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_f,      togglefullscr,  {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,    			XK_Return, togglescratch,  {.ui = 0 } },
	{ MODKEY,            			XK_apostrophe, togglescratch,  {.ui = 1 } },
	/*{ MODKEY,            			XK_x,	   togglescratch,  {.ui = 2 } }, */
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
    { MODKEY,                       XK_w,      spawn,          {.v = (const char*[]){ BROWSER, NULL } } },
    { MODKEY,                       XK_grave,  spawn,          {.v = (const char*[]){ "dmenuunicode", NULL } } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button1,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
    { ClkTagBar,            0,              Button4,        shiftview,      {.i = -1} },
    { ClkTagBar,            0,              Button5,        shiftview,      {.i = 1} },
};

