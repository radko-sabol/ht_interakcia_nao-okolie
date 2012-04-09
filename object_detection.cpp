/**
* DEV
*/

#include "object_detection.hpp"

/**
 * Detekcia kruhov v obraze
 *
 * @param cv::Mat &img - obraz, v ktorom su detegovane kruhy
 */
void ObjectDetection::circleDetectObjects(cv::Mat &img) // TODO: vyladit nastavenie farieb lebo je vela falosnych detekcii
{
  cv::Mat gray, gray2, imageHSV;
  //cvtColor(img, gray, CV_BGR2GRAY);
  cv::cvtColor(img, imageHSV, CV_BGR2HSV);
  cv::inRange(imageHSV, cv::Scalar(0, 50, 170, 0), cv::Scalar(10, 180, 256, 0), gray); // prahovanie jednym rozsahom farieb
  cv::inRange(imageHSV, cv::Scalar(170, 50, 170, 0), cv::Scalar(256, 180, 256, 0), gray2); // prahovanie druhym rozsahom farieb
  cv::bitwise_or(gray, gray2, gray); // spojenie prahovanych obrazov
  // smooth it, otherwise a lot of false circles may be detected
  cv::GaussianBlur(gray, gray, cv::Size(9, 9), 1, 1);
  cv::Canny(gray, gray, 1, 3);
  std::vector<cv::Vec3f> circles;
  //HoughCircles(gray, circles, CV_HOUGH_GRADIENT,
  //             2, gray.rows/4, 200, 100 );

  cv::HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 2, gray.rows/4, 200, 100, 5, 50);
  for (size_t i = 0; i < circles.size(); i++)
  {
    /*
    std::cout << circles[i][2] << std::endl; // radius
    cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    int radius = cvRound(circles[i][2]);
    // draw the circle center
    cv::circle(img, center, 3, cv::Scalar(0,255,0), -1, 8, 0);
    // draw the circle outline
    cv::circle(img, center, radius, cv::Scalar(0,0,255), 3, 8, 0);
    */

    //std::cout << "najdeny kruh" << std::endl;
    m_objects[1].resize(m_objects[1].size()+1);
    m_objects[1][m_objects[1].size()-1].resize(1);
    m_objects[1][m_objects[1].size()-1][0] = cv::Scalar(circles[i][0], circles[i][1], circles[i][2], 0);
  }

  //cv::imshow( "circless", img );
  //cv::imshow( "circles", gray );
}



/**
 * Detektor obdlznikovych objektov v obraze
 *
 * Zdroj: http://stackoverflow.com/questions/7731742/square-detection-doesnt-find-squares
 */
int thresh = 50, N = 2; // karlphillip: decreased N to 2, was 11.

/**
 * Detektor obdlznikovych objektov v obraze - pomocna funkcia
 *   - hlada kosinus medzi vektormi pt0->pt1 a pt0->pt2
 *
 * @param cv::Point pt1
 * @param cv::Point pt2
 * @param cv::Point pt0
 * Zdroj: http://stackoverflow.com/questions/7731742/square-detection-doesnt-find-squares
 */
