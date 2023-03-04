#include "MorseDecoder.hpp"

char *binString(unsigned short n)
{
  static char bin[17];
  int x;

  for (x = 0; x < 16; x++)
  {
    bin[x] = n & 0x8000 ? '1' : '0';
    n <<= 1;
  }
  bin[16] = '\0';

  return (bin);
}

MorseDecoder::MorseDecoder(unsigned long decode_threshold, letter_decoded_fun_ptr letter_decoded_cb, millis_fun_ptr millis_cb)
{
  this->decode_threshold = decode_threshold;
  this->letter_decoded_cb = letter_decoded_cb;
  this->millis_cb = millis_cb;
}

void MorseDecoder::setTransmitting(bool transmitting)
{
  this->transmitting = transmitting;
}

bool MorseDecoder::isDecodeExpired()
{
  if (last_input_time == 0 || transmitting)
  {
    return false;
  }
  else
  {
    return (millis_cb() - last_input_time) > decode_threshold;
  }
}

void MorseDecoder::tick()
{
  if (isDecodeExpired())
  {
    decode();
    last_input_time = 0;
  }
}

void MorseDecoder::decode()
{
  char letter;
  switch (letters)
  {
  case 0b110:
    letter = 'a';
    break;
  case 0b10001:
    letter = 'b';
    break;
  case 0b10101:
    letter = 'c';
    break;
  case 0b1001:
    letter = 'd';
    break;
  case 0b10:
    letter = 'e';
    break;
  case 0b10100:
    letter = 'f';
    break;
  case 0b1011:
    letter = 'g';
    break;
  case 0b10000:
    letter = 'h';
    break;
  case 0b100:
    letter = 'i';
    break;
  case 0b11110:
    letter = 'j';
    break;
  case 0b1101:
    letter = 'k';
    break;
  case 0b10010:
    letter = 'l';
    break;
  case 0b111:
    letter = 'm';
    break;
  case 0b101:
    letter = 'n';
    break;
  case 0b1111:
    letter = 'o';
    break;
  case 0b10110:
    letter = 'p';
    break;
  case 0b11011:
    letter = 'q';
    break;
  case 0b1010:
    letter = 'r';
    break;
  case 0b1000:
    letter = 's';
    break;
  case 0b11:
    letter = 't';
    break;
  case 0b1100:
    letter = 'u';
    break;
  case 0b11000:
    letter = 'v';
    break;
  case 0b1110:
    letter = 'w';
    break;
  case 0b11001:
    letter = 'x';
    break;
  case 0b11101:
    letter = 'y';
    break;
  case 0b10011:
    letter = 'z';
    break;
  case 0b111110:
    letter = '1';
    break;
  case 0b111100:
    letter = '2';
    break;
  case 0b111000:
    letter = '3';
    break;
  case 0b110000:
    letter = '4';
    break;
  case 0b100000:
    letter = '5';
    break;
  case 0b100001:
    letter = '6';
    break;
  case 0b100011:
    letter = '7';
    break;
  case 0b100111:
    letter = '8';
    break;
  case 0b101111:
    letter = '9';
    break;
  case 0b111111:
    letter = '0';
    break;
  default:
    letter = '?';
  }
  letter_decoded_cb(letter);
  clearLetterBuf();
}

void MorseDecoder::clearLetterBuf()
{
  letter_cnt = 0;
  letters = 0;
}

void MorseDecoder::appendDash()
{
  append(SIGNAL_DASH);
}

void MorseDecoder::appendDot()
{
  append(SIGNAL_DOT);
}

void MorseDecoder::append(SignalType signal_type)
{
  letters &= ~(1 << letter_cnt);
  if (signal_type == SIGNAL_DASH)
  {
    letters |= (1 << letter_cnt);
  }
  letters |= (1 << (letter_cnt + 1));
  letter_cnt++;
  last_input_time = millis_cb();
}
