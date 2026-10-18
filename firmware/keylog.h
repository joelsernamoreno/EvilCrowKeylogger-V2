#pragma once
#include <Arduino.h>

#ifndef SHIFT
#define SHIFT 0x80
#endif
#ifndef ALTGR
#define ALTGR 0x40
#endif

extern const unsigned char en_us[128];
extern const unsigned char es_es[128];
extern const unsigned char fr_fr[128];
extern const unsigned char it_it[128];
extern const unsigned char da_dk[128];
extern const unsigned char de_de[128];
extern const unsigned char hr_hr[128];
extern const unsigned char hu_hu[128];
extern const unsigned char pt_pt[128];
extern const unsigned char pt_br[128];
extern const unsigned char be_be[128];
extern const unsigned char br_br[128];
extern const unsigned char ca_ca[128];
extern const unsigned char ca_fr[128];
extern const unsigned char sk_sk[128];
extern const unsigned char cz_cz[128];
extern const unsigned char sv_se[128];
extern const unsigned char si_si[128];

// level: 0 = base, 1 = shift, 2 = altgr
struct KlExtra { uint8_t key; uint8_t level; const char* s; };

// ---- ES_ES ----------------------------------------------------
static const KlExtra KL_ES[] = {
  {0x33,0,"\xC3\xB1"}, {0x33,1,"\xC3\x91"},   // ñ Ñ
  {0x31,0,"\xC3\xA7"}, {0x31,1,"\xC3\x87"},   // ç Ç   (AltGr '}' stays ASCII)
  {0x35,0,"\xC2\xBA"}, {0x35,1,"\xC2\xAA"},   // º ª   (AltGr '\' stays ASCII)
  {0x2E,0,"\xC2\xA1"}, {0x2E,1,"\xC2\xBF"},   // ¡ ¿
  {0x34,0,"\xC2\xB4"}, {0x34,1,"\xC2\xA8"},   // ´ ¨ (dead keys; AltGr '{' ASCII)
  {0x08,2,"\xE2\x82\xAC"},                    // € (AltGr+e)
  {0x64,0,"<"},        {0x64,1,">"},          // ISO <>
};
// ---- FR_FR ------------------------------------------------------
static const KlExtra KL_FR[] = {
  {0x1F,0,"\xC3\xA9"},                        // é (base of '2' key)
  {0x24,0,"\xC3\xA8"},                        // è ('7')
  {0x26,0,"\xC3\xA7"},                        // ç ('9')
  {0x27,0,"\xC3\xA0"},                        // à ('0')
  {0x34,0,"\xC3\xB9"},                        // ù (US "'")
  {0x2D,1,"\xC2\xB0"},                        // ° (shift of ')')
  {0x08,2,"\xE2\x82\xAC"},                    // €
  {0x64,0,"<"},        {0x64,1,">"},
};
// ---- DE_DE ------------------------------------------------------
static const KlExtra KL_DE[] = {
  {0x2D,0,"\xC3\x9F"},                        // ß
  {0x34,0,"\xC3\xA4"}, {0x34,1,"\xC3\x84"},   // ä Ä
  {0x33,0,"\xC3\xB6"}, {0x33,1,"\xC3\x96"},   // ö Ö
  {0x2F,0,"\xC3\xBC"}, {0x2F,1,"\xC3\x9C"},   // ü Ü
  {0x2E,0,"\xC2\xB4"},                        // ´ (dead acute; shift ` is ASCII)
  {0x35,1,"\xC2\xB0"},                        // ° (shift of ^)
  {0x10,2,"\xC2\xB5"},                        // µ (AltGr+m)
  {0x08,2,"\xE2\x82\xAC"},                    // €
  {0x64,0,"<"},        {0x64,1,">"},
};
// ---- IT_IT ------------------------------------------------------
static const KlExtra KL_IT[] = {
  {0x2E,0,"\xC3\xAC"},                        // ì
  {0x2F,0,"\xC3\xA8"}, {0x2F,1,"\xC3\xA9"},   // è é
  {0x33,0,"\xC3\xB2"}, {0x33,1,"\xC3\xA7"},   // ò ç
  {0x34,0,"\xC3\xA0"}, {0x34,1,"\xC2\xB0"},   // à °
  {0x31,0,"\xC3\xB9"}, {0x31,1,"\xC2\xA7"},   // ù §
  {0x08,2,"\xE2\x82\xAC"},                    // €
  {0x64,0,"<"},        {0x64,1,">"},
};
// ---- PT_PT ------------------------------------------------------
static const KlExtra KL_PT[] = {
  {0x33,0,"\xC3\xA7"}, {0x33,1,"\xC3\x87"},   // ç Ç
  {0x2E,0,"\xC2\xAB"}, {0x2E,1,"\xC2\xBB"},   // « »
  {0x08,2,"\xE2\x82\xAC"},                    // €
  {0x64,0,"<"},        {0x64,1,">"},
};
// ---- PT_BR ------------------------------------------------------
static const KlExtra KL_PTBR[] = {
  {0x33,0,"\xC3\xA7"}, {0x33,1,"\xC3\x87"},   // ç Ç
  {0x64,0,"<"},        {0x64,1,">"},
};
// ---- SV_SE ------------------------------------------------------
static const KlExtra KL_SV[] = {
  {0x34,0,"\xC3\xA4"}, {0x34,1,"\xC3\x84"},   // ä Ä
  {0x33,0,"\xC3\xB6"}, {0x33,1,"\xC3\x96"},   // ö Ö
  {0x2F,0,"\xC3\xA5"}, {0x2F,1,"\xC3\x85"},   // å Å
  {0x08,2,"\xE2\x82\xAC"},                    // €
  {0x64,0,"<"},        {0x64,1,">"},
};
// ---- DA_DK ------------------------------------------------------
static const KlExtra KL_DA[] = {
  {0x34,0,"\xC3\xA6"}, {0x34,1,"\xC3\x86"},   // æ Æ
  {0x33,0,"\xC3\xB8"}, {0x33,1,"\xC3\x98"},   // ø Ø
  {0x2F,0,"\xC3\xA5"}, {0x2F,1,"\xC3\x85"},   // å Å
  {0x08,2,"\xE2\x82\xAC"},                    // €
  {0x64,0,"<"},        {0x64,1,">"},
};
// ---- BE_BE ------------------------------------------------------
static const KlExtra KL_BE[] = {
  {0x1F,0,"\xC3\xA9"},                        // é
  {0x24,0,"\xC3\xA8"},                        // è
  {0x26,0,"\xC3\xA7"},                        // ç
  {0x27,0,"\xC3\xA0"},                        // à
  {0x34,0,"\xC3\xB9"},                        // ù
  {0x35,0,"\xC2\xB2"},                        // ²
  {0x08,2,"\xE2\x82\xAC"},                    // €
  {0x64,0,"<"},        {0x64,1,">"},
};

