#define USE_TLS false
#define DEBUG true

#if DEBUG

#define debug(a) (Serial.print(a))
#define debugln(a) (Serial.println(a))

#else

#define debug(a)
#define debugln(a)

#endif