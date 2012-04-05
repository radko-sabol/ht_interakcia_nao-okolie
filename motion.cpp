#include "motion.hpp"

void Motion::center(int x, int y, AL::ALMotionProxy& mp)
{
	//mp.setAngles("HeadYaw",0,0.05);
	//mp.setAngles("HeadPitch",0,0.05);
	
	//float a=0.5;
	float b=0;
	float theta=0;
	float frequency=0.5;
	char c;

	float nchange_x = 0;
    float nchange_y = 0;
    float mnozina_x[3];
    float mnozina_y[3];

    if(x < 160)
         nchange_x = (160 - x) / 160;
    else nchange_x = (x - 160) / 160;

    if(y < 120)
         nchange_y = (120 - y) / 120;
    else nchange_y = (y - 120) / 120;

    funkciaPrislusnosti(mnozina_x, nchange_x);
    funkciaPrislusnosti(mnozina_y, nchange_y);

    float change_x=vyhodnoteniePravidiel(mnozina_x) * 0.1; //change in gradian
    float change_y=vyhodnoteniePravidiel(mnozina_y) * 0.1;

	double distance;
	std::vector<float> position_x = mp.getAngles("HeadYaw",true);	//useSensors – If true, sensor angles will be returned
	std::vector<float> position_y = mp.getAngles("HeadPitch",true);	
	//if ((position_x>-2)&&(position_x<2))

	for(int i = 0; i < position_y.size(); i++)
		distance = (pow(1.0+tan(position_y[0]),-1) * 0.465);

	
		std::cout << " vzdialenost: " << distance << std::endl;
		if(x < 160)
		{		
			mp.changeAngles("HeadYaw", change_x, 0.05f);
		}
		else 
		{
			mp.changeAngles("HeadYaw", -change_x, 0.05f);
		}
		if(y < 120)
			mp.changeAngles("HeadPitch", -change_y,0.05f);
		else mp.changeAngles("HeadPitch", change_y,0.05f);	
	//}
		float ndistance = distance*2-0.6f;
		float mnozina_d[3];
		funkciaPrislusnosti(mnozina_d,ndistance);
		float a = vyhodnoteniePravidiel(mnozina_d);
		theta=position_x[0]/4;				//maximalna zmena 2
	if(distance > 0.31)	
	{
		std::cout << "velkost kroku: " << a <<"theta: " <<theta <<std::endl; 
		mp.post.setWalkTargetVelocity(a, b, theta, frequency);
	}
	else mp.post.setWalkTargetVelocity(0, 0, 0, 0);

}

void Motion::funkciaPrislusnosti(float *hodnoty, float hodnota)
{
    funkciaBlizsko(hodnoty,hodnota);
    funkciaStredne(hodnoty,hodnota);
    funkciaDaleko(hodnoty,hodnota);
}

void Motion::funkciaDaleko(float *hodnoty,float hodnota)
{
    hodnoty[0]=(1-2*hodnota);
}
void Motion::funkciaStredne(float *hodnoty,float hodnota)
{
    if(hodnota<0.5)
        hodnoty[1]=2*hodnota;
    else
        hodnoty[1]=2-2*hodnota;
}
void Motion::funkciaBlizsko(float *hodnoty,float hodnota)
{
    hodnoty[2]=2*hodnota-1;
}

float Motion::vyhodnoteniePravidiel(const float *hodnotyx)
{
    float vystup[3]={0,0,0};

    if(hodnotyx[0]>0)
    {
        //std::cout << "odpal 1";
        vystup[0]=hodnotyx[0]*0.05f;
    }
    if(hodnotyx[1]>0)
    {
        //std::cout << "odpal 2";
        vystup[1]=hodnotyx[1]*0.45f;
    }
    if(hodnotyx[2]>0)
    {
        //std::cout << "odpal 3";
        vystup[2]=hodnotyx[2]*0.95f;
    }
return vystup[0]+vystup[1]+vystup[2];
}