struct KlMap { const unsigned char* tab; const KlExtra* ex; uint8_t n; };
static const KlMap KL_MAPS[] = {
  { es_es, KL_ES,   sizeof(KL_ES)/sizeof(KlExtra)   },
  { fr_fr, KL_FR,   sizeof(KL_FR)/sizeof(KlExtra)   },
  { de_de, KL_DE,   sizeof(KL_DE)/sizeof(KlExtra)   },
  { it_it, KL_IT,   sizeof(KL_IT)/sizeof(KlExtra)   },
  { pt_pt, KL_PT,   sizeof(KL_PT)/sizeof(KlExtra)   },
  { pt_br, KL_PTBR, sizeof(KL_PTBR)/sizeof(KlExtra) },
  { sv_se, KL_SV,   sizeof(KL_SV)/sizeof(KlExtra)   },
  { da_dk, KL_DA,   sizeof(KL_DA)/sizeof(KlExtra)   },
  { be_be, KL_BE,   sizeof(KL_BE)/sizeof(KlExtra)   },
};
static const uint8_t KL_MAPS_N = sizeof(KL_MAPS)/sizeof(KlMap);

// Returns UTF-8 for a non-ASCII / high-scancode key, or nullptr if none.
inline const char* keylogExtra(const unsigned char* layout, uint8_t key, uint8_t level) {
  for (uint8_t m = 0; m < KL_MAPS_N; m++) {
    if (KL_MAPS[m].tab != layout) continue;
    for (uint8_t i = 0; i < KL_MAPS[m].n; i++)
      if (KL_MAPS[m].ex[i].key == key && KL_MAPS[m].ex[i].level == level)
        return KL_MAPS[m].ex[i].s;
    break;
  }
  // Generic ISO "<>" key
  if (key == 0x64) {
    if (level == 0) return "<";
    if (level == 1) return ">";
    if (level == 2) return "|";
  }
  return nullptr;
}

inline char asciiFromLayout(const unsigned char* layout, uint8_t key, bool shift, bool altgr) {
  uint8_t want = key | (shift ? SHIFT : 0) | (altgr ? ALTGR : 0);
  for (uint8_t ascii = 32; ascii < 128; ascii++)
    if (pgm_read_byte(&layout[ascii]) == want) return (char)ascii;
  return 0;
}