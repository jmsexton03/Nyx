
! This module stores the extra parameters for the VODE calls.

module vode_aux_module

  use amrex_fort_module, only : rt => amrex_real
!  use cudafor
  implicit none

  real(rt), allocatable :: z_vode
  real(rt), allocatable :: rho_vode, T_vode, ne_vode
  real(rt) :: rho_init_vode, rho_src_vode, rhoe_src_vode, e_src_vode
  real(rt), dimension(:), allocatable, save :: rho_vode_vec, T_vode_vec, ne_vode_vec
  integer , allocatable :: JH_vode, JHe_vode, i_vode, j_vode, k_vode, fn_vode, NR_vode
  logical,  allocatable :: firstcall
#ifdef AMREX_USE_CUDA
  attributes(managed) :: z_vode, NR_vode, T_vode, ne_vode, rho_vode, JH_vode, JHe_vode, i_vode, j_vode, k_vode, fn_vode
#endif
  !$OMP THREADPRIVATE (rho_vode, rho_vode_vec, T_vode, T_vode_vec, ne_vode, ne_vode_vec, JH_vode, JHe_vode, i_vode, j_vode, k_vode, fn_vode, NR_vode, firstcall)

end module vode_aux_module