double ObjectDetection::angle(cv::Point pt1, cv::Point pt2, cv::Point pt0 )
{
  double dx1 = pt1.x - pt0.x;
  double dy1 = pt1.y - pt0.y;
  double dx2 = pt2.x - pt0.x;
  double dy2 = pt2.y - pt0.y;
  return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

/**
 * Detektor obdlznikovych objektov v obraze
 *
 * @param const cv::Mat &image - obraz, v ktorom su detegovane obdlznikove predmety
 * Zdroj: http://stackoverflow.com/questions/7731742/square-detection-doesnt-find-squares
 */
void ObjectDetection::squareDetectObjects(const cv::Mat &image)
{
  /** Deklaracia premennych */
  cv::Mat pyr,
          gray0(image.size(), CV_8U),
          gray;
  //cv::Mat image2 = image.clone(); // naklonovanie vstupneho obrazu
  //CvMat imageCvMat = image2; // konverzia z cv::Mat do CvMat
    
  // karlphillip: dilate the image so this technique can detect the white square,
  CvMat imageCvMat = image; // konverzia z cv::Mat do CvMat
  IplConvKernel *se21 = cvCreateStructuringElementEx(3, 3, 0, 0, CV_SHAPE_RECT, NULL);
  cvDilate(&imageCvMat, &imageCvMat, se21, 1);
  cvReleaseStructuringElement(&se21);
  cv::Mat out(&imageCvMat); // spatna konverzia z CvMat do cv::Mat

  //Mat out(image);
  //dilate(out, out, Mat(), Point(-1,-1)); // original, ale nefunguje - preto som presiel na cvDilate

  // then blur it so that the ocean/sea become one big segment to avoid detecting them as 2 big squares.
  cv::medianBlur(out, out, 7);

  // down-scale and upscale the image to filter out the noise
  cv::pyrDown(out, pyr, cv::Size(out.cols/2, out.rows/2)); // zmensenie obrazku na polovicu
  cv::pyrUp(pyr, out, out.size()); // spatne zvacsenie na plnu velkost obrazka
  std::vector<std::vector<cv::Point>> contours;

  // find squares in every color plane of the image
  for(int c = 0; c < 3; c++)
  {
    int ch[] = {c, 0};
    mixChannels(&out, 1, &gray0, 1, ch, 1);

    // try several threshold levels
    for (int l = 0; l < N; l++)
    {
      // hack: use Canny instead of zero threshold level.
      // Canny helps to catch squares with gradient shading
      if (l == 0)
      {
        // apply Canny. Take the upper threshold from slider
        // and set the lower to 0 (which forces edges merging)
        cv::Canny(gray0, gray, 0, thresh, 5);
        // dilate canny output to remove potential
        // holes between edge segments
        cv::dilate(gray, gray, cv::Mat(), cv::Point(-1,-1));
      }
      else
      {
        // apply threshold if l!=0:
        //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
        gray = gray0 >= (l+1)*255/N;
      }

      // find contours and store them all as a list
      cv::findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

      std::vector<cv::Point> approx;
      std::vector<cv::Scalar> approxS;
      approxS.resize(4);

      // test each contour
      for (size_t i = 0; i < contours.size(); i++)
      {
        // approximate contour with accuracy proportional
        // to the contour perimeter
        cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

        // square contours should have 4 vertices after approximation
        // relatively large area (to filter out noisy contours)
        // and be convex.
        // Note: absolute value of an area is used because
        // area may be positive or negative - in accordance with the
        // contour orientation
        if ((approx.size() == 4) && (fabs(cv::contourArea(cv::Mat(approx))) > 1000) && (cv::isContourConvex(cv::Mat(approx))))
        {
          double maxCosine = 0;

          for (int j = 2; j < 5; j++)
          {
            // find the maximum cosine of the angle between joint edges
            double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
            maxCosine = MAX(maxCosine, cosine);
          }

          for (unsigned int k = 0; k < approx.size(); k++)
          {
            approxS[k] = cv::Scalar(approx[k].x, approx[k].y, 0, 0);
          }
          

          // if cosines of all angles are small
          // (all angles are ~90 degree) then write quandrange
          // vertices to resultant sequence
          if (maxCosine < 0.3)
          { 
            //std::cout << "najdeny stvorec" << std::endl;
            //squares.push_back(approx);
            m_objects[2].push_back(approxS);
          }
        }
      }
    }
  }
  //std::cout << objects.size(); 
  //objects[0][0][0] = cv::Scalar(1, 2, 3, 4);
  //std::cout << " ... " << objects.size() << std::endl;
}

/**
 * Nacitanie haar kaskad
 *
 * @param std::string cascadePath - cesta ku suboru s haar kaskadami
 * @return boolean - indikator, ci boli kaskady uspesne nacitane
 */
boolean ObjectDetection::loadHaarObjectDetector(std::string cascadePath)
{
    qDebug() << QString(cascadePath.c_str());
    if (! m_haarCascade.load(cascadePath)) { qDebug() << "Pri nacitani haar kaskad sa vyskytla chyba"; return false; } else { qDebug() << "Haar kaskady uspesne nacitane"; return true; } // C++
}

/**
 * Haar detektor objektov
 *
 * @param cv::Mat image - obraz, v ktorom su hladane objekty z haar kaskady
 * Zdroj: http://opencv.itseez.com/doc/tutorials/objdetect/cascade_classifier/cascade_classifier.html
 */
void ObjectDetection::haarDetectObjects(cv::Mat image)
{
  std::vector<cv::Rect> foundObjects;
  cv::Mat imageGray;

  cv::cvtColor(image, imageGray, CV_BGR2GRAY);
  cv::equalizeHist(imageGray, imageGray);
  
  m_haarCascade.detectMultiScale(imageGray, foundObjects, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(7, 7));

  for (size_t i = 0; i < foundObjects.size(); i++)
  {
    m_objects[0].resize(m_objects[0].size()+1); // alokacia miesta pre najdeny objekt
    m_objects[0][m_objects[0].size()-1].resize(1); // alokacia miesta pre suradnice objektu (zapisuje sa len do prveho prvku, ale potrebujem mat taketo pole aby som vedel pri hladani obdlznikov zapisovat suradnice vsetkcyh rohov (nie je tam 90°, tak musim kazdy bod osobitne))
    m_objects[0][m_objects[0].size()-1][0] = cv::Scalar(foundObjects[i].x, foundObjects[i].y, foundObjects[i].width, foundObjects[i].height);
  }
}

/**
 * Zakreslenie najdenych objektov (premenna m_objects) do obrazu imageMat
 *
 * @param cv::Mat &imageMat - obraz, do ktoreho sa zakreslia najdene objekty
 */
void ObjectDetection::drawDetectedObjects(cv::Mat &imageMat)
{
  for (size_t i = 0; i < m_objects.size(); i++) // ID identifikatora
  {
    for (size_t j = 0; j < m_objects[i].size(); j++) // najdene objekty
    {
      if (i == 0) // detegovany haar objekt
      {
        cv::rectangle(imageMat, cv::Point(m_objects[i][j][0][0], m_objects[i][j][0][1]), cv::Point(m_objects[i][j][0][0] + m_objects[i][j][0][2], m_objects[i][j][0][1] + m_objects[i][j][0][3]), cv::Scalar(0, 255, 0), 3, 8, 0);
        cv::Point center(m_objects[i][j][0][0] + m_objects[i][j][0][2]*0.5, m_objects[i][j][0][1] + m_objects[i][j][0][3]*0.5);
        cv::circle(imageMat, center, 1, cv::Scalar(255, 0, 0), 3, 8, 0);
        //std::cout << "X: " << center.x << " ; Y: " << center.y << std::endl;
          
        /*
	      if ((center.x > 0) && (center.y > 0))
        {
          motion.center(center.x, center.y, mp); // TODO: potrebne urobit mp ako globalnu premennu A presunut volanie tejto funkcie na lepsie miesto
        }
        */
          
      }

      if (i == 1) // detegovany kruh
      {
        cv::Point circleCenter(cvRound(m_objects[i][j][0][0]), cvRound(m_objects[i][j][0][1]));
        int circleRadius = cvRound(m_objects[i][j][0][2]);

        cv::circle(imageMat, circleCenter, circleRadius, cv::Scalar(0, 0, 255), 3, 8, 0);
      }

      if (i == 2) // detegovany stvorec
      {
        const cv::Scalar p1 = m_objects[i][j][0];
        const cv::Scalar p2 = m_objects[i][j][1];
        const cv::Scalar p3 = m_objects[i][j][2];
        const cv::Scalar p4 = m_objects[i][j][3];

        /*
        std::cout << "P1 - X: " << p1[0] << " ;Y: " << p1[1] << " ;?: " << p1[2] << " ;?: " << p1[3] << std::endl;
        std::cout << "P2 - X: " << p2[0] << " ;Y: " << p2[1] << " ;?: " << p2[2] << " ;?: " << p2[3] << std::endl;
        std::cout << "P3 - X: " << p3[0] << " ;Y: " << p3[1] << " ;?: " << p3[2] << " ;?: " << p3[3] << std::endl;
        std::cout << "P4 - X: " << p4[0] << " ;Y: " << p4[1] << " ;?: " << p4[2] << " ;?: " << p4[3] << std::endl;
        */

        /*
        // Vypocet stredoveho bodu
        int x = 0,
            y = 0;

        x = ((squares[i][0].x + squares[i][2].x)/2 + (squares[i][1].x + squares[i][3].x)/2) / 2;
        y = ((squares[i][0].y + squares[i][2].y)/2 + (squares[i][1].y + squares[i][3].y)/2) / 2;

        cv::circle(image, cv::Point(squares[i][0].x, squares[i][0].y), 2, cv::Scalar(0, 0, 255), 1);
        //cv::circle(image, cv::Point(squares[i][1].x, squares[i][1].y), 2, cv::Scalar(255, 0, 0), 1);
        cv::circle(image, cv::Point(squares[i][2].x, squares[i][2].y), 2, cv::Scalar(255, 0, 255), 1);
        cv::circle(image, cv::Point(x, y), 2, cv::Scalar(255, 0, 0), 1);
        */

        cv::line(imageMat, cv::Point(p1[0], p1[1]), cv::Point(p2[0], p2[1]), cv::Scalar(255, 0, 0), 2);
        cv::line(imageMat, cv::Point(p2[0], p2[1]), cv::Point(p3[0], p3[1]), cv::Scalar(255, 0, 0), 2);
        cv::line(imageMat, cv::Point(p3[0], p3[1]), cv::Point(p4[0], p4[1]), cv::Scalar(255, 0, 0), 2);
        cv::line(imageMat, cv::Point(p4[0], p4[1]), cv::Point(p1[0], p1[1]), cv::Scalar(255, 0, 0), 2);
      }
    }
  }

  // zobrazenie obrazu s detegovanymi objektmi
  //cv::imshow("Video detected", imageMat);
}

/**
 *
 */
std::vector<std::vector<cv::Scalar>> ObjectDetection::getObjects(int index)
{
  return m_objects[index];
}

/**
 * Vyprazdnenie premennej obsahujucej informacie o najdenych objektoch
 */
void ObjectDetection::clearObjects()
{
    m_objects.clear();
    m_objects.resize(3);
}
