/*
 * =====================================================================================
 *
 *       Filename:  TrackingAction.hh
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  25.09.2013 10:15:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */


#ifndef IBN_LSRP_TRACKING_ACTION
#define IBN_LSRP_TRACKING_ACTION

class G4Track;
#include "G4UserTrackingAction.hh"
#include "globals.hh"

class TrackingAction : public G4UserTrackingAction 
{
  public:
    TrackingAction();
    virtual ~TrackingAction();
   
    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);

  private:
    G4String processName;
};

#endif
