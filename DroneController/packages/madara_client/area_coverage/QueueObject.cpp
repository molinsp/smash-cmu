/*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

/*******************************************************************************
 * QueueObject.cpp - Declares the structures and methods for the
 * algorithm to perform an priority area search
 ******************************************************************************/

#include "QueueObject.h"

SMASH::AreaCoverage::QueueObject::QueueObject(int mid, int mx, int my, double mtotalDistance){
  id = mid;
  x = mx;
  y = my;
  totalDistance = mtotalDistance;
}

SMASH::AreaCoverage::QueueObject::QueueObject(const QueueObject &qo){
  id = qo.id;
  x = qo.x;
  y = qo.y;
  totalDistance = qo.totalDistance;
}

bool SMASH::AreaCoverage::QueueObject::operator<(QueueObject oo) { 
  return totalDistance < oo.totalDistance; 
}
