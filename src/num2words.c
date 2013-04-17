#include "num2words.h"
#include "string.h"

static const char* const ONES[] = {
  "cero",
  "una",
  "dos",
  "tres",
  "cuatro",
  "cinco",
  "seis",
  "siete",
  "ocho",
  "nueve"
};

static const char* const TEENS[] ={
  "",
  "once",
  "doce",
  "trece",
  "catorce",
  "quince",
  "dieciseis",
  "diecisiete",
  "dieciocho",
  "diecinueve"
};

static const char* const TENS[] = {
  "",
  "diez",
  "veinte",
  "treinta",
  "cuarenta",
  "cincuenta",
  "sesenta",
  "setenta",
  "ochenta",
  "noventa"
};

static const char* STR_OH_CLOCK = " en punto";
static const char* STR_NOON = "medio dia";
static const char* STR_MIDNIGHT = "media noche";
static const char* STR_QUARTER = "cuarto";
static const char* STR_TO = "menos";
static const char* STR_PAST = "pasado";
static const char* STR_HALF = "media";
static const char* STR_AND = "y";
static const char* STR_AM = " a.m.";
static const char* STR_PM = " p.m.";
static const char* STR_SPECIAL_25 = "veinti- cinco";


static size_t append_number(char* words, int num) {
  int tens_val = num / 10 % 10;
  int ones_val = num % 10;

  size_t len = 0;

  if (tens_val > 0) {
    if (tens_val == 1 && num != 10) {
      strcat(words, TEENS[ones_val]);
      return strlen(TEENS[ones_val]);
    }
    strcat(words, TENS[tens_val]);
    len += strlen(TENS[tens_val]);
    if (ones_val > 0) {
      strcat(words, " ");
      len += 1;
    }
  }

  if (ones_val > 0 || num == 0) {
    
    if(tens_val > 0) {
      strcat(words, STR_AND);
      strcat(words, " ");
      len += strlen(STR_AND) + 1;
    }

    strcat(words, ONES[ones_val]);
    len += strlen(ONES[ones_val]);
  }
  return len;
}

static size_t append_string(char* buffer, const size_t length, const char* str) {
  strncat(buffer, str, length);

  size_t written = strlen(str);
  return (length > written) ? written : length;
}

void fuzzy_time_to_words(int hours, int minutes, char* words, size_t length) {
  
  int fuzzy_hours = hours;
  int hours24 = 0;
  int fuzzy_minutes = ((minutes + 2) / 5) * 5;

  // Handle hour & minute roll-over.
  if (fuzzy_minutes > 55) {
    fuzzy_minutes = 0;
    fuzzy_hours += 1;
    if (fuzzy_hours > 23) {
      fuzzy_hours = 0;
    }
  }

  size_t remaining = length;
  memset(words, 0, length);

  // si falta un cuarto para cambiar de hora, se suma uno a la hora, para pasar a modalidad de "menos"
  if(fuzzy_minutes >=45)
    fuzzy_hours = (fuzzy_hours + 1) % 24;
  else
    fuzzy_hours = fuzzy_hours % 24;

  // manejo de las 0 horas y 12 horas (medianoche y mediodia)
  if (fuzzy_hours == 0) {
    remaining -= append_string(words, remaining, STR_MIDNIGHT);
  } else if (fuzzy_hours == 12) {
    remaining -= append_string(words, remaining, STR_NOON);
  } else {
    // esta en base a 12 horas
    remaining -= append_number(words, fuzzy_hours % 12);
  }

  remaining -= append_string(words, remaining, " ");

  if (fuzzy_minutes == 0 && !(fuzzy_hours == 0 || fuzzy_hours == 12)) {
    //remaining -= append_string(words, remaining, " ");
    remaining -= append_string(words, remaining, STR_OH_CLOCK);
  }

  if (fuzzy_minutes != 0 && (fuzzy_minutes >= 10 || fuzzy_minutes == 5 || fuzzy_hours == 0 || fuzzy_hours == 12)) {
    
    if (fuzzy_minutes == 15) {
      // manejo de los quince minutos
      remaining -= append_string(words, remaining, STR_AND);
      remaining -= append_string(words, remaining, " ");
      remaining -= append_string(words, remaining, STR_QUARTER);
      remaining -= append_string(words, remaining, " ");
    } 
     
    else if (fuzzy_minutes == 45) {
      // manejo del cuarto de hora previo al cambio de hora
      remaining -= append_string(words, remaining, STR_TO);
      remaining -= append_string(words, remaining, " ");
      remaining -= append_string(words, remaining, STR_QUARTER);
      remaining -= append_string(words, remaining, " ");

      
    } else if (fuzzy_minutes == 30) {
      // manejo de la mitad de hora.
      remaining -= append_string(words, remaining, STR_AND);
      remaining -= append_string(words, remaining, " ");

      if(!(fuzzy_hours == 0 || fuzzy_hours == 12))
        remaining -= append_string(words, remaining, STR_HALF);
      else
        remaining -= append_number(words, fuzzy_minutes);

      remaining -= append_string(words, remaining, " ");
    } 
    else if(fuzzy_minutes == 25) {
      // manejo del caso de veinti-cinco
      remaining -= append_string(words, remaining, STR_AND);
      remaining -= append_string(words, remaining, " ");
      remaining -= append_string(words, remaining, STR_SPECIAL_25);
      remaining -= append_string(words, remaining, " ");
    }
    else if (fuzzy_minutes < 45) {
      // manejo de cada cinco minutos hasta llegar al cuarto antes de cambio hora
      remaining -= append_string(words, remaining, STR_AND);
      remaining -= append_string(words, remaining, " ");
      remaining -= append_number(words, fuzzy_minutes);
      remaining -= append_string(words, remaining, " ");
    } 
      else {
      // manejo de cada cinco minutos luego del cuarto antes de cambio de hora.
      remaining -= append_string(words, remaining, STR_TO);
      remaining -= append_string(words, remaining, " ");
      remaining -= append_number(words, 60 - fuzzy_minutes);
      remaining -= append_string(words, remaining, " ");

      
    }
  }

  // mostrar uso horario solo si no estamos en horas especiales
  if(!(fuzzy_hours == 0 || fuzzy_hours == 12))
  {
    if(fuzzy_minutes >=45)
    {
      if(hours < 24)
        hours24 = (hours + 1);
      else
        hours24 = 0;
    }
    else
      hours24 = hours;
    
    if(hours24 < 12)
      remaining -= append_string(words, remaining, STR_AM);
    else
      remaining -= append_string(words, remaining, STR_PM);
  }
      

  
}
