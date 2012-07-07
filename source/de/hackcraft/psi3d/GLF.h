/* 
 * File:   GLS.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on someday
 */

#ifndef _GLFONT_H
#define	_GLFONT_H

#include <GL/glew.h>

#include <stdlib.h>
#include <stdio.h>

#include <stdarg.h>
#include <cmath>

#define FORMATBUFFER \
    va_list args; \
    va_start(args, format); \
    char buffer[1024]; \
    int len = vsnprintf(buffer, 1023, format, args); \
    buffer[len] = '\0'; \
    va_end(args);

#define MIPMAPPEDFONT 0

// 256 ASCI characacter table.
// 8x times 16y bits = 128bits = 4 unsigned longs for each character.
// Character Pixels are stored row major (least significant bit of a
// byte is the left most pixel: 1 << x)
#define FONTTABLEDATA    \
    0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, /* (0) */ \
    0X817E0000, 0XA58181A5, 0X7E818199, 0X0, /* (1) */ \
    0XFF7E0000, 0XDBFFFFDB, 0X7EFFFFE7, 0X0, /* (2) */ \
    0X0, 0X7F7F7F36, 0X81C3E7F, 0X0, /* (3) */ \
    0X0, 0X7F3E1C08, 0X81C3E, 0X0, /* (4) */ \
    0X18000000, 0XE7E73C3C, 0X3C1818E7, 0X0, /* (5) */ \
    0X18000000, 0XFFFF7E3C, 0X3C18187E, 0X0, /* (6) */ \
    0X0, 0X3C180000, 0X183C, 0X0, /* (7) */ \
    0XFFFFFFFF, 0XC3E7FFFF, 0XFFFFE7C3, 0XFFFFFFFF, /* (8) */ \
    0X0, 0X42663C00, 0X3C6642, 0X0, /* (9) */ \
    0XFFFFFFFF, 0XBD99C3FF, 0XFFC399BD, 0XFFFFFFFF, /* (10) */ \
    0X60780000, 0X331E5870, 0X1E333333, 0X0, /* (11) */ \
    0X663C0000, 0X3C666666, 0X18187E18, 0X0, /* (12) */ \
    0XCCFC0000, 0XC0C0CFC, 0X70F0E0C, 0X0, /* (13) */ \
    0XC6FE0000, 0XC6C6C6FE, 0X67E7E6C6, 0X3, /* (14) */ \
    0X18000000, 0XE73CDB18, 0X1818DB3C, 0X0, /* (15) */ \
    0X7030100, 0X1F7F1F0F, 0X103070F, 0X0, /* (16) */ \
    0X70604000, 0X7C7F7C78, 0X40607078, 0X0, /* (17) */ \
    0X3C180000, 0X1818187E, 0X183C7E, 0X0, /* (18) */ \
    0X66660000, 0X66666666, 0X66660066, 0X0, /* (19) */ \
    0XDBFE0000, 0XD8DEDBDB, 0XD8D8D8D8, 0X0, /* (20) */ \
    0X6633E00, 0X6363361C, 0X63301C36, 0X3E, /* (21) */ \
    0X0, 0X0, 0X7F7F7F7F, 0X0, /* (22) */ \
    0X3C180000, 0X1818187E, 0X7E183C7E, 0X0, /* (23) */ \
    0X3C180000, 0X1818187E, 0X18181818, 0X0, /* (24) */ \
    0X18180000, 0X18181818, 0X183C7E18, 0X0, /* (25) */ \
    0X0, 0X7F301800, 0X1830, 0X0, /* (26) */ \
    0X0, 0X7F060C00, 0XC06, 0X0, /* (27) */ \
    0X0, 0X3030000, 0X7F03, 0X0, /* (28) */ \
    0X0, 0X7F361400, 0X1436, 0X0, /* (29) */ \
    0X0, 0X3E1C1C08, 0X7F7F3E, 0X0, /* (30) */ \
    0X0, 0X3E3E7F7F, 0X81C1C, 0X0, /* (31) */ \
    0X0, 0X0, 0X0, 0X0, /* Space (32) */ \
    0X3C180000, 0X18183C3C, 0X18180018, 0X0, /* ! (33) */ \
    0X66666600, 0X24, 0X0, 0X0, /* " (34) */ \
    0X36000000, 0X36367F36, 0X36367F36, 0X0, /* # (35) */ \
    0X633E1818, 0X603E0343, 0X3E636160, 0X1818, /* $ (36) */ \
    0X0, 0X18306343, 0X6163060C, 0X0, /* % (37) */ \
    0X361C0000, 0X3B6E1C36, 0X6E333333, 0X0, /* & (38) */ \
    0XC0C0C00, 0X6, 0X0, 0X0, /* ' (39) */ \
    0X18300000, 0XC0C0C0C, 0X30180C0C, 0X0, /* ( (40) */ \
    0X180C0000, 0X30303030, 0XC183030, 0X0, /* ) (41) */ \
    0X0, 0XFF3C6600, 0X663C, 0X0, /* * (42) */ \
    0X0, 0X7E181800, 0X1818, 0X0, /* + (43) */ \
    0X0, 0X0, 0X18181800, 0XC, /* , (44) */ \
    0X0, 0X7F000000, 0X0, 0X0, /* - (45) */ \
    0X0, 0X0, 0X18180000, 0X0, /* . (46) */ \
    0X0, 0X18306040, 0X103060C, 0X0, /* / (47) */ \
    0X361C0000, 0X6B6B6363, 0X1C366363, 0X0, /* 0 (48) */ \
    0X1C180000, 0X1818181E, 0X7E181818, 0X0, /* 1 (49) */ \
    0X633E0000, 0XC183060, 0X7F630306, 0X0, /* 2 (50) */ \
    0X633E0000, 0X603C6060, 0X3E636060, 0X0, /* 3 (51) */ \
    0X38300000, 0X7F33363C, 0X78303030, 0X0, /* 4 (52) */ \
    0X37F0000, 0X603F0303, 0X3E636060, 0X0, /* 5 (53) */ \
    0X61C0000, 0X633F0303, 0X3E636363, 0X0, /* 6 (54) */ \
    0X637F0000, 0X18306060, 0XC0C0C0C, 0X0, /* 7 (55) */ \
    0X633E0000, 0X633E6363, 0X3E636363, 0X0, /* 8 (56) */ \
    0X633E0000, 0X607E6363, 0X1E306060, 0X0, /* 9 (57) */ \
    0X0, 0X1818, 0X181800, 0X0, /* : (58) */ \
    0X0, 0X1818, 0XC181800, 0X0, /* ; (59) */ \
    0X60000000, 0X60C1830, 0X6030180C, 0X0, /* < (60) */ \
    0X0, 0X7E00, 0X7E, 0X0, /* = (61) */ \
    0X6000000, 0X6030180C, 0X60C1830, 0X0, /* > (62) */ \
    0X633E0000, 0X18183063, 0X18180018, 0X0, /* ? (63) */ \
    0X3E000000, 0X7B7B6363, 0X3E033B7B, 0X0, /* @ (64) */ \
    0X1C080000, 0X7F636336, 0X63636363, 0X0, /* A (65) */ \
    0X663F0000, 0X663E6666, 0X3F666666, 0X0, /* B (66) */ \
    0X663C0000, 0X3030343, 0X3C664303, 0X0, /* C (67) */ \
    0X361F0000, 0X66666666, 0X1F366666, 0X0, /* D (68) */ \
    0X667F0000, 0X161E1646, 0X7F664606, 0X0, /* E (69) */ \
    0X667F0000, 0X161E1646, 0XF060606, 0X0, /* F (70) */ \
    0X663C0000, 0X7B030343, 0X5C666363, 0X0, /* G (71) */ \
    0X63630000, 0X637F6363, 0X63636363, 0X0, /* H (72) */ \
    0X183C0000, 0X18181818, 0X3C181818, 0X0, /* I (73) */ \
    0X30780000, 0X30303030, 0X1E333333, 0X0, /* J (74) */ \
    0X66670000, 0X1E1E3666, 0X67666636, 0X0, /* K (75) */ \
    0X60F0000, 0X6060606, 0X7F664606, 0X0, /* L (76) */ \
    0X77630000, 0X636B7F7F, 0X63636363, 0X0, /* M (77) */ \
    0X67630000, 0X737B7F6F, 0X63636363, 0X0, /* N (78) */ \
    0X633E0000, 0X63636363, 0X3E636363, 0X0, /* O (79) */ \
    0X663F0000, 0X63E6666, 0XF060606, 0X0, /* P (80) */ \
    0X633E0000, 0X63636363, 0X3E7B6B63, 0X7030, /* Q (81) */ \
    0X663F0000, 0X363E6666, 0X67666666, 0X0, /* R (82) */ \
    0X633E0000, 0X301C0663, 0X3E636360, 0X0, /* S (83) */ \
    0X7E7E0000, 0X1818185A, 0X3C181818, 0X0, /* T (84) */ \
    0X63630000, 0X63636363, 0X3E636363, 0X0, /* U (85) */ \
    0X63630000, 0X63636363, 0X81C3663, 0X0, /* V (86) */ \
    0X63630000, 0X6B6B6363, 0X36777F6B, 0X0, /* W (87) */ \
    0X63630000, 0X1C1C3E36, 0X6363363E, 0X0, /* X (88) */ \
    0X66660000, 0X183C6666, 0X3C181818, 0X0, /* Y (89) */ \
    0X637F0000, 0XC183061, 0X7F634306, 0X0, /* Z (90) */ \
    0XC3C0000, 0XC0C0C0C, 0X3C0C0C0C, 0X0, /* [ (91) */ \
    0X1000000, 0X1C0E0703, 0X40607038, 0X0, /* Backslash (92) */ \
    0X303C0000, 0X30303030, 0X3C303030, 0X0, /* ] (93) */ \
    0X63361C08, 0X0, 0X0, 0X0, /* ^ (94) */ \
    0X0, 0X0, 0X0, 0XFF00, /* _ (95) */ \
    0X180C0C, 0X0, 0X0, 0X0, /* ` (96) */ \
    0X0, 0X3E301E00, 0X6E333333, 0X0, /* a (97) */ \
    0X6070000, 0X66361E06, 0X3E666666, 0X0, /* b (98) */ \
    0X0, 0X3633E00, 0X3E630303, 0X0, /* c (99) */ \
    0X30380000, 0X33363C30, 0X6E333333, 0X0, /* d (100) */ \
    0X0, 0X7F633E00, 0X3E630303, 0X0, /* e (101) */ \
    0X361C0000, 0X60F0626, 0XF060606, 0X0, /* f (102) */ \
    0X0, 0X33336E00, 0X3E333333, 0X1E3330, /* g (103) */ \
    0X6070000, 0X666E3606, 0X67666666, 0X0, /* h (104) */ \
    0X18180000, 0X18181C00, 0X3C181818, 0X0, /* i (105) */ \
    0X60600000, 0X60607000, 0X60606060, 0X3C6666, /* j (106) */ \
    0X6070000, 0X1E366606, 0X6766361E, 0X0, /* k (107) */ \
    0X181C0000, 0X18181818, 0X3C181818, 0X0, /* l (108) */ \
    0X0, 0X6B7F3700, 0X636B6B6B, 0X0, /* m (109) */ \
    0X0, 0X66663B00, 0X66666666, 0X0, /* n (110) */ \
    0X0, 0X63633E00, 0X3E636363, 0X0, /* o (111) */ \
    0X0, 0X66663B00, 0X3E666666, 0XF0606, /* p (112) */ \
    0X0, 0X33336E00, 0X3E333333, 0X783030, /* q (113) */ \
    0X0, 0X666E3B00, 0XF060606, 0X0, /* r (114) */ \
    0X0, 0X6633E00, 0X3E63301C, 0X0, /* s (115) */ \
    0XC080000, 0XC0C3F0C, 0X386C0C0C, 0X0, /* t (116) */ \
    0X0, 0X33333300, 0X6E333333, 0X0, /* u (117) */ \
    0X0, 0X66666600, 0X183C6666, 0X0, /* v (118) */ \
    0X0, 0X6B636300, 0X367F6B6B, 0X0, /* w (119) */ \
    0X0, 0X1C366300, 0X63361C1C, 0X0, /* x (120) */ \
    0X0, 0X63636300, 0X7E636363, 0X1F3060, /* y (121) */ \
    0X0, 0X18337F00, 0X7F63060C, 0X0, /* z (122) */ \
    0X18700000, 0X180E1818, 0X70181818, 0X0, /* { (123) */ \
    0X18180000, 0X18001818, 0X18181818, 0X0, /* | (124) */ \
    0X180E0000, 0X18701818, 0XE181818, 0X0, /* } (125) */ \
    0X3B6E0000, 0X0, 0X0, 0X0, /* ~ (126) */ \
    0X0, 0X63361C08, 0X7F6363, 0X0, /*  (127) */ \
    0X663C0000, 0X3030343, 0X303C6643, 0X3E60, /* ? (128) */ \
    0X330000, 0X33333300, 0X6E333333, 0X0, /* � (129) */ \
    0XC183000, 0X7F633E00, 0X3E630303, 0X0, /* ? (130) */ \
    0X361C0800, 0X3E301E00, 0X6E333333, 0X0, /* ? (131) */ \
    0X330000, 0X3E301E00, 0X6E333333, 0X0, /* ? (132) */ \
    0X180C0600, 0X3E301E00, 0X6E333333, 0X0, /* ? (133) */ \
    0X1C361C00, 0X3E301E00, 0X6E333333, 0X0, /* ? (134) */ \
    0X0, 0X606663C, 0X60303C66, 0X3C, /* ? (135) */ \
    0X361C0800, 0X7F633E00, 0X3E630303, 0X0, /* ? (136) */ \
    0X630000, 0X7F633E00, 0X3E630303, 0X0, /* ? (137) */ \
    0X180C0600, 0X7F633E00, 0X3E630303, 0X0, /* ? (138) */ \
    0X660000, 0X18181C00, 0X3C181818, 0X0, /* ? (139) */ \
    0X663C1800, 0X18181C00, 0X3C181818, 0X0, /* ? (140) */ \
    0X180C0600, 0X18181C00, 0X3C181818, 0X0, /* � (141) */ \
    0X8006300, 0X6363361C, 0X6363637F, 0X0, /* ? (142) */ \
    0X1C361C, 0X6363361C, 0X6363637F, 0X0, /* � (143) */ \
    0X60C18, 0X3E06667F, 0X7F660606, 0X0, /* � (144) */ \
    0X0, 0X6C6E3300, 0X761B1B7E, 0X0, /* ? (145) */ \
    0X367C0000, 0X337F3333, 0X73333333, 0X0, /* ? (146) */ \
    0X361C0800, 0X63633E00, 0X3E636363, 0X0, /* ? (147) */ \
    0X630000, 0X63633E00, 0X3E636363, 0X0, /* ? (148) */ \
    0X180C0600, 0X63633E00, 0X3E636363, 0X0, /* ? (149) */ \
    0X331E0C00, 0X33333300, 0X6E333333, 0X0, /* ? (150) */ \
    0X180C0600, 0X33333300, 0X6E333333, 0X0, /* ? (151) */ \
    0X630000, 0X63636300, 0X7E636363, 0X1E3060, /* ? (152) */ \
    0X3E006300, 0X63636363, 0X3E636363, 0X0, /* ? (153) */ \
    0X63006300, 0X63636363, 0X3E636363, 0X0, /* ? (154) */ \
    0X3C181800, 0X6060666, 0X18183C66, 0X0, /* ? (155) */ \
    0X26361C00, 0X6060F06, 0X3F670606, 0X0, /* ? (156) */ \
    0X66660000, 0X187E183C, 0X1818187E, 0X0, /* � (157) */ \
    0X33331F00, 0X7B33231F, 0X63333333, 0X0, /* ? (158) */ \
    0X18D87000, 0X187E1818, 0X18181818, 0XE1B, /* ? (159) */ \
    0X60C1800, 0X3E301E00, 0X6E333333, 0X0, /* � (160) */ \
    0XC183000, 0X18181C00, 0X3C181818, 0X0, /* � (161) */ \
    0X60C1800, 0X63633E00, 0X3E636363, 0X0, /* � (162) */ \
    0X60C1800, 0X33333300, 0X6E333333, 0X0, /* � (163) */ \
    0X3B6E0000, 0X66663B00, 0X66666666, 0X0, /* � (164) */ \
    0X63003B6E, 0X7B7F6F67, 0X63636373, 0X0, /* � (165) */ \
    0X36363C00, 0X7E007C, 0X0, 0X0, /* � (166) */ \
    0X36361C00, 0X3E001C, 0X0, 0X0, /* � (167) */ \
    0XC0C0000, 0X60C0C00, 0X3E636303, 0X0, /* � (168) */ \
    0X0, 0X37F0000, 0X30303, 0X0, /* � (169) */ \
    0X0, 0X607F0000, 0X606060, 0X0, /* � (170) */ \
    0X43030300, 0XC183363, 0X30613B06, 0X7C18, /* � (171) */ \
    0X43030300, 0XC183363, 0X7C797366, 0X6060, /* � (172) */ \
    0X18180000, 0X18181800, 0X183C3C3C, 0X0, /* � (173) */ \
    0X0, 0X1B366C00, 0X6C36, 0X0, /* � (174) */ \
    0X0, 0X6C361B00, 0X1B36, 0X0, /* � (175) */ \
    0X22882288, 0X22882288, 0X22882288, 0X22882288, /* � (176) */ \
    0X55AA55AA, 0X55AA55AA, 0X55AA55AA, 0X55AA55AA, /* � (177) */ \
    0XEEBBEEBB, 0XEEBBEEBB, 0XEEBBEEBB, 0XEEBBEEBB, /* � (178) */ \
    0X18181818, 0X18181818, 0X18181818, 0X18181818, /* � (179) */ \
    0X18181818, 0X1F181818, 0X18181818, 0X18181818, /* � (180) */ \
    0X18181818, 0X1F181F18, 0X18181818, 0X18181818, /* � (181) */ \
    0X6C6C6C6C, 0X6F6C6C6C, 0X6C6C6C6C, 0X6C6C6C6C, /* � (182) */ \
    0X0, 0X7F000000, 0X6C6C6C6C, 0X6C6C6C6C, /* � (183) */ \
    0X0, 0X1F181F00, 0X18181818, 0X18181818, /* � (184) */ \
    0X6C6C6C6C, 0X6F606F6C, 0X6C6C6C6C, 0X6C6C6C6C, /* � (185) */ \
    0X6C6C6C6C, 0X6C6C6C6C, 0X6C6C6C6C, 0X6C6C6C6C, /* � (186) */ \
    0X0, 0X6F607F00, 0X6C6C6C6C, 0X6C6C6C6C, /* � (187) */ \
    0X6C6C6C6C, 0X7F606F6C, 0X0, 0X0, /* � (188) */ \
    0X6C6C6C6C, 0X7F6C6C6C, 0X0, 0X0, /* � (189) */ \
    0X18181818, 0X1F181F18, 0X0, 0X0, /* � (190) */ \
    0X0, 0X1F000000, 0X18181818, 0X18181818, /* � (191) */ \
    0X18181818, 0XF8181818, 0X0, 0X0, /* � (192) */ \
    0X18181818, 0XFF181818, 0X0, 0X0, /* � (193) */ \
    0X0, 0XFF000000, 0X18181818, 0X18181818, /* � (194) */ \
    0X18181818, 0XF8181818, 0X18181818, 0X18181818, /* � (195) */ \
    0X0, 0XFF000000, 0X0, 0X0, /* � (196) */ \
    0X18181818, 0XFF181818, 0X18181818, 0X18181818, /* � (197) */ \
    0X18181818, 0XF818F818, 0X18181818, 0X18181818, /* � (198) */ \
    0X6C6C6C6C, 0XEC6C6C6C, 0X6C6C6C6C, 0X6C6C6C6C, /* � (199) */ \
    0X6C6C6C6C, 0XFC0CEC6C, 0X0, 0X0, /* � (200) */ \
    0X0, 0XEC0CFC00, 0X6C6C6C6C, 0X6C6C6C6C, /* � (201) */ \
    0X6C6C6C6C, 0XFF00EF6C, 0X0, 0X0, /* � (202) */ \
    0X0, 0XEF00FF00, 0X6C6C6C6C, 0X6C6C6C6C, /* � (203) */ \
    0X6C6C6C6C, 0XEC0CEC6C, 0X6C6C6C6C, 0X6C6C6C6C, /* � (204) */ \
    0X0, 0XFF00FF00, 0X0, 0X0, /* � (205) */ \
    0X6C6C6C6C, 0XEF00EF6C, 0X6C6C6C6C, 0X6C6C6C6C, /* � (206) */ \
    0X18181818, 0XFF00FF18, 0X0, 0X0, /* � (207) */ \
    0X6C6C6C6C, 0XFF6C6C6C, 0X0, 0X0, /* � (208) */ \
    0X0, 0XFF00FF00, 0X18181818, 0X18181818, /* � (209) */ \
    0X0, 0XFF000000, 0X6C6C6C6C, 0X6C6C6C6C, /* � (210) */ \
    0X6C6C6C6C, 0XFC6C6C6C, 0X0, 0X0, /* � (211) */ \
    0X18181818, 0XF818F818, 0X0, 0X0, /* � (212) */ \
    0X0, 0XF818F800, 0X18181818, 0X18181818, /* � (213) */ \
    0X0, 0XFC000000, 0X6C6C6C6C, 0X6C6C6C6C, /* � (214) */ \
    0X6C6C6C6C, 0XFF6C6C6C, 0X6C6C6C6C, 0X6C6C6C6C, /* � (215) */ \
    0X18181818, 0XFF18FF18, 0X18181818, 0X18181818, /* � (216) */ \
    0X18181818, 0X1F181818, 0X0, 0X0, /* � (217) */ \
    0X0, 0XF8000000, 0X18181818, 0X18181818, /* � (218) */ \
    0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, /* � (219) */ \
    0X0, 0XFF000000, 0XFFFFFFFF, 0XFFFFFFFF, /* � (220) */ \
    0XF0F0F0F, 0XF0F0F0F, 0XF0F0F0F, 0XF0F0F0F, /* � (221) */ \
    0XF0F0F0F0, 0XF0F0F0F0, 0XF0F0F0F0, 0XF0F0F0F0, /* � (222) */ \
    0XFFFFFFFF, 0XFFFFFF, 0X0, 0X0, /* � (223) */ \
    0X0, 0X1B3B6E00, 0X6E3B1B1B, 0X0, /* � (224) */ \
    0X331E0000, 0X331B3333, 0X33636363, 0X0, /* � (225) */ \
    0X637F0000, 0X3030363, 0X3030303, 0X0, /* � (226) */ \
    0X0, 0X3636367F, 0X36363636, 0X0, /* � (227) */ \
    0X7F000000, 0X180C0663, 0X7F63060C, 0X0, /* � (228) */ \
    0X0, 0X1B1B7E00, 0XE1B1B1B, 0X0, /* � (229) */ \
    0X0, 0X66666666, 0X6063E66, 0X3, /* � (230) */ \
    0X0, 0X18183B6E, 0X18181818, 0X0, /* � (231) */ \
    0X7E000000, 0X66663C18, 0X7E183C66, 0X0, /* � (232) */ \
    0X1C000000, 0X7F636336, 0X1C366363, 0X0, /* � (233) */ \
    0X361C0000, 0X36636363, 0X77363636, 0X0, /* � (234) */ \
    0XC780000, 0X667C3018, 0X3C666666, 0X0, /* � (235) */ \
    0X0, 0XDBDB7E00, 0X7EDB, 0X0, /* � (236) */ \
    0XC0000000, 0XDBDB7E60, 0X3067ECF, 0X0, /* � (237) */ \
    0XC380000, 0X63E0606, 0X380C0606, 0X0, /* � (238) */ \
    0X3E000000, 0X63636363, 0X63636363, 0X0, /* � (239) */ \
    0X0, 0X7F00007F, 0X7F0000, 0X0, /* � (240) */ \
    0X0, 0X187E1818, 0XFF000018, 0X0, /* � (241) */ \
    0XC000000, 0X30603018, 0X7E000C18, 0X0, /* � (242) */ \
    0X30000000, 0XC060C18, 0X7E003018, 0X0, /* � (243) */ \
    0XD8700000, 0X181818D8, 0X18181818, 0X18181818, /* � (244) */ \
    0X18181818, 0X18181818, 0XE1B1B1B, 0X0, /* � (245) */ \
    0X0, 0X7E001818, 0X181800, 0X0, /* � (246) */ \
    0X0, 0X3B6E00, 0X3B6E, 0X0, /* � (247) */ \
    0X36361C00, 0X1C, 0X0, 0X0, /* � (248) */ \
    0X0, 0X18000000, 0X18, 0X0, /* � (249) */ \
    0X0, 0X0, 0X18, 0X0, /* � (250) */ \
    0X3030F000, 0X37303030, 0X383C3636, 0X0, /* � (251) */ \
    0X36361B00, 0X363636, 0X0, 0X0, /* � (252) */ \
    0XC1B0E00, 0X1F1306, 0X0, 0X0, /* � (253) */ \
    0X0, 0X3E3E3E3E, 0X3E3E3E, 0X0, /* � (254) */ \
    0X0, 0X0, 0X0, 0X0, /* Unbreakable Space (255) */


