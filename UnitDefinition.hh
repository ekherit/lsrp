/*
 * =====================================================================================
 *
 *       Filename:  UnitDefinition.hh
 *
 *    Description:  Unit Definition
 *
 *        Version:  1.0
 *        Created:  14.01.2014 15:28:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (), I.B.Nikolaev@inp.nsk.su
 *        Company:  Budker Institute of Nuclear Physics, Novosibirsk, Russia
 *
 * =====================================================================================
 */

#ifndef IBN_LSRP_UNIT_DEFINITION_HH
#define IBN_LSRP_UNIT_DEFINITION_HH

#include <G4UnitsTable.hh>

static void DefineUnits(void)
{
  new G4UnitDefinition("hertz","Hz","Frequency",1./second);
  new G4UnitDefinition("kilohertz","kHz","Frequency",1e3/second);
  new G4UnitDefinition("megahertz","MHz","Frequency",1e6/second);
  new G4UnitDefinition("gigahertz","GHz","Frequency",1e9/second);
  new G4UnitDefinition("microjoule","mkJ","Energy",1e-6*joule);
}

#endif
