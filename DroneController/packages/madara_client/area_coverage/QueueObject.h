 /*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

/*******************************************************************************
 * QueueObject.h - Declares the structures and methods for the
 * algorithm to perform a prioritized area search
 ******************************************************************************/

#ifndef _QUEUE_OBJECT_
#define _QUEUE_OBJECT_

namespace SMASH { namespace AreaCoverage {

class QueueObject
{
	
  public:
    int id;
    int x;
    int y;
    double totalDistance;

  public:
    QueueObject(int id, int mx, int my, double mtotalDistance);
    QueueObject(const QueueObject &qo);

    bool operator<(QueueObject oo);

};
}}
#endif // _QUEUE_OBJECT_
