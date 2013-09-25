/*
 * =====================================================================================
 *
 *       Filename:  TrackInformation.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  25.09.2013 10:11:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */


#include "TrackInformation.hh"
#include "G4ios.hh"

G4Allocator<TrackInformation> aTrackInformationAllocator;

TrackInformation::TrackInformation()
{
    originalTrackID = 0;
    particleDefinition = 0;
    originalPosition = G4ThreeVector(0.,0.,0.);
    originalMomentum = G4ThreeVector(0.,0.,0.);
    originalEnergy = 0.;
    originalTime = 0.;
}

TrackInformation::TrackInformation(const G4Track* aTrack)
{
    originalTrackID = aTrack->GetTrackID();
    particleDefinition = aTrack->GetDefinition();
    originalPosition = aTrack->GetPosition();
    originalMomentum = aTrack->GetMomentum();
    originalEnergy = aTrack->GetTotalEnergy();
    originalTime = aTrack->GetGlobalTime();
}

TrackInformation::TrackInformation(const TrackInformation* aTrackInfo)
{
    originalTrackID = aTrackInfo->originalTrackID;
    particleDefinition = aTrackInfo->particleDefinition;
    originalPosition = aTrackInfo->originalPosition;
    originalMomentum = aTrackInfo->originalMomentum;
    originalEnergy = aTrackInfo->originalEnergy;
    originalTime = aTrackInfo->originalTime;
}

TrackInformation::~TrackInformation(){;}

void TrackInformation::Print() const
{
    G4cout 
     << "Original track ID " << originalTrackID 
     << " at " << originalPosition << G4endl;
}
