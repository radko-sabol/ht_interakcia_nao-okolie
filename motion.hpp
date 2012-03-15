#include <iostream>
#include <math.h>

#include <alproxies/almotionproxy.h>
#include <alproxies/albehaviormanagerproxy.h>


class Motion
{
public:
  void center(int x, int y, AL::ALMotionProxy& mp);
  void funkciaPrislusnosti(float *hodnoty, float hodnota);
  void funkciaDaleko(float *hodnoty,float hodnota);
  void funkciaStredne(float *hodnoty,float hodnota);
  void funkciaBlizsko(float *hodnoty,float hodnota);
  float vyhodnoteniePravidiel(const float *hodnotyx);
};