/**
 * Textured Console Font out of a box.
 * For opengl use glUploadFont()
 * and then glprintf("Hello World");
 * used in Console class.
 *
 * See method definitions for further comment and information.
 * See end of file for a testdriver/sample of use.
 */
struct GLF {
    typedef int32_t four_bytes;


    /**
     * Retrieves the black(0) or white(255) pixel color of the given ascii character at
     * position (x, y) ([0, 8] and [0, 15] range).
     *
     * @param font
     * @param ascii
     * @param x
     * @param y
     * @return
     */
    static unsigned char getFontPixel(four_bytes* font, unsigned char ascii, int x, int y) {
        int u = (int) (x);
        int v = (int) (y);
        unsigned char result = 0; \
        if (((unsigned char*) font)[(((unsigned int)(ascii))&0xFF) * 16 + v] & (1UL << u)) result = 255;
        return result;
    }

    /**
     * Retrieves the 8x16 monochrome bitmap of the specified ascii character.
     *
     * @param font
     * @param bitmap
     * @param ascii
     */
    static void getFontBitmap_8_16_mono(four_bytes* font, unsigned char* bitmap, unsigned char ascii) {
        int i;
        for (i = 0; i < 8 * 16; i++) {
            bitmap[i] = getFontPixel(font, ascii, i % 8, i / 8);
        }
    }

