#include "motion.hpp"

/**
 *Centrovanie hlavy k sledovanemu objektu
 * 
 * @param int x - x_ova suradnica objektu v obraze
 * @param int y - y_ova suradnica objektu v obraze
 * @param AL::ALMotionProxy& mp - Nao motion proxy 
 */
void Motion::headCenter(int x, int y, AL::ALMotionProxy &mp)
{
  //mp.setAngles("HeadYaw",0,0.05);
  //mp.setAngles("HeadPitch",0,0.05);

  float nchange_x = 0;
  float nchange_y = 0;
  float mnozina_x[3];
  float mnozina_y[3];

  if (x < 160) { nchange_x = (float)(160 - x) / 160; }
  else         { nchange_x = (float)(x - 160) / 160; }

  if (y < 120) { nchange_y = (float)(120 - y) / 120; }
  else         { nchange_y = (float)(y - 120) / 120; }

  funkciaPrislusnosti(mnozina_x, nchange_x);
  funkciaPrislusnosti(mnozina_y, nchange_y);

  float change_x = (float)(vyhodnoteniePravidiel(mnozina_x) * 0.1); // change in radian
  float change_y = (float)(vyhodnoteniePravidiel(mnozina_y) * 0.1); // pretypovavam lebo: motion.cpp:32: warning: C4244: 'initializing' : conversion from 'double' to 'float', possible loss of data



  //std::cout << " vzdialenost: " << distance << std::endl;
  if (x < 160) { mp.changeAngles("HeadYaw", change_x, 0.05); }
  else         { mp.changeAngles("HeadYaw", -change_x, 0.05); }

  if (y < 120) { mp.changeAngles("HeadPitch", -change_y,0.05); }
  else         { mp.changeAngles("HeadPitch", change_y,0.05); }


}

void Motion::walkToObject(AL::ALMotionProxy &mp)
{
     float b=0;
     float theta=0;
     float frequency=0.5;
     double distance; // potrebne ku chodzi
     std::vector<float> position_x = mp.getAngles("HeadYaw", true);	//useSensors – If true, sensor angles will be returned
     std::vector<float> position_y = mp.getAngles("HeadPitch", true);
     for (unsigned int i = 0; i < position_y.size(); i++)
     {
         distance = (pow(1.0+tan(position_y[0]),-1) * 0.465);
     }

      // chodza
      float ndistance = (float)(distance * 2 - 0.6f);
      float mnozina_d[3];
      funkciaPrislusnosti(mnozina_d,ndistance);
      float a = vyhodnoteniePravidiel(mnozina_d);
      theta = position_x[0]/4;				//maximalna zmena 2
      if (distance > 0.31)
      {
         std::cout << "velkost kroku: " << a <<"theta: " <<theta <<std::endl;
         mp.post.setWalkTargetVelocity(a, b, theta, frequency);
      }
      else { mp.post.setWalkTargetVelocity(0, 0, 0, 0); }
}

void Motion::stopWalking(AL::ALMotionProxy &mp)
{
  mp.post.setWalkTargetVelocity(0, 0, 0, 0);
}

/**
 * Priradenie funkcii prislusnosti k lingvistickym premennym
 * 
 * @param float *hodnoty - mnozina hodnot prislusnosti
 * @param float hodnota - premenna vzdialenosti (stredu zorneho uhla, objektu)
 */
void Motion::funkciaPrislusnosti(float *hodnoty, float hodnota)
{
    funkciaBlizko(hodnoty,hodnota);
    funkciaStredne(hodnoty,hodnota);
    funkciaDaleko(hodnoty,hodnota);
}

/**
 * Funkcia prislusnosti k lingvistickej premmenej daleko
 * 
 * @param float *hodnoty - mnozina hodnot prislusnosti
 * @param float hodnota - premenna vzdialenosti (stredu zorneho uhla, objektu)
 */
void Motion::funkciaDaleko(float *hodnoty,float hodnota)
{
    hodnoty[0]=(1-2*hodnota);
}

/**
 * Funkcia prislusnosti k lingvistickej premmenej stredne
 * 
 * @param float *hodnoty - mnozina hodnot prislusnosti
 * @param float hodnota - premenna vzdialenosti (stredu zorneho uhla, objektu)
 */
void Motion::funkciaStredne(float *hodnoty,float hodnota)
{
    if (hodnota < 0.5) { hodnoty[1] = (2 * hodnota); }
    else               { hodnoty[1] = (2 - 2 * hodnota); }
}

/**
 * Funkcia prislusnosti k lingvistickej premmenej blizko
 * 
 * @param float *hodnoty - mnozina hodnot prislusnosti
 * @param float hodnota - premenna vzdialenosti (stredu zorneho uhla, objektu)
 */
void Motion::funkciaBlizko(float *hodnoty,float hodnota)
{
    hodnoty[2] = (2 * hodnota - 1);
}

/**
 * Vyhodnotenie fuzzy pravidiel
 * 
 * @param const float*hodnotyx -  mnozina hodnot prislusnosti k lingvistickym premennym
 */
float Motion::vyhodnoteniePravidiel(const float *hodnotyx)
{
  float vystup[3] = {0, 0, 0};

  if (hodnotyx[0] > 0)
  {
    //std::cout << "odpal 1";
    vystup[0] = (hodnotyx[0] * 0.05f);
  }

  if (hodnotyx[1] > 0)
  {
    //std::cout << "odpal 2";
    vystup[1] = (hodnotyx[1] * 0.45f);
  }

  if (hodnotyx[2] > 0)
  {
    //std::cout << "odpal 3";
    vystup[2] = (hodnotyx[2] * 0.95f);
  }

  return vystup[0]+vystup[1]+vystup[2];
}
