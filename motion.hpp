#include <iostream>
#include <math.h>

#include <alproxies/almotionproxy.h>
#include <alproxies/albehaviormanagerproxy.h>


class Motion
{
public:
  void headCenter(int x, int y, AL::ALMotionProxy &mp);
  void walkToObject(AL::ALMotionProxy &mp);
  void stopWalking(AL::ALMotionProxy &mp);

private:
  void funkciaPrislusnosti(float *hodnoty, float hodnota);
  void funkciaDaleko(float *hodnoty, float hodnota);
  void funkciaStredne(float *hodnoty, float hodnota);
  void funkciaBlizko(float *hodnoty, float hodnota);
  float vyhodnoteniePravidiel(const float *hodnotyx);
};
