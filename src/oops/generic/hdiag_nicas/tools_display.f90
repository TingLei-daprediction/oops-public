!----------------------------------------------------------------------
! Module: tools_display
!> Purpose: display variables
!> <br>
!> Author: Benjamin Menetrier
!> <br>
!> Licensing: this code is distributed under the CeCILL-C license
!> <br>
!> Copyright © 2017 METEO-FRANCE
!----------------------------------------------------------------------
module tools_display

use iso_c_binding
use tools_kinds, only: kind_real
use type_mpl, only: mpl,mpl_abort

implicit none

! Display colors
character(len=1024) :: black     !< Black color code
character(len=1024) :: green     !< Green color code
character(len=1024) :: peach     !< Peach color code
character(len=1024) :: aqua      !< Aqua color code
character(len=1024) :: purple    !< Purple color code
character(len=1024) :: err       !< Error color code
character(len=1024) :: wng       !< Warning color code

! Vertical unit
character(len=1024) :: vunitchar !< Vertical unit

! Progression display
integer :: ddis                  !< Progression display step

private
public :: black,green,peach,aqua,purple,err,wng,vunitchar,ddis
public :: listing_setup,msgerror,msgwarning,prog_init,prog_print

contains

!----------------------------------------------------------------------
! Subroutine: listing_setup
!> Purpose: setup listing
!----------------------------------------------------------------------
subroutine listing_setup(colorlog,logpres)

implicit none

! Passed variables
logical,intent(in) :: colorlog !< Color listing flag
logical,intent(in) :: logpres  !< Vertical unit flag

! Local variables
character(len=4) :: myprocchar

! Setup display colors
if (colorlog) then
   black = char(27)//'[0;0m'
   green = char(27)//'[0;32m'
   peach = char(27)//'[1;91m'
   aqua = char(27)//'[1;36m'
   purple = char(27)//'[1;35m'
   err = char(27)//'[0;37;41;1m'
   wng = char(27)//'[0;37;42;1m'
else
   black = ' '
   green = ' '
   peach = ' '
   aqua = ' '
   purple = ' '
   err = ' '
   wng = ' '
end if
ddis = 5

! Vertical unit
if (logpres) then
   vunitchar = 'log(Pa)'
else
   vunitchar = 'lev.'
end if

! Open listing files
write(myprocchar,'(i4.4)') mpl%myproc-1
if ((mpl%main.and..not.colorlog).or..not.mpl%main) &
 & open(unit=mpl%unit,file='hdiag_nicas.out.'//myprocchar,action='write',status='replace')

end subroutine listing_setup

!----------------------------------------------------------------------
! Subroutine: msgerror
!> Purpose: print error message and stop
!----------------------------------------------------------------------
subroutine msgerror(message)

implicit none

! Passed variables
character(len=*),intent(in) :: message !< Message

! Clean MPL abort
call mpl_abort(trim(err)//'!!! Error: '//trim(message)//trim(black))

end subroutine msgerror

!----------------------------------------------------------------------
! Subroutine: msgwarning
!> Purpose: print warning message
!----------------------------------------------------------------------
subroutine msgwarning(message)

implicit none

! Passed variables
character(len=*),intent(in) :: message !< Message

! Print warning message
write(mpl%unit,'(a)') trim(wng)//'!!! Warning: '//trim(message)//trim(black)

end subroutine msgwarning

!----------------------------------------------------------------------
! Subroutine: prog_init
!> Purpose: initialize progression display
!----------------------------------------------------------------------
subroutine prog_init(progint,done)

implicit none

! Passed variables
integer,intent(out) :: progint                    !< Progression integer
logical,dimension(:),intent(out),optional :: done !< Progression logical array

! Print message
write(mpl%unit,'(i3,a)',advance='no') 0,'%'

! Initialization
progint = ddis
if (present(done)) done = .false.

end subroutine prog_init

!----------------------------------------------------------------------
! Subroutine: prog_print
!> Purpose: print progression display
!----------------------------------------------------------------------
subroutine prog_print(progint,done)

implicit none

! Passed variables
integer,intent(inout) :: progint        !< Progression integer
logical,dimension(:),intent(in) :: done !< Progression logical array

! Local variables
real(kind_real) :: prog

! Print message
prog = 100.0*float(count(done))/float(size(done))
if (int(prog)>progint) then
   if (progint<100) write(mpl%unit,'(i3,a)',advance='no') progint,'% '
   progint = progint+ddis
end if

end subroutine prog_print

end module tools_display
