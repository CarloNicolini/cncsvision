/****************************************************************************/
/* LCTKEYS.H                                                                */
/****************************************************************************/
#ifndef  LCTKEYS_H_INCLUDED
#define  LCTKEYS_H_INCLUDED
/****************************************************************************/
/* KEY CONSTANTS                                                            */

#define  EXTENDED          0xE000   /* Place the non-printing characters in */
                                    /*   the UNICODE "Private Use" space    */
#define  EXTENDEDMAX       0xE1FF   /* Upper Bound, lots of extra codes     */

#define  BACKSPACE              8   /* Non-Extended ASCII values            */
#define  TAB                    9
#define  LINE_FEED             10
#define  FORM_FEED             12
#define  ENTER                 13
#define  CTRL_Z                26
#define  ESCAPE                27
#define  SPACE                 32

#define  F1          EXTENDED+ 59   /* Extended ASCII key values            */
#define  F2          EXTENDED+ 60
#define  F3          EXTENDED+ 61
#define  F4          EXTENDED+ 62
#define  F5          EXTENDED+ 63
#define  F6          EXTENDED+ 64
#define  F7          EXTENDED+ 65
#define  F8          EXTENDED+ 66
#define  F9          EXTENDED+ 67
#define  F10         EXTENDED+ 68
#define  F11         EXTENDED+ 69
#define  F12         EXTENDED+ 70

#define  SHIFT_F1    EXTENDED+ 84
#define  SHIFT_F2    EXTENDED+ 85
#define  SHIFT_F3    EXTENDED+ 86
#define  SHIFT_F4    EXTENDED+ 87
#define  SHIFT_F5    EXTENDED+ 88
#define  SHIFT_F6    EXTENDED+ 89
#define  SHIFT_F7    EXTENDED+ 90
#define  SHIFT_F8    EXTENDED+ 91
#define  SHIFT_F9    EXTENDED+ 92
#define  SHIFT_F10   EXTENDED+ 93

#define  CTRL_F1     EXTENDED+ 94
#define  CTRL_F2     EXTENDED+ 95
#define  CTRL_F3     EXTENDED+ 96
#define  CTRL_F4     EXTENDED+ 97
#define  CTRL_F5     EXTENDED+ 98
#define  CTRL_F6     EXTENDED+ 99
#define  CTRL_F7     EXTENDED+100
#define  CTRL_F8     EXTENDED+101
#define  CTRL_F9     EXTENDED+102
#define  CTRL_F10    EXTENDED+103

#define  ALT_F1      EXTENDED+104
#define  ALT_F2      EXTENDED+105
#define  ALT_F3      EXTENDED+106
#define  ALT_F4      EXTENDED+107
#define  ALT_F5      EXTENDED+108
#define  ALT_F6      EXTENDED+109
#define  ALT_F7      EXTENDED+110
#define  ALT_F8      EXTENDED+111
#define  ALT_F9      EXTENDED+112
#define  ALT_F10     EXTENDED+113

#define  HOME        EXTENDED+ 71
#define  UP_ARROW    EXTENDED+ 72
#define  PGUP        EXTENDED+ 73
#define  PLUS        EXTENDED+ 74
#define  LEFT_ARROW  EXTENDED+ 75
#define  RIGHT_ARROW EXTENDED+ 77
#define  END         EXTENDED+ 79
#define  DOWN_ARROW  EXTENDED+ 80
#define  PGDN        EXTENDED+ 81
#undef DELETE
#define  DELETE      EXTENDED+ 83
                             
#define  ALT_LEFT_ARROW   EXTENDED+155
#define  ALT_RIGHT_ARROW  EXTENDED+157

#define  CTRL_PGUP   EXTENDED+132
#define  CTRL_PGDN   EXTENDED+118
#define  CTRL_HOME   EXTENDED+119
#define  CTRL_END    EXTENDED+117
#define  CTRL_LEFT_ARROW   EXTENDED+115
#define  CTRL_RIGHT_ARROW  EXTENDED+116
#define  CTRL_UP_ARROW     EXTENDED+141
#define  CTRL_DOWN_ARROW   EXTENDED+145

/****************************************************************************/
#endif // LCTKEYS_H_INCLUDED
/****************************************************************************/