    /**
     * Retrieves the white 8x16 rgb bitmap of the specified ascii character.
     *
     * @param font
     * @param bitmap
     * @param ascii
     */
    static void getFontBitmap_8_16_rgb(four_bytes* font, unsigned char* bitmap, unsigned char ascii) {
        int i;
        for (i = 0; i < 8 * 16 * 3; i += 3) {
            int j = i / 3;
            bitmap[i + 0] = getFontPixel(font, ascii, j % 8, j / 8);
            bitmap[i + 1] = bitmap[i];
            bitmap[i + 2] = bitmap[i];
        }
    }

    /**
     * Retrieves the white 8x16 rgba bitmap of the specified ascii character
     * where every black pixel is translucent.
     *
     * @param font
     * @param bitmap
     * @param ascii
     */
    static void getFontBitmap_8_16_rgba(four_bytes* font, unsigned char* bitmap, unsigned char ascii) {
        int i;
        for (i = 0; i < 8 * 16 * 4; i += 4) {
            int j = i >> 2;
            unsigned char mono = getFontPixel(font, ascii, (j & 7), (j >> 3));
            float r, g, b;
            float c = ((j >> 3) / 16.0f);
            c = (c < 0.5f) ? (c) : (c - 0.5f);
            c = (0.5 + c) * (0.7 + 0.3 * sin((j >> 3) / 16.0f * M_PI));
            r = g = b = c;
            bitmap[i + 0] = (unsigned char) (r * mono);
            bitmap[i + 1] = (unsigned char) (g * mono);
            bitmap[i + 2] = (unsigned char) (b * mono);
            bitmap[i + 3] = mono;
        }
    }

