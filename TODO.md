# TODO

- Work around Microsoft Visual Studio bug.  It support C11 and C17,
  but treats `char` and `signed char` as the same type.  Use a
  predefined macro to remove one or the other from the generic selection.\
  Reference: https://developercommunity2.visualstudio.com/t/_Generic-char-signed-char-unsigned-cha/1228885?preview=true
- Record any predefined `ENABLE_*` and DISABLE_*` macros in the output.
- Get results for more implementations.
