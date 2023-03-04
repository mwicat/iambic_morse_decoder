#ifndef MORSE_DECODER_HPP
#define MORSE_DECODER_HPP

typedef unsigned long (*millis_fun_ptr)();
typedef void (*letter_decoded_fun_ptr)(char);

enum SignalType
{
  SIGNAL_DOT,
  SIGNAL_DASH,
};

class MorseDecoder
{
public:
  MorseDecoder(unsigned long decode_threshold, letter_decoded_fun_ptr letter_decoded_cb, millis_fun_ptr millis_cb);
  void appendDash();
  void appendDot();
  void append(SignalType signal_type);
  void tick();
  void decode();
  void clearLetterBuf();
  void setTransmitting(bool transmitting);
  bool isDecodeExpired();

private:
  bool transmitting;
  millis_fun_ptr millis_cb;
  unsigned long last_input_time = 0;
  unsigned long int decode_threshold;
  letter_decoded_fun_ptr letter_decoded_cb;
  unsigned short letters = 0;
  int letter_cnt = 0;
};

#endif
