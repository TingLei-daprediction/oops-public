!----------------------------------------------------------------------
! Module: driver_obsgen
!> Purpose: generate random observations locations
!> <br>
!> Author: Benjamin Menetrier
!> <br>
!> Licensing: this code is distributed under the CeCILL-C license
!> <br>
!> Copyright © 2017 METEO-FRANCE
!----------------------------------------------------------------------
module driver_obsgen

use tools_const, only: pi
use type_geom, only: geomtype
use type_mpl, only: mpl,mpl_bcast
use type_nam, only: namtype
use type_odata, only: odatatype
use type_randgen, only: rand_real

implicit none

private
public :: run_obsgen

contains

!----------------------------------------------------------------------
! Subroutine: run_obsgen
!> Purpose: generate random observations locations
!----------------------------------------------------------------------
subroutine run_obsgen(nam,geom,odata)

implicit none

! Passed variables
type(namtype),target,intent(in) :: nam   !< Namelist
type(geomtype),target,intent(in) :: geom !< Geometry
type(odatatype),intent(inout) :: odata   !< Observation operator data

if (nam%new_obsop) then
   ! Define number of observations
   odata%nobs = nam%nobs

   ! Allocation
   allocate(odata%lonobs(odata%nobs))
   allocate(odata%latobs(odata%nobs))

   ! Generate random observation network
   if (mpl%main) then
      call rand_real(minval(geom%lon),maxval(geom%lon),odata%lonobs)
      call rand_real(minval(geom%lat),maxval(geom%lat),odata%latobs)
   end if
   call mpl_bcast(odata%lonobs,mpl%ioproc)
   call mpl_bcast(odata%latobs,mpl%ioproc)

   ! Print results
   write(mpl%unit,'(a7,a,i8)') '','Number of observations: ',odata%nobs
end if

end subroutine run_obsgen

end module driver_obsgen