    static unsigned int* getFont() {
        static unsigned int* gInstantfont = NULL;
        if (gInstantfont == NULL) {
            gInstantfont = glUploadFont();
        }
        return gInstantfont;
    }

    /**
     * Use this function to generate and upload GL textures for ascii characters
     * { start_ascii+0, start_ascii+1, ..., start_ascii+howmany-1 },
     * where start_ascii == 0 and howmany = 256.
     * The corresponding GL bind ids are stored back into gFontBind[0], gFontBindv[1], ..., gFontBind[howmany-1].
     * gFontBind is a user defined pointer - memory is allocated by this function
     * and it is user-responsibility to free and set to NULL when not used anymore.
     */
    static unsigned int* glUploadFont() {
        four_bytes font[] = {FONTTABLEDATA};
        unsigned int start_ascii = 0;
        unsigned int end_ascii = 255;
        unsigned int* binds = (unsigned int*) malloc(sizeof (unsigned int) * 256);
        glPushAttrib(GL_TEXTURE_BIT);
        glEnable(GL_TEXTURE_2D);
        glGenTextures(end_ascii - start_ascii, binds);
        unsigned char* rgba = (unsigned char*) malloc(sizeof (char) *8 * 16 * 4); /* glTexImage2D *needs* dynamic memory.*/
        unsigned int i;
        for (i = 0; i <= end_ascii; i++) {
            glBindTexture(GL_TEXTURE_2D, binds[i]);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            unsigned char ascii = (start_ascii + i);
            getFontBitmap_8_16_rgba(font, rgba, ascii);
            if (MIPMAPPEDFONT) {
                gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, 8, 16, GL_RGBA, GL_UNSIGNED_BYTE, (void*) rgba);
            } else {
                glTexImage2D(GL_TEXTURE_2D, 0, 4, 8, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*) rgba);
            }
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        free(rgba);
        glPopAttrib();
        return binds;
    }

    /**
     * Draws a texture mapped square from (0,0,0) to (1,-1,0)
     * with texture coords (0,0) to (1,1).
     * With gluPerspective that's a square from left-top to right-low.
     *
     * @param bind
     */
    static void glTexturedSquare(unsigned int bind) {
        glBindTexture(GL_TEXTURE_2D, (unsigned int) bind);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(0, 0, 0);
        glTexCoord2f(1, 0);
        glVertex3f(1, 0, 0);
        glTexCoord2f(1, 1);
        glVertex3f(1, -1, 0);
        glTexCoord2f(0, 1);
        glVertex3f(0, -1, 0);
        glEnd();
    }

    /**
     * Just call like printf but with asciibinds being all gl
     * texture bindings for character pictures 0..255.
     *
     * @param buffer
     */
    static void glprint(const char* buffer) {
        char* buf = (char*) buffer;
        int len = strlen(buf);
        unsigned int* asciibinds = getFont();
        if (asciibinds == NULL) return;
        glPushAttrib(GL_ENABLE_BIT);
        {
            glDisable(GL_CULL_FACE);
            glEnable(GL_TEXTURE_2D);
            glPushMatrix();
            {
                int i, col = 0;
                for (i = 0; i < len; i++) {
                    if (buf[i] == '\n') {
                        glTranslatef(-col, -1, 0);
                        col = 0;
                    } else if (buf[i] == '\t') {
                        glTranslatef((col + 1) % 5, 0, 0);
                        col += ((col + 1) % 5);
                    } else {
                        glTexturedSquare(asciibinds[(unsigned char)buf[i]]);
                        glTranslatef(1, 0, 0);
                        col++;
                    }
                }
            }
            glPopMatrix();
        }
        glPopAttrib();
    }

    /**
     *
     * @param format
     * @param ...
     */
    static void glprintf(const char* format, ...) {
        FORMATBUFFER
        glprint(buffer);
    }

};


#endif


// To compile the test driver program you need to define
// GLUTTESTDRIVER (either below or using gcc's -D option).
// You'll need glut to run the test driver!

#define nGLUTTESTDRIVER
#if defined(GLUTTESTDRIVER)

#include "de/hackcraft/psi3d/Console.h"

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>

unsigned int* gInstantfont = NULL;


Console gCon[4];

void Init() {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glUploadFont();

    Console* con = NULL;

    con = &gCon[0];
    con->cursor = 0;
    con->width = 16;
    con->size = con->width * 7;
    con->buffer = (char*) malloc(con->size);
    memset(con->buffer, 32, con->size);

    bnprintf(con->buffer, con->size, con->width, &con->cursor,
            "Booting system.\n12345678\n12\t678\n12345678\nDone.\n............................."
            );

    con = &gCon[1];
    con->cursor = 0;
    con->width = 40;
    con->size = con->width * 30;
    con->buffer = (char*) malloc(con->size);
    memset(con->buffer, 32, con->size);
}

void Reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (double) w / (double) h, 0.001, 100);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
}

void Draw() {
    //  glClearColor(0.3, 0.3, 0.7, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static float t = 0;
    t += 0.3;

    Console* con = NULL;

    glDisable(GL_BLEND);
    glColor3f(0, 0, 0.6);
    glPushMatrix();
    glTranslatef(0, 0, -35);
    glRotatef(t * 10, 1, 2, 3);
    glTranslatef(-20, +15, 0);
    con = &gCon[1];
    glDrawConsole(con->buffer, con->size, con->width, con->cursor);
    glPopMatrix();
    glEnable(GL_BLEND);

    con = &gCon[1];
    bnprintf(con->buffer, con->size, con->width, &con->cursor, "123");

    glColor3f(0.1, 0.4, 0.1);
    glPushMatrix();
    glTranslatef(-19.0 / 2.0, +7.5, -14);
    glPrintf("Console System Test");
    glPopMatrix();

    glColor3f(0.1, 0.4, 0.1);
    glPushMatrix();
    glTranslatef(-12.0 / 2.0, -6.5, -14);
    glPrintf("Instant Font");
    glPopMatrix();

    glColor3f(0, 0.3, 0.7);
    glPushMatrix();
    glTranslatef(-5, +5, -20);
    glScalef(0.6, 1.2, 1);
    con = &gCon[0];
    glDrawConsole(con->buffer, con->size, con->width, con->cursor);
    glPopMatrix();

    glutSwapBuffers();
}

void Idle() {
    glutPostRedisplay();
}

void Key(unsigned char k, int x, int y) {
    if (k == 27) exit(0);
}

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    int argc = 2;
    char *argv[] = {"Nothing", "Nothing", "Nothing"};

#else

int main(int argc, char **argv) {

#endif

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);

#define FULLSCREEN
#if defined(FULLSCREEN)
    glutGameModeString("640x480:16");
    glutEnterGameMode();
    if (glutGameModeGet(GLUT_GAME_MODE_WIDTH) == -1)
#endif
    {
        glutInitWindowSize(640, 480);
        glutInitWindowPosition(0, 0);
        glutCreateWindow("benjamin.pickhardt udo.edu");
    };

    glutDisplayFunc(*Draw);
    glutReshapeFunc(*Reshape);
    glutIdleFunc(*Idle);
    glutKeyboardFunc(*Key);

    //  glutSetCursor(GLUT_CURSOR_NONE);
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    Init();
    glutMainLoop();

    return 0;
}

#endif